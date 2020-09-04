#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include "lbinchunk.h"
#include "lerror.h"
#include "memory.h"
#include "misc.h"
Prototype *global_proto;
char **global_source;
int global_source_len;

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
  uint8_t * buf = (uint8_t *)lmalloc(n * sizeof(char) + 1);
  if(buf == NULL) 
    panic(OOM);
  if(fread(buf,1,n,fp) == 0)
    panic(strerror(errno));
  buf[n] = 0;
  return buf;
}
char * read_string(FILE *fp) {
  uint8_t size = read_byte(fp);
  if(size == 0)
    return NULL;
  if(size == (uint8_t)0xff) {
    uint64_t size = read_uint64(fp);
    return (char *)read_bytes(fp,size - 1);
  }
  return (char *)read_bytes(fp,size - 1);
}
void check_header(FILE *fp) {
  //Check header
  char *a,*b;
  if(strncmp((a = (char *)read_bytes(fp,4)), LUA_SIGNATURE, 4) != 0) {
    panic("not a precompiled chunk");
  } else if(read_byte(fp) != LUAC_VERSION) {
    panic("version mismatch!");
  } else if(read_byte(fp) != LUAC_FORMAT) {
    panic("format mismatch!");
  } else if(strncmp((b = (char *)read_bytes(fp,6)), LUAC_DATA, 6)) {
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
  lfree(a);
  lfree(b);
}
Prototype * read_proto(FILE *fp,char *parent_src,int flag) {
  //Get function prototype
  char * source = read_string(fp);
  if(source == NULL || strlen(source) == 0) {
    source = parent_src;
    if(global_source[global_source_len - 1] != NULL)
	    expandGSource();

    for(int i = 0;i < global_source_len;i++) {
	    if(global_source[i] == parent_src)
		    break;
    	    if(global_source[i] == NULL) {
		    global_source[i] = source;
		    break;
	    }
    }
  }

  Prototype * ret = (Prototype *)lmalloc(sizeof(Prototype));
  if(ret == NULL)
    panic(OOM);
  if(flag == 1)
	  global_proto = ret;

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
  ret->upvalue_names = read_upvalue_names(fp,&ret->upvalue_names_len);
  return ret;
}
Prototype ** read_protos(FILE *fp,char * parent_src,uint32_t  *size) {
  //Get the subfunction prototype table
  *size = read_uint32(fp);
  if(*size == 0)
    return NULL;
  Prototype ** protos = (Prototype **)lmalloc(sizeof(Prototype *) * (*size));
  if(protos == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *size;i++)
    protos[i] = read_proto(fp,parent_src,0);
  return protos;
}

void expandGSource(void) {
	char ** g = malloc(sizeof(char *) * (global_source_len + 4));
	if(NULL == g)
		panic(OOM);
	global_source_len += 4;
	memset(g,0, global_source_len);
	memcpy(g,global_source,sizeof(char *) * (global_source_len - 4));
	free(global_source);
	global_source = g;
}

Prototype * Undump(FILE *fp) {
  check_header(fp);
  read_byte(fp);
  INITVALUE(global_source, global_source_len, DEFAULT_GLOBAL_SOURCE_LEN);
  return read_proto(fp,"",1);
}
uint32_t *read_code(FILE *fp,uint32_t  *size) {
  //Read instruction list
  *size = read_uint32(fp);
  uint32_t *code = (uint32_t *)lmalloc(sizeof(uint32_t)* *size);
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
  Type * constants = lmalloc(sizeof(Type)* (*len));
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
      uint8_t *n = (uint8_t *)lmalloc(sizeof(uint64_t));
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
      double *x = (double *)lmalloc(sizeof(double));
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
  Upvalue *upvalues = (Upvalue *)lmalloc(sizeof(Upvalue) * *len);
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
  uint32_t * lineinfo = (uint32_t *)lmalloc(sizeof(uint32_t *)* (*len));
  if(lineinfo == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *len;i++)
    lineinfo[i] = read_uint32(fp);
  return lineinfo;
}
LocVar * read_locvars(FILE *fp,uint32_t  *len) {
  *len = read_uint32(fp);
  LocVar * locvars = (LocVar *)lmalloc(sizeof(LocVar) * (*len));
  if(locvars == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *len;i++) {
    locvars[i].var_name = read_string(fp);
    locvars[i].start_pc = read_uint32(fp);
    locvars[i].end_pc = read_uint32(fp);
  }
  return locvars;
}
char ** read_upvalue_names(FILE *fp,uint32_t *size) {
  *size = read_uint32(fp);
  char ** names = (char **)lmalloc(sizeof(char *) * (*size));
  if(names == NULL)
    panic(OOM);
  for(uint32_t i = 0;i < *size;i++)
    names[i] = read_string(fp);
  return names;

}
void freeConstants(Type * constant) {
  switch(constant->type) {
      case TAG_NIL:
      case TAG_INTEGER:
        break;
      default:
        lfree(constant->data);
        break;
  }
}
void freeLocVars(LocVar * var) {
  lfree(var->var_name);
}
#define freeUpvalueName(p) lfree(p)
void freeP(Prototype *proto,char * source) {
  freeProtoType(proto,source);
  FREEVALUE(global_source, global_source_len, lfree);
}
void freeProtoType(Prototype * proto,char * source) {
  //释放Prototype结构体
  if(source != proto->source) {
    int flag = 1;
    for(int i = 0;i < global_source_len;i++)
	    if(global_source[i] == proto->source)
			    flag = 0;
    if(flag) {
	    
    	lfree(proto->source);
    }
  }
  lfree(proto->code);
  lfree(proto->line_info);
  lfree(proto->upvalues);
  for(uint64_t i = 0;i < proto->constants_len;i++)
    freeConstants(proto->constants + i);
  for(uint64_t i = 0;i < proto->loc_vars_len;i++)
    freeLocVars(proto->loc_vars + i);
  for(uint64_t i = 0;i < proto->protos_size;i++)
    freeProtoType(proto->protos[i],source);
  for(uint64_t i = 0;i < proto->upvalue_names_len;i++)
    freeUpvalueName(proto->upvalue_names[i]);
  if(proto->protos != NULL)
  	lfree(proto->protos);
  lfree(proto->upvalue_names);
  lfree(proto->loc_vars);
  lfree(proto->constants);
  lfree(proto);
}
