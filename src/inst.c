#include <stdbool.h>
#include <stdlib.h>
#include "inst.h"
#include "lstate.h"
#include "opcode.h"
#include "convert.h"
#include "memory.h"

#define RegisterCount(state) (state)->stack->lua_closure->proto->max_stack_size
//Lua指令集
void moveInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;
    copy_value(vm->state,ins.b,ins.a);
}

void jmpInst(instruction i) {
    struct code_format ins = AsBx(i);
    addPC(vm->state,ins.bx);
    if(ins.a != 0)
        panic("todo!");
}

void loadNilInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    push_nil(vm->state);
    for(uint32_t i = ins.a;i < ins.a + ins.b;i++)
        copy_value(vm->state,-1,i);
    free(popN((vm->state)->stack,1));
}

void loadBoolInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    push_bool(vm->state,ins.b != 0);
    replace(vm->state,ins.a);
    if(ins.c != 0) {
        addPC(vm->state,1);
    }
}

void loadKInst(instruction i) {
    struct code_format ins = ABx(i);
    //printf("address:%p\n",ins);
    ins.a++;
    getConst(vm->state,ins.bx);
    replace(vm->state,ins.a);
}

void loadKxInst(instruction i) {
    struct code_format ins = ABx(i);
    ins.a += 1;
    struct code_format ax = Ax(fetch(vm->state));
    getConst(vm->state,ax.ax);
    replace(vm->state,ins.a);
}

inline static void __binaryArith(instruction i,ArithOp op) {
    struct code_format ins = ABC(i);
    ins.a++;
    getRK(vm->state,ins.b);
    getRK(vm->state,ins.c);
    Arith(vm->state,op);
    replace(vm->state,ins.a);
}



void addInst(instruction i) { __binaryArith(i,LUAPP_OPADD); }
void subInst(instruction i) { __binaryArith(i,LUAPP_OPSUB); }
void mulInst(instruction i) { __binaryArith(i,LUAPP_OPMUL); }
void modInst(instruction i) { __binaryArith(i,LUAPP_OPMOD); }
void powInst(instruction i) { __binaryArith(i,LUAPP_OPPOW); }
void divInst(instruction i) { __binaryArith(i,LUAPP_OPDIV); }
void idivInst(instruction i) { __binaryArith(i,LUAPP_OPIDIV); }
void bandInst(instruction i) { __binaryArith(i,LUAPP_OPBAND); }
void borInst(instruction i) { __binaryArith(i,LUAPP_OPBOR); }
void bxorInst(instruction i) { __binaryArith(i,LUAPP_OPBXOR); }
void shlInst(instruction i) { __binaryArith(i,LUAPP_OPSHL); }
void shrInst(instruction i) { __binaryArith(i,LUAPP_OPSHR); }

inline static void __unaryArith(instruction i,ArithOp op) {
    struct code_format ins = ABC(i);
    ins.a++;
    getRK(vm->state,ins.b);
    Arith(vm->state,op);
    replace(vm->state,ins.a);
}

void unmInst(instruction i) { __unaryArith(i,LUAPP_OPUNM); }
void bnotInst(instruction i) { __unaryArith(i,LUAPP_OPBNOT); }

void lenInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;
     Len(vm->state,ins.b);
     replace(vm->state,ins.a);
}

void concatInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;
    ins.c++;
    uint32_t n = ins.c - ins.b + 1;
    checkStack(vm->state->stack,n);
    for(uint32_t j = ins.b;j <= ins.c;j++)
        push_value(vm->state,j);

    Concat(vm->state,n,ins.b);
        replace(vm->state,ins.a);
}

inline static void __compare(instruction i,CompareOp op) {
    struct code_format ins = ABC(i);
    getRK(vm->state,ins.b);
    getRK(vm->state,ins.c);
    if(Compare(vm->state,-2,-1,op)  != (ins.a != 0)) {
        addPC(vm->state,1);
    }
    lfree(popN((vm->state)->stack,2));
}

void eqInst(instruction i) { __compare(i,LUAPP_OPEQ); } // ==
void ltInst(instruction i) { __compare(i,LUAPP_OPLT); } //<
void leInst(instruction i) { __compare(i,LUAPP_OPLE); } // <=

void notInst(instruction i) {
    struct code_format ins = ABC(i);

    ins.a++;
    ins.b++;

    push_bool(vm->state,to_bool(vm->state,ins.b));
    replace(vm->state,ins.a);
}

void testSetInst(instruction i) {
    struct code_format ins = ABC(i);

    ins.a++;
    ins.b++;

    if(to_bool(vm->state,ins.b) == (ins.c != 0)) {
        copy_value(vm->state,ins.b,ins.a);
    } else {
        addPC(vm->state,1);
    }
}

void testInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;

    if(to_bool(vm->state,ins.a) != (ins.c != 0))
        addPC(vm->state,1);
}

void forPrepInst(instruction i) {
    struct code_format ins = AsBx(i);
    ins.a++;

    push_value(vm->state,ins.a);
    push_value(vm->state,ins.a + 2);
    Arith(vm->state,LUAPP_OPSUB);
    replace(vm->state,ins.a);
    addPC(vm->state,ins.bx);
}

