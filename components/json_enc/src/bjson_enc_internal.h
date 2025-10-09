#pragma once
#include <stdint.h>

typedef enum { 
  AST_T_STR=1, 
  AST_T_I16, 
  AST_T_U16, 
  AST_T_I32, 
  AST_T_U32 
} ast_type_t;

typedef struct ast_kv_s {
  ast_type_t type;
  const char* key;     // 원문 키(소유권: arena)
  const char* sval;    // 문자열 값(소유권: arena) / NULL if int
  int        smax;     // STR_N 상한
  long long  iv;       // 정수 값
  int        isz;      // 2 or 4 (bytes)
  struct ast_kv_s* next;
} ast_kv_t;

typedef struct {
  const char* json;    // 입력 버퍼
  size_t      len;
  size_t      pos;

  char*  arena;
  size_t arena_cap, arena_used;

  ast_kv_t* head;
  ast_kv_t* tail;
} pctx_t;
