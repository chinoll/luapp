#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <xxhash.h>
#include "lbinchunk.h"
#include "lerror.h"
#include "opcode.h"
#include "lstate.h"
#include "lvm.h"
#include "inst.h"
#include "list.h"
#include "hashmap.h"
#include "gc.h"
#include "lvalue.h"
#include "misc.h"
#include "consts.h"
int debug_level;

void printStack(LuaState * state) {
    uint64_t top = get_top(state);
    for(uint64_t i = 1;i <= top;i++) {
        int8_t t = type(state,i);
        switch(t) {
            case LUAPP_TBOOLEAN: {
                if(to_bool(state,i))
                    printf("[true]");
                else
                    printf("[false]");
                break;
            }
            case LUAPP_TINT:
                printf("[%ld]",to_int(state,i));
                break;
            case LUAPP_TFLOAT:
                printf("[%lf]",to_number(state,i));
                break;
            case LUAPP_TSTRING:
                printf("[\"%s\"]",to_string(state,i));
                break;
            default:
                printf("[%s]",type_name(t));
        }
    }
    printf("\n");
}

uint64_t ObjHash(void *obj,uint64_t len,uint64_t seed) {
    return XXH64(obj, sizeof(LuaValue),seed);
}

int print(LuaState *ls) {
    int nargs = get_top(ls);
    for(int j = 1;j <= nargs;j++) {
        if(isBool(ls, j))
            printf("%d",to_bool(ls,j));
        else if(isString(ls,j))
            printf(to_string(ls,j));
        else if(isNumber(ls,j))
            printf("%f",to_number(ls,j));
        else
            printf(type_name(type(ls,j)));
    }
    printf("\n");
    return 0;
}

int getMetatable(LuaState *state) {
    if(GetMetatable(state, 1)) {
        push_nil(state);
    }
    return 1;
}
int setMetatable(LuaState *state) {
    SetMetatable(state, 1);
    return 1;
}
int main(int argc,char *argv[]) {
    if(argc < 2)
        return 1;
    FILE *fp = fopen(argv[1],"rb");
    if(fp == NULL)
        return 2;
    debug_level = 0;
    list_init(&rootSet);
    INITVALUE(global_stack, global_stack_size, DEFAULT_GLOBAL_STACK_SIZE);
    INITVALUE(global_upvals, global_upvals_size, DEFAULT_GLOBAL_STACK_SIZE);
    period = getMillisecond() + 1000;
    Load(fp,"b");
    
    register_function(vm->state,"print",print);
    register_function(vm->state, "getmetatable", getMetatable);
    register_function(vm->state, "setmetatable", setMetatable);

    Call(vm->state,0,0);
    fclose(fp);
    GCall();
    freeP(global_proto,"");
    freeLuaVM(vm);
    FREEVALUE(global_stack,global_stack_size,freeLuaStack);
    FREEVALUE(global_upvals,global_upvals_size,freeLuaUpvalue);
    return 0;
}
