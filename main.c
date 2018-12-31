#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include "lbinchunk.h"
#include "lerror.h"
#include "opcode.h"
#include "lstate.h"

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
int main(int argc,char *argv[]) {
    LuaState * state = newLuaState();
    push_int(state,1);
    push_string(state,"2.0");
    push_string(state,"3.0");
    push_num(state,4.0);
    printStack(state);
    Arith(state,LUAPP_OPADD); printStack(state);
    Arith(state,LUAPP_OPBNOT); printStack(state);
    Arith(state,LUAPP_OPMUL);   printStack(state);
    LuaValue *val = pop(state->stack);
    freeLuaValue(val);
    printStack(state);
    push_string(state,"Hello,Lua!");    printStack(state);
    push_int(state,1002);   printStack(state);
    Len(state,2);   printStack(state);
    Concat(state,3);  printStack(state);
    push_bool(state,Compare(state,1,2,LUAPP_OPEQ));
    printStack(state);
    freeLuaState(state);
}
