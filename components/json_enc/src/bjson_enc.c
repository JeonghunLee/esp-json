#include "bjson_enc.h"
#include "bjson_enc_internal.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ARENA_CAP (4096)
static int is_ident0(int c){ return isalpha(c) || (c=='_'); }
static int is_ident(int c){ return isalnum(c) || (c=='_'); }

/**
 * @brief Skip whitespace in the parser context.
 *
 * Advances `p->pos` past spaces, tabs, CR and LF.
 *
 * @param p Parser context.
 * @return Always returns 0 (helper convenience).
 */
static int  ws(pctx_t* p){ while (p->pos<p->len && (p->json[p->pos]==' '||p->json[p->pos]=='\t'||p->json[p->pos]=='\r'||p->json[p->pos]=='\n')) p->pos++; return 0; }
/**
 * @brief Peek current character or -1 if at end.
 *
 * @param p Parser context.
 * @return Current character as unsigned char or -1 when at end.
 */
static int  ch(pctx_t* p){ return (p->pos<p->len)? (unsigned char)p->json[p->pos] : -1; }
/**
 * @brief Consume the given character if it is next (after skipping ws).
 *
 * @param p Parser context.
 * @param c Character to match and consume.
 * @return 1 if consumed, 0 otherwise.
 */
static int  eat(pctx_t* p, char c){ ws(p); if (ch(p)==c){ p->pos++; return 1; } return 0; }

static void* a_alloc(pctx_t* p, size_t n){
  if (p->arena_used + n > p->arena_cap) return NULL;
  void* r = p->arena + p->arena_used; p->arena_used += n; return r;
}
static char* a_dup(pctx_t* p, const char* s, size_t n){
  char* r = (char*)a_alloc(p, n+1); if(!r) return NULL; memcpy(r,s,n); r[n]=0; return r;
}

/**
 * @brief Parse a string or unquoted identifier and duplicate into arena.
 *
 * Handles quoted strings with simple escape skipping. For unquoted
 * identifiers it accepts [A-Za-z_][A-Za-z0-9_]*.
 *
 * @param p Parser context.
 * @param ok[out] Set to 1 on success, 0 on parse failure.
 * @return Pointer into arena containing NUL-terminated string on
 *         success, or NULL on allocation/parse error.
 */
static char* parse_string(pctx_t* p, int* ok){
  *ok=0; ws(p);
  if (eat(p,'"')){ // quoted
    size_t start = p->pos;
    while (p->pos < p->len){
      char c = p->json[p->pos++];
      if (c=='"'){ size_t n = (p->pos-1) - start; char* s=a_dup(p,&p->json[start],n); if(!s) return NULL; *ok=1; return s; }
      if (c=='\\'){ if (p->pos>=p->len) return NULL; p->pos++; } // simple escape skip
    }
    return NULL;
  }
  // unquoted identifier
  if (!is_ident0(ch(p))) return NULL;
  size_t start = p->pos++;
  while (p->pos<p->len && is_ident(ch(p))) p->pos++;
  *ok=1; return a_dup(p, &p->json[start], p->pos-start);
}

/**
 * @brief Parse an integer literal from the current position.
 *
 * Supports optional +/-, and returns 0 on parse failure.
 *
 * @param p Parser context.
 * @param out[out] Parsed integer on success.
 * @return 1 on success, 0 on failure.
 */
static int parse_int(pctx_t* p, long long* out){
  ws(p);
  size_t start = p->pos; int c=ch(p);
  if (c=='-' || c=='+'){ p->pos++; }
  if (!isdigit(ch(p))) return 0;
  while (isdigit(ch(p))) p->pos++;
  // no trailing alpha
  size_t n = p->pos - start;
  char buf[32]; if (n>=sizeof(buf)) return 0;
  memcpy(buf, &p->json[start], n); buf[n]=0;
  char* endp=NULL; long long v = strtoll(buf,&endp,10);
  if (!endp || *endp!=0) return 0;
  *out = v; return 1;
}

