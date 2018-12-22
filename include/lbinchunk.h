#ifndef __BINCHUNK_H
#define __BINCHUNK_H

#include <stdio.h>
#include <stdint.h>

#define LUA_SIGNATURE "\x1bLua"
#define LUAC_VERSION 0x53
#define LUAC_FORMAT 0x0
#define LUAC_DATA "\x19\x93\r\n\x1a\n"
#define CINT_SIZE 4
#define CSZIET_SIZE 8
#define INSTRUCTION_SIZE 4
#define LUA_INTEGER_SIZE 8
#define LUA_NUMBER_SIZE 8
#define LUAC_INT ((int)0x5678)
#define LUAC_NUM ((double)370.5)

#define TAG_NIL 0x00
#define TAG_BOOLEAN 0x01
#define TAG_NUMBER 0x03
#define TAG_INTEGER 0x13
#define TAG_SHORT_STR 0x04
#define TAG_LONG_STR 0x14
typedef struct __type {
  int type;
  void * data;
} Type;

typedef struct __head {
    uint8_t signature[4];
    uint8_t version;
    uint8_t format;
    uint8_t luacData[6];
    uint8_t cintSize;
    uint8_t sizetSize;
    uint8_t instructionSize;
    uint8_t luaIntegerSize;
    uint8_t luaNumberSize;
    int64_t luacInt;
    double luacNum;
} Head;
typedef struct __upvalue {
    uint8_t in_stack;
    uint8_t idx;
} Upvalue;
typedef struct __locvar {
    char *var_name;
    uint32_t start_pc;
    uint32_t end_pc;
} LocVar;
typedef struct __prototype {
    uint32_t line_def;
    uint32_t last_line_def;
    char *source;
    uint8_t num_params;
    uint8_t is_vararg;
    uint8_t max_stack_size;
    uint32_t *code;
    uint32_t code_len;
    Type *constants;
    uint32_t constants_len;
    Upvalue *upvalues;
    uint32_t upvalues_len;
    struct __prototype **protos;
    uint32_t protos_size;
    uint32_t *line_info;
    uint32_t line_info_len;
    LocVar *loc_vars;
    uint32_t loc_vars_len;
    char ** upvalue_names;
    uint32_t upvalue_names_len;

} Prototype;
typedef struct __binChunk {
    Head head;
    uint8_t size_upvalues;
    Prototype *main_func;
} BinChunk;
void freeBinChunk(BinChunk * bin);
#define read_byte(fp) (char)fgetc(fp)
uint32_t read_uint32(FILE *fp);
uint64_t read_uint64(FILE * fp);
int64_t read_int64(FILE *fp);
double read_lua_numer(FILE *fp);
char * read_string(FILE *fp);
uint8_t * read_bytes(FILE *fp,uint64_t n);
void check_header(FILE *fp);
#define read_lua_integer(x) read_uint64(x)
Prototype * Undump(FILE *fp);
Prototype * read_proto(FILE *fp,char *parent_src);
uint32_t *read_code(FILE *fp,uint32_t *size);
Type *read_constants(FILE *fp,uint32_t *len);
Type read_constant(FILE *fp);
Upvalue * read_upvalues(FILE * fp,uint32_t *len);
Prototype ** read_protos(FILE * fp,char *parent_src,uint32_t *size);
uint32_t * read_line_info(FILE * fp,uint32_t *len);
LocVar * read_locvars(FILE * fp,uint32_t *len);
char ** read_upvalue_names(FILE *fp,uint32_t *size);
#endif
