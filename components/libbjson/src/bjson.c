#include "bjson.h"
#include <string.h>
#include <stdint.h>

/**
 * @brief Read 32-bit little-endian unsigned integer.
 *
 * @param p Pointer to 4 bytes.
 * @return 32-bit value.
 */
static uint32_t r32(const uint8_t* p){ return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24); }

/**
 * @brief Align pointer upwards to 4-byte boundary.
 *
 * @param p Input pointer.
 * @return Pointer aligned to 4 bytes at or after `p`.
 */
static const uint8_t* align4p(const uint8_t* p){ uintptr_t x=(uintptr_t)p; return (const uint8_t*)((x+3)&~(uintptr_t)3); }

/**
 * @brief Open a BJSON document for reading.
 *
 * Validates the magic header and populates `bjd_doc_t`. The provided
 * buffer must remain valid for the lifetime of the document.
 *
 * @param buf Pointer to BJSON buffer.
 * @param len Length of buffer in bytes.
 * @param d[out] Document structure to populate.
 * @return BJD_OK on success, or an error code on invalid input/magic.
 */
bjd_err_t bjd_open(const uint8_t* buf, size_t len, bjd_doc_t* d){
  if (!buf || len<12 || !d) return BJD_EINVAL;
  if (memcmp(buf,"BJSN",4)!=0) return BJD_EMAGIC;
  d->base=buf; d->len=len; d->count=r32(buf+8); d->entries=buf+12; return BJD_OK;
}

/**
 * @brief Compute pointer to next entry given current entry pointer.
 *
 * Validates header size, name length and value length against `end`.
 *
 * @param cur Current entry pointer.
 * @param end Pointer one past the end of buffer.
 * @param nxt[out] Pointer to next entry on success.
 * @return 1 on success, 0 on error/truncation.
 */
static int next_ent(const uint8_t* cur, const uint8_t* end, const uint8_t** nxt){
  if ((size_t)(end-cur) < 8) return 0;
  uint8_t nlen = cur[1];
  uint32_t vlen = r32(cur+4);
  const uint8_t* after = cur + 8 + nlen + vlen;
  if (after > end) return 0;
  *nxt = align4p(after);
  return 1;
}

/**
 * @brief Find an entry by key name in the document.
 *
 * On success fills `out` with entry metadata and returns the index.
 * Returns -1 if not found.
 *
 * @param d Document to search.
 * @param key NUL-terminated key name to find.
 * @param out[out] Entry metadata on success.
 * @return Index of entry on success, -1 if not found or on error.
 */
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

/**
 * @brief Retrieve a signed 32-bit integer value by key.
 *
 * Accepts stored values of 16-bit or 32-bit integer types and promotes
 * them appropriately.
 *
 * @param d Document handle.
 * @param key Key name to lookup.
 * @param out[out] Destination to store value on success.
 * @return 0 on success, -1 on not found or type/size mismatch.
 */
int bjd_get_i32(const bjd_doc_t* d, const char* key, int32_t* out){
  bjd_entry_t e; if (bjd_find(d,key,&e)<0) return -1;
  if (e.type!=BJD_T_I16 && e.type!=BJD_T_I32) return -1;
  if (e.val_len==2){ int16_t v=(int16_t)(e.val[0]|(e.val[1]<<8)); *out=v; return 0; }
  if (e.val_len==4){ int32_t v=(int32_t)r32(e.val); *out=v; return 0; }
  return -1;
}
/**
 * @brief Retrieve an unsigned 32-bit integer value by key.
 *
 * Supports stored 16-bit or 32-bit unsigned types.
 *
 * @param d Document handle.
 * @param key Key name to lookup.
 * @param out[out] Destination to store value on success.
 * @return 0 on success, -1 on not found or type/size mismatch.
 */
int bjd_get_u32(const bjd_doc_t* d, const char* key, uint32_t* out){
  bjd_entry_t e; if (bjd_find(d,key,&e)<0) return -1;
  if (e.type!=BJD_T_U16 && e.type!=BJD_T_U32) return -1;
  if (e.val_len==2){ uint16_t v=(uint16_t)(e.val[0]|(e.val[1]<<8)); *out=v; return 0; }
  if (e.val_len==4){ *out=r32(e.val); return 0; }
  return -1;
}
/**
 * @brief Retrieve a string value by key.
 *
 * Returns a pointer into the original BJSON buffer; the caller must
 * ensure the buffer remains valid while using the pointer.
 *
 * @param d Document handle.
 * @param key Key name to lookup.
 * @param s[out] Pointer to string data on success (not NUL-terminated).
 * @param n[out] Length of string in bytes.
 * @return 0 on success, -1 if not found or type mismatch.
 */
int bjd_get_str(const bjd_doc_t* d, const char* key, const char** s, uint32_t* n){
  bjd_entry_t e; if (bjd_find(d,key,&e)<0) return -1;
  if (e.type!=BJD_T_STR) return -1;
  *s=(const char*)e.val; *n=e.val_len; return 0;
}
