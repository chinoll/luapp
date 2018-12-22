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
                printf("[%s]",to_string(state,i));
                break;
            default:
                printf("[%s]",type_name(t));
        }
    }
    printf("\n");
}
int main(int argc,char *argv[]) {
    LuaState * state = newLuaState();
    push_bool(state,true); printStack(state);
    push_int(state,100); printStack(state);
    push_nil(state); printStack(state);
    push_string(state,"Hello,Lua!"); printStack(state);
    push_value(state,-4); printStack(state);
    replace(state,3); printStack(state);
    set_top(state,6); printStack(state);
    //remove_value(state,-3); printStack(state);
    rotate(state,-3,-1); printStack(state);
    popN(state,1); printStack(state);
    set_top(state,-5); printStack(state);
    freeLuaState(state);
}
