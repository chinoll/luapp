#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include "lbinchunk.h"
#include "lerror.h"

uint32_t read_uint32(FILE *fp) {
  unsigned ret;
  fread(&ret,1,4,fp);
  return ret;
}
uint64_t read_uint64(FILE *fp) {
  uint64_t ret;
  fread(&ret,8,1,fp);
  return ret;
}
int64_t read_int64(FILE *fp) {
  return (int64_t)read_uint64(fp);
}
double read_lua_number(FILE *fp) {
  double x;
  fread(&x,8,1,fp);
  return x;
}
uint8_t * read_bytes(FILE *fp,uint64_t n) {
  uint8_t * buf = (int8_t *)malloc(n*sizeof(int8_t));
  if(buf == NULL) 
    panic(OOM);
  if(fread(buf,1,n,fp) == 0)
    panic(strerror(errno));
  return buf;
}
char * read_string(FILE *fp) {
  //
  uint8_t size = read_byte(fp);
  if(size == 0)
    return NULL;
  if(size == (uint8_t)0xff) {
    uint64_t size = read_uint64(fp);
    return read_bytes(fp,size - 1);
  }
  return read_bytes(fp,size - 1);
}
void check_header(FILE *fp) {
  //Check header
  if(strcmp(read_bytes(fp,4),LUA_SIGNATURE) != 0) {
    panic("not a precompiled chunk");
  } else if(read_byte(fp) != LUAC_VERSION) {
    panic("version mismatch!");
  } else if(read_byte(fp) != LUAC_FORMAT) {
    panic("format mismatch!");
  } else if(strcmp(read_bytes(fp,6),LUAC_DATA)) {
    panic("corrupted!");
  } else if(read_byte(fp) != CINT_SIZE) {
    panic("int size mismatch!");
  } else if(read_byte(fp) != CSZIET_SIZE) {
    panic("size_t size mismatch!");
  } else if(read_byte(fp) != INSTRUCTION_SIZE) {
    panic("instruction size mismatch!");
  } else if(read_byte(fp) != LUA_INTEGER_SIZE) {
    panic("lua_integer size mismatch!");
  } else if(read_byte(fp) != LUA_NUMBER_SIZE) {
    panic("lua_number size mismatch!");
  } else if(read_lua_integer(fp) != LUAC_INT) { 
    panic("endianness mismatch!");
  } else if(read_lua_number(fp) != LUAC_NUM){
    panic("float format mismatch!");
  }
}
Prototype * read_proto(FILE *fp,char *parent_src) {
  //Get function prototype
  char * source = read_string(fp);
  if(source == NULL || strlen(source) == 0) {
    source = parent_src;
  }
  Prototype * ret = (Prototype *)malloc(sizeof(Prototype));
  if(ret == NULL)
    panic(OOM);
  ret->source = source;
  ret->line_def = read_uint32(fp);
  ret->last_line_def = read_uint32(fp);
  ret->num_params = read_byte(fp);
  ret->is_vararg = read_byte(fp);
  ret->max_stack_size = read_byte(fp);
  ret->code = read_code(fp,&ret->code_len);
  ret->constants = read_constants(fp,&ret->constants_len);
  ret->upvalues = read_upvalues(fp,&ret->upvalues_len);
  ret->protos = read_protos(fp,source,&ret->protos_size);
  ret->line_info = read_line_info(fp,&ret->line_info_len);
  ret->loc_vars = read_locvars(fp,&ret->loc_vars_len);
  ret->upvalue_names = read_upvalue_names(fp);
  return ret;
}
Prototype ** read_protos(FILE *fp,char * parent_src,uint32_t  *size) {
  //Get the subfunction prototype table
  *size = read_uint32(fp);
  if(*size == 0)
    return NULL;
  Prototype ** protos = (Prototype **)malloc(sizeof(Prototype *) * (*size));
  if(protos == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *size;i++)
    protos[i] = read_proto(fp,parent_src);
  return protos;
}

Prototype * Undump(FILE *fp) {
  check_header(fp);
  read_byte(fp);
  return read_proto(fp,"");
}
uint32_t *read_code(FILE *fp,uint32_t  *size) {
  //Read instruction list
  *size = read_uint32(fp);
  uint32_t *code = (uint32_t *)malloc(sizeof(uint32_t)* *size);
  if(code == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *size;i++) {
    code[i] = read_uint32(fp);
  }
  return code;
}
Type * read_constants(FILE *fp,uint32_t * len) {
  //Read constant table
  *len = read_uint32(fp);
  if(*len == 0)
    return NULL;
  Type * constants = malloc(sizeof(Type)* (*len));
  if(constants == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *len;i++) {
    constants[i] = read_constant(fp);

  } 
  return constants;
}
Type read_constant(FILE *fp) {
  //Get a constant
  Type type = {0,NULL};
  uint8_t t = read_byte(fp);
  switch(t) {
    case TAG_NIL:
      break;
    case TAG_BOOLEAN:{
      uint8_t *n = (uint8_t *)malloc(sizeof(uint64_t));
      if(n == NULL)
        panic(OOM);
      if(read_byte(fp) != 0) {
        *n = 1;
        type.data = n;
      }
      break;
    }
    case TAG_INTEGER: {
      type.data = (void *)read_lua_integer(fp);
      break;
    }
    case TAG_NUMBER:{
      double *x = (double *)malloc(sizeof(double));
      if(x == NULL)
        panic(OOM);
      *x = read_lua_number(fp);
      type.data = (void *)x;
      break;
    }
    case TAG_SHORT_STR:
    case TAG_LONG_STR:
      type.data = (void *)read_string(fp);
      break;
    default:
      panic("corrupted!");
  }
  type.type = t;
  return type;
}
Upvalue * read_upvalues(FILE *fp,uint32_t  *len) {
  //Read the Upvalue table
  *len = read_uint32(fp);
  Upvalue *upvalues = (Upvalue *)malloc(sizeof(Upvalue) * *len);
  if(upvalues == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *len;i++) {
    upvalues[i].in_stack = read_byte(fp);
    upvalues[i].idx = read_byte(fp);
  }
  return upvalues;
}

uint32_t * read_line_info(FILE *fp,uint32_t * len) {
  *len = read_uint32(fp);
  uint32_t * lineinfo = (uint32_t *)malloc(sizeof(uint32_t *)* (*len));
  if(lineinfo == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *len;i++)
    lineinfo[i] = read_uint32(fp);
  return lineinfo;
}
LocVar * read_locvars(FILE *fp,uint32_t  *len) {
  *len = read_uint32(fp);
  LocVar * locvars = (LocVar *)malloc(sizeof(LocVar) * (*len));
  if(locvars == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *len;i++) {
    locvars[i].var_name = read_string(fp);
    locvars[i].start_pc = read_uint32(fp);
    locvars[i].end_pc = read_uint32(fp);
  }
  return locvars;
}
char ** read_upvalue_names(FILE *fp) {
  uint32_t size = read_uint32(fp);
  char ** names = (char **)malloc(sizeof(char *) * size);
  if(names == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < size;i++)
    names[i] = read_string(fp);
  return names;

}