static int prefix_match(const char* key, const char* pref){ size_t k=strlen(pref); return strncmp(key,pref,k)==0; }

static int classify_key(const char* key, ast_type_t* t, int* smax, int* isz){
  if (prefix_match(key,"STR_32_"))  { *t=AST_T_STR; *smax=32;  return 1; }
  if (prefix_match(key,"STR_64_"))  { *t=AST_T_STR; *smax=64;  return 1; }
  if (prefix_match(key,"STR_128_")) { *t=AST_T_STR; *smax=128; return 1; }
  if (prefix_match(key,"STR_256_")) { *t=AST_T_STR; *smax=256; return 1; }
  if (prefix_match(key,"INT16_"))   { *t=AST_T_I16; *isz=2;    return 1; }
  if (prefix_match(key,"UINT16_"))  { *t=AST_T_U16; *isz=2;    return 1; }
  if (prefix_match(key,"INT32_"))   { *t=AST_T_I32; *isz=4;    return 1; }
  if (prefix_match(key,"UINT32_"))  { *t=AST_T_U32; *isz=4;    return 1; }
  return 0;
}

static int push_kv(pctx_t* p, ast_kv_t kv){
  ast_kv_t* node = (ast_kv_t*)a_alloc(p, sizeof(ast_kv_t));
  if (!node) return 0;
  *node = kv; node->next=NULL;
  if (!p->head) p->head=node; else p->tail->next=node; p->tail=node;
  return 1;
}

/**
 * @brief Parse a single object member (key:value) into the AST.
 *
 * Validates key format via `classify_key` and performs range checks
 * for integer types. On allocation or parse error `*err` is set and
 * the function returns 0.
 *
 * @param p Parser context.
 * @param err[out] Non-zero on error.
 * @return 1 on success, 0 on parse or allocation error.
 */
static int parse_member(pctx_t* p, int* err){
  *err=0;
  int ok=0; char* key = parse_string(p,&ok); if (!ok || !key){ *err=1; return 0; }
  if (!eat(p,':')){ *err=1; return 0; }

  ast_type_t t=0; int smax=0, isz=0;
  if (!classify_key(key,&t,&smax,&isz)){ *err=1; return 0; }

  ast_kv_t kv = {0}; kv.type=t; kv.key=key; kv.smax=smax; kv.isz=isz;

  if (t==AST_T_STR){
    int ok2=0; char* sval = parse_string(p,&ok2);
    if (!ok2 || !sval){ *err=1; return 0; }
    // UTF-8 바이트 수 기준
    if ((int)strlen(sval) > smax){ *err=1; return 0; }
    kv.sval = sval;
  } else {
    long long v=0; if (!parse_int(p,&v)){ *err=1; return 0; }
    // 범위 체크
    switch (t){
      case AST_T_I16: if (v < -32768 || v > 32767) { *err=1; return 0; } break;
      case AST_T_U16: if (v < 0 || v > 65535)      { *err=1; return 0; } break;
      case AST_T_I32: if (v < (long long)INT32_MIN || v > (long long)INT32_MAX) { *err=1; return 0; } break;
      case AST_T_U32: if (v < 0 || v > 0xFFFFFFFFLL){ *err=1; return 0; } break;
      default: break;
    }
    kv.iv = v;
  }
  if (!push_kv(p, kv)){ *err=1; return 0; }
  return 1;
}

/**
 * @brief Parse a JSON-like object into the parser context AST.
 *
 * Accepts an object wrapped by '{' and '}', allows empty objects.
 * On parse error `*err` is set and the function returns 0.
 *
 * @param p Parser context.
 * @param err[out] Non-zero on parse error.
 * @return 1 on success, 0 on failure.
 */
static int parse_object(pctx_t* p, int* err){
  if (!eat(p,'{')){ *err=1; return 0; }
  ws(p);
  if (eat(p,'}')) return 1; // empty
  for(;;){
    if (!parse_member(p,err)) return 0;
    ws(p);
    if (eat(p,'}')) break;
    if (!eat(p,',')){ *err=1; return 0; }
  }
  return 1;
}

