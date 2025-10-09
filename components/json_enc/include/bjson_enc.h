#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  BJSON_OK=0, 
  BJSON_EINVAL, 
  BJSON_EBUF, 
  BJSON_ESYNTAX, 
  BJSON_ETYPE, 
  BJSON_ERANGE
} bjson_err_t;

/** JSON(관용 허용) 텍스트 → BJSON 바이너리 */
bjson_err_t bjson_encode_from_json(const char* json, uint8_t* out, size_t out_cap, size_t* out_len);

#ifdef __cplusplus
}
#endif
