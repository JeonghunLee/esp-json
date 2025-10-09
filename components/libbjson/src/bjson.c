#include "bjson.h"
#include <string.h>
#include <stdint.h>

static uint32_t r32(const uint8_t* p){ return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24); }
static const uint8_t* align4p(const uint8_t* p){ uintptr_t x=(uintptr_t)p; return (const uint8_t*)((x+3)&~(uintptr_t)3); }

bjd_err_t bjd_open(const uint8_t* buf, size_t len, bjd_doc_t* d){
  if (!buf || len<12 || !d) return BJD_EINVAL;
  if (memcmp(buf,"BJSN",4)!=0) return BJD_EMAGIC;
  d->base=buf; d->len=len; d->count=r32(buf+8); d->entries=buf+12; return BJD_OK;
}

static int next_ent(const uint8_t* cur, const uint8_t* end, const uint8_t** nxt){
  if ((size_t)(end-cur) < 8) return 0;
  uint8_t nlen = cur[1];
  uint32_t vlen = r32(cur+4);
  const uint8_t* after = cur + 8 + nlen + vlen;
  if (after > end) return 0;
  *nxt = align4p(after);
  return 1;
}

int bjd_find(const bjd_doc_t* d, const char* key, bjd_entry_t* out){
  const uint8_t* cur = d->entries; const uint8_t* end = d->base + d->len;
  size_t klen = strlen(key);
  for (uint32_t i=0;i<d->count;i++){
    uint8_t type = cur[0], nlen = cur[1];
    uint32_t vlen = r32(cur+4);
    const char* name=(const char*)(cur+8);
    const uint8_t* val = (const uint8_t*)(cur+8+nlen);
    if (klen==nlen && memcmp(name,key,nlen)==0){
      out->type=(bjd_type_t)type; out->name=name; out->name_len=nlen; out->val=val; out->val_len=vlen; return (int)i;
    }
    const uint8_t* nxt; if (!next_ent(cur,end,&nxt)) break; cur = nxt;
  }
  return -1;
}

int bjd_get_i32(const bjd_doc_t* d, const char* key, int32_t* out){
  bjd_entry_t e; if (bjd_find(d,key,&e)<0) return -1;
  if (e.type!=BJD_T_I16 && e.type!=BJD_T_I32) return -1;
  if (e.val_len==2){ int16_t v=(int16_t)(e.val[0]|(e.val[1]<<8)); *out=v; return 0; }
  if (e.val_len==4){ int32_t v=(int32_t)r32(e.val); *out=v; return 0; }
  return -1;
}
int bjd_get_u32(const bjd_doc_t* d, const char* key, uint32_t* out){
  bjd_entry_t e; if (bjd_find(d,key,&e)<0) return -1;
  if (e.type!=BJD_T_U16 && e.type!=BJD_T_U32) return -1;
  if (e.val_len==2){ uint16_t v=(uint16_t)(e.val[0]|(e.val[1]<<8)); *out=v; return 0; }
  if (e.val_len==4){ *out=r32(e.val); return 0; }
  return -1;
}
int bjd_get_str(const bjd_doc_t* d, const char* key, const char** s, uint32_t* n){
  bjd_entry_t e; if (bjd_find(d,key,&e)<0) return -1;
  if (e.type!=BJD_T_STR) return -1;
  *s=(const char*)e.val; *n=e.val_len; return 0;
}
