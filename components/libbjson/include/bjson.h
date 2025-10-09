#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { 
  BJD_OK=0, 
  BJD_EINVAL, 
  BJD_EMAGIC 
} bjd_err_t;

typedef enum { 
  BJD_T_STR=1, 
  BJD_T_I16, 
  BJD_T_U16, 
  BJD_T_I32, 
  BJD_T_U32 
} bjd_type_t;

typedef struct {
  bjd_type_t type;
  const char* name;  uint8_t name_len;
  const uint8_t* val; uint32_t val_len;
} bjd_entry_t;

typedef struct {
  const uint8_t* base; size_t len; uint32_t count; const uint8_t* entries;
} bjd_doc_t;

bjd_err_t bjd_open(const uint8_t* buf, size_t len, bjd_doc_t* doc);
int       bjd_find(const bjd_doc_t* doc, const char* key, bjd_entry_t* out); // -1 not found
int       bjd_get_i32(const bjd_doc_t* doc, const char* key, int32_t* out);
int       bjd_get_u32(const bjd_doc_t* doc, const char* key, uint32_t* out);
int       bjd_get_str(const bjd_doc_t* doc, const char* key, const char** s, uint32_t* n);

#ifdef __cplusplus
}
#endif
