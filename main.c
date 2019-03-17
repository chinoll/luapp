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
#include "lvm.h"
#include "inst.h"


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
void LuaMain(Prototype *proto) {
    uint32_t nregs = proto->max_stack_size;
    LuaVM *vm = NewLuaVM(nregs + 8,proto);
    set_top(&vm->state,nregs);
    while(true) {
        uint64_t pc = getPC(&vm->state);
        instruction inst = fetch(&vm->state);
        if (get_opcode(inst) != OP_RETURN) {
            ExecuteInstruction(vm, inst);
            printf("[%d] %s ", pc + 1, codes[get_opcode(inst)].name);
            printStack(&vm->state);
        } else {
            break;
        }
    }
    freeLuaVM(vm);
}

int main(int argc,char *argv[]) {
    if(argc < 2)
        return 1;
    FILE *fp = fopen(argv[1],"rb");
    if(fp == NULL)
        return 2;
    Prototype *proto = Undump(fp);
    LuaMain(proto);
    fclose(fp);
    freeProtoType(proto,"");
    return 0;
}
