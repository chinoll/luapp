#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "binchunk.h"
#include "lua_error.h"
void printHeader(Prototype *f) {
  char *functype="main";
  char varargFlag;
  if(f->line_def > 0)
    functype = "function";
  if(f->is_vararg > 0)
    varargFlag = '+';
  printf("\n%s <%s:%d,%d> (%d instructions)\n",functype,f->source,f->line_def,f->last_line_def,f->code_len);
  printf("%d%c params, %d slots, %d upvalues, ",f->num_params,varargFlag,f->max_stack_size,f->upvalues_len);
  printf("%d locals, %d constants, %d functions\n",f->loc_vars_len,f->constants_len,f->protos_size);

}
void printCode(Prototype *f) {
  for(uint32_t  i = 0;i < f->code_len;i++) {
    char t = '-';
    char *line = &t;
    if (f->line_info_len > 0) {
      line = malloc(20);
      if(line == NULL) 
        panic(OOM);
      sprintf(line,"%d",f->line_info[i]);
    }
    printf("\t%d\t[%s]\t0x%08x\n",i + 1,line,f->code[i]);
    if(&t != line)
      free(line);
  }
}
void constantToString(Type * f,char * str) {
  switch(f->type) {
    case TAG_NIL:
      strcpy(str,"nil");
      break;
    case TAG_BOOLEAN: {
      if(*((uint8_t *)f->data) == 1)
        strcpy(str,"true");
      else 
        strcpy(str,"false");
      break;
    }
    case TAG_INTEGER:
        sprintf(str,"%d",(uint64_t)f->data);
        break;
    case TAG_LONG_STR:
    case TAG_SHORT_STR:
      strcpy(str,f->data);
      break;
    case TAG_NUMBER:
      sprintf(str,"%f",*((double *)f->data));
      break;
  }

}
char *upval_name(Prototype *f,int idx) {
  if(f->upvalues_len > 0)
    return f->upvalue_names[idx];
  return "-";
}
void printDetail(Prototype *f) {
  printf("constants (%d):\n",f->constants_len);
  for(uint32_t i = 0;i < f->constants_len;i++) {
    uint8_t str[20];
    constantToString(&f->constants[i],str);
    printf("\t%d\t%s\n",i+1,str);
  }
  printf("locals (%d):\n",f->loc_vars_len);
  for(uint32_t i = 0;i < f->loc_vars_len;i++)
    printf("\t%d\t%s\t%d\t%d\n",i,f->loc_vars[i].var_name,f->loc_vars[i].start_pc + 1,f->loc_vars[i].end_pc);
  printf("upvalues (%d):\n",f->upvalues_len);
  for(uint32_t i = 0;i < f->upvalues_len;i++)
    printf("\t%d\t%s\t%d\t%d\n",i,upval_name(f,i),f->upvalues[i].in_stack,f->upvalues[i].idx);
}
void list(Prototype * f) {
  printHeader(f);
  printCode(f);
  printDetail(f);
  for(uint32_t i = 0;i < f->protos_size;i++)
    list(f->protos[i]);
}
int main(int argc,char *argv[]) {
  if(argc != 2)
    exit(1);
  FILE *fp = fopen(argv[1],"rb");
  if(fp == NULL) {
    char * t = strerror(errno);
    panic(t);
  }
  Prototype * proto = Undump(fp);
  list(proto);
}