/* --------- Encoder (AST→BJSON) --------- */
static void w32(uint8_t* p, uint32_t v){ p[0]=v&0xFF; p[1]=(v>>8)&0xFF; p[2]=(v>>16)&0xFF; p[3]=(v>>24)&0xFF; }
static int  align4sz(int n){ return (n+3)&~3; }

/**
 * @brief Encode the parser AST into the BJSON binary format.
 *
 * Writes header, entry count and each key/value encoded and 4-byte
 * padded. Returns 1 on success.
 *
 * @param p Parser context with built AST.
 * @param out Output buffer to write BJSON data.
 * @param cap Capacity of the output buffer.
 * @param out_len[out] Number of bytes written on success.
 * @return 1 on success, 0 if buffer is not large enough.
 */
static int encode_ast(const pctx_t* p, uint8_t* out, size_t cap, size_t* out_len){
  uint8_t* cur=out; uint8_t* end=out+cap;
  if (cur+12 > end) return 0;
  memcpy(cur,"BJSN",4); cur+=4;
  *cur++=1; *cur++=1; *cur++=0; *cur++=0;
  uint8_t* cntp=cur; cur+=4;

  uint32_t cnt=0;
  for (const ast_kv_t* kv=p->head; kv; kv=kv->next){
    uint8_t type=0; uint32_t vlen=0;
    switch (kv->type){
      case AST_T_STR: type=1; vlen=(uint32_t)strlen(kv->sval); break;
      case AST_T_I16: type=2; vlen=2; break;
      case AST_T_U16: type=3; vlen=2; break;
      case AST_T_I32: type=4; vlen=4; break;
      case AST_T_U32: type=5; vlen=4; break;
    }
    uint8_t nlen = (uint8_t)strlen(kv->key);
    int need = 1+1+2+4 + nlen + vlen;
    int need_al = align4sz(need);
    if (cur + need_al > end) return 0;

    *cur++ = type;
    *cur++ = nlen;
    *cur++ = 0; *cur++ = 0;
    w32(cur, vlen); cur+=4;
    memcpy(cur, kv->key, nlen); cur+=nlen;

    if (type==1){
      memcpy(cur, kv->sval, vlen); cur+=vlen;
    } else {
      if (kv->isz==2){ uint16_t v = (uint16_t)kv->iv; *cur++=v&0xFF; *cur++=(v>>8)&0xFF; }
      else { uint32_t v=(uint32_t)kv->iv; *cur++=v&0xFF; *cur++=(v>>8)&0xFF; *cur++=(v>>16)&0xFF; *cur++=(v>>24)&0xFF; }
    }
    while (((uintptr_t)cur & 3)!=0) *cur++=0;
    cnt++;
  }
  w32(cntp, cnt);
  *out_len = (size_t)(cur - out);
  return 1;
}

/* --------- Public API --------- */
bjson_err_t bjson_encode_from_json(const char* json, uint8_t* out, size_t out_cap, size_t* out_len){
  if (!json || !out || !out_len) return BJSON_EINVAL;
  pctx_t c = {0};
  c.json = json; c.len = strlen(json);
  c.arena_cap = ARENA_CAP;
  c.arena = (char*)malloc(ARENA_CAP);
  if (!c.arena) return BJSON_EBUF;

  int err=0;
  ws(&c);
  if (!parse_object(&c,&err)){ free(c.arena); return err?BJSON_ESYNTAX:BJSON_EINVAL; }
  ws(&c);
  if (c.pos != c.len){ free(c.arena); return BJSON_ESYNTAX; }

  size_t olen=0;
  int ok = encode_ast(&c, out, out_cap, &olen);
  free(c.arena);
  if (!ok) return BJSON_EBUF;
  *out_len = olen;
  return BJSON_OK;
}