void forLoopInst(instruction i) {
    struct code_format ins = AsBx(i);
    ins.a++;
    push_value(vm->state,ins.a + 2);
    push_value(vm->state,ins.a);
    Arith(vm->state,LUAPP_OPADD);
    replace(vm->state,ins.a);

    bool isStep = to_int(vm->state,ins.a + 2) >= 0;
    if((isStep && Compare(vm->state,ins.a,ins.a + 1,LUAPP_OPLE)) || \
        (!isStep && Compare(vm->state,ins.a + 1,ins.a,LUAPP_OPLE))) {
        addPC(vm->state,ins.bx);
        copy_value(vm->state,ins.a,ins.a + 3);
    }
}

void newTableInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    CreateTable(vm->state,fb2int(ins.b),fb2int(ins.c));
    replace(vm->state,ins.a);
}

void setTableInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;

    getRK(vm->state,ins.b);
    getRK(vm->state,ins.c);
    SetTable(vm->state, ins.a);

}

void setListInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;

    if(ins.c > 0)
        ins.c--;
    else
        ins.c = Ax(fetch(vm->state)).ax;

    int64_t idx = (int64_t)((int64_t )ins.c * LFIELDS_PER_FLUSH);

    for(uint64_t j = 1;j <= ins.b;j++) {
        idx++;
        push_value(vm->state,ins.a + j);
        SetI(vm->state,ins.a,idx);
    }
    bool flag = ins.b == 0;
    if(flag) {
        ins.b = to_int(vm->state,-1) - ins.a - 1;
        pop(vm->state->stack);
    }

    if(flag) {
        for(int32_t j = RegisterCount(vm->state) + 1;j <= get_top(vm->state);j++) {
            idx++;
            push_value(vm->state,j);
            SetI(vm->state,ins.a,idx);

        }
        set_top(vm->state,RegisterCount(vm->state));
    }
}

void getTableInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;

    getRK(vm->state,ins.c);
    GetTable(vm->state,ins.b);
    replace(vm->state,ins.a);
}

static inline void __fixStack(int32_t a) {
    int64_t x = to_int(vm->state,-1);
    pop(vm->state->stack);
    checkStack(vm->state->stack,x - a);
    for(int64_t i = a;i < x;i++)
        push_value(vm->state,i);
    rotate(vm->state,RegisterCount(vm->state) + 1, x - a);
}
static inline int32_t __pushFuncAndArgs(int32_t a,int32_t b) {
    if(b >= 1) {
        checkStack(vm->state->stack,b);
        for(int32_t i = a; i < a + b;i++)
            push_value(vm->state,i);
        return b - 1;
    } else {
        __fixStack(a);
        return get_top(vm->state) - RegisterCount(vm->state) - 1;
    }
}
static inline void __popResults(int32_t a,int32_t c) {
    if(c > 1) {
        for(int32_t i = a + c - 2;i >= a;i--)
            replace(vm->state,i);
    } else if(c < 1) {
        checkStack(vm->state->stack,1);
        push_int(vm->state,a);
    }
}
void callInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    //if(i == 16793828)
//	    printf("error!");
    int32_t nargs = __pushFuncAndArgs(ins.a,ins.b);
    Call(vm->state,nargs,ins.c - 1);
    __popResults(ins.a,ins.c);
    
}

void returnInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;

    if(ins.b > 1) {
        checkStack(vm->state->stack,ins.b - 1);
        for(int32_t i = ins.a;i <= ins.a+ins.b-2;i++) {
            push_value(vm->state,i);
        }
    } else if(ins.b < 1) {
        __fixStack(ins.a);
    }
}

void tailcallInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.c = 0;

    int32_t nargs = __pushFuncAndArgs(ins.a,ins.b);
    Call(vm->state,nargs, ins.c - 1);
    __popResults(ins.a,ins.c);
}

void selfInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;

    copy_value(vm->state,ins.b,ins.a + 1);
    getRK(vm->state,ins.c);
    GetTable(vm->state,ins.b);
    replace(vm->state,ins.a);
}
void LoadProto(int32_t idx) {
    Prototype *proto = vm->state->stack->lua_closure->proto->protos[idx];
    Closure *closure = newLuaClosure(proto,NULL);
    push(vm->state->stack,newLuaValue(LUAPP_TCLOSURE,closure,sizeof(Closure)));
}
void closureInst(instruction i) {
    struct code_format ins = ABx(i);
    ins.a++;

    LoadProto(ins.bx);
    replace(vm->state,ins.a);
}


void varargInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;

    if(ins.b != 1) {
        LoadVararg(vm->state,ins.b - 1);
        __popResults(ins.a,ins.b);
    }
}

void getTabUpInst(instruction i) {
    struct code_format ins = ABC(i);
    ins.a++;

    pushGlobalTable(vm->state);
    getRK(vm->state, ins.c);
    GetTable(vm->state, -2);
    replace(vm->state, ins.a);
    pop(vm->state->stack);
}