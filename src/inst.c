#include <stdbool.h>
#include <stdlib.h>
#include "inst.h"
#include "lstate.h"
#include "opcode.h"
#include "convert.h"
//Lua指令集
void moveInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;
    copy_value(&vm->state,ins.b,ins.a);
}

void jmpInst(instruction i,LuaVM *vm) {
    struct code_format ins = AsBx(i);
    addPC(&vm->state,ins.bx);
    if(ins.a != 0)
        panic("todo!");
}

void loadNilInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    push_nil(&vm->state);
    for(uint32_t i = ins.a;i < ins.a + ins.b;i++)
        copy_value(&vm->state,-1,i);
    popN(&vm->state,1);
}

void loadBoolInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;
    push_bool(&vm->state,ins.b != 0);
    replace(&vm->state,ins.a);
    if(ins.c != 0) {
        addPC(&vm->state,1);
    }
}

void loadKInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABx(i);
    //printf("address:%p\n",ins);
    ins.a++;
    getConst(&vm->state,ins.bx);
    replace(&vm->state,ins.a);
}

void loadKxInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABx(i);
    ins.a += 1;
    struct code_format ax = Ax(fetch(&vm->state));
    getConst(&vm->state,ax.ax);
    replace(&vm->state,ins.a);
}

inline static void __binaryArith(instruction i,LuaVM *vm,ArithOp op) {
    struct code_format ins = ABC(i);
    ins.a++;
    getRK(&vm->state,ins.b);
    getRK(&vm->state,ins.c);
    Arith(&vm->state,op);
    replace(&vm->state,ins.a);
}



void addInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPADD); }
void subInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPSUB); }
void mulInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPMUL); }
void modInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPMOD); }
void powInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPPOW); }
void divInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPDIV); }
void idivInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPIDIV); }
void bandInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPBAND); }
void borInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPBOR); }
void bxorInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPBXOR); }
void shlInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPSHL); }
void shrInst(instruction i,LuaVM *vm) { __binaryArith(i,vm,LUAPP_OPSHR); }

inline static void __unaryArith(instruction i,LuaVM *vm,ArithOp op) {
    struct code_format ins = ABC(i);
    ins.a++;
    getRK(&vm->state,ins.b);
    Arith(&vm->state,op);
    replace(&vm->state,ins.a);
}

void unmInst(instruction i,LuaVM *vm) { __unaryArith(i,vm,LUAPP_OPUNM); }
void bnotInst(instruction i,LuaVM *vm) { __unaryArith(i,vm,LUAPP_OPBNOT); }

void lenInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;
     Len(&vm->state,ins.b);
     replace(&vm->state,ins.a);
}

void concatInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;
    ins.c++;
    uint32_t n = ins.c - ins.b + 1;
    checkStack(vm->state.stack,n);
    for(uint32_t j = ins.b;j <= ins.c;j++)
        push_value(&vm->state,j);

    Concat(&vm->state,n,ins.b);
        replace(&vm->state,ins.a);
}

inline static void __compare(instruction i,LuaVM *vm,CompareOp op) {
    struct code_format ins = ABC(i);
    getRK(&vm->state,ins.b);
    getRK(&vm->state,ins.c);
    if(Compare(&vm->state,-2,-1,op)  != (ins.a != 0)) {
        addPC(&vm->state,1);
    }
    popN(&vm->state,2);
}

void eqInst(instruction i,LuaVM *vm) { __compare(i,vm,LUAPP_OPEQ); } // ==
void ltInst(instruction i,LuaVM *vm) { __compare(i,vm,LUAPP_OPLT); } //<
void leInst(instruction i,LuaVM *vm) { __compare(i,vm,LUAPP_OPLE); } // <=

void notInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);

    ins.a++;
    ins.b++;

    push_bool(&vm->state,to_bool(&vm->state,ins.b));
    replace(&vm->state,ins.a);
}

void testSetInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);

    ins.a++;
    ins.b++;

    if(to_bool(&vm->state,ins.b) == (ins.c != 0)) {
        copy_value(&vm->state,ins.b,ins.a);
    } else {
        addPC(&vm->state,1);
    }
}

void testInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;

    if(to_bool(&vm->state,ins.a) != (ins.c != 0))
        addPC(&vm->state,1);
}

void forPrepInst(instruction i,LuaVM *vm) {
    struct code_format ins = AsBx(i);
    ins.a++;

    push_value(&vm->state,ins.a);
    push_value(&vm->state,ins.a + 2);
    Arith(&vm->state,LUAPP_OPSUB);
    replace(&vm->state,ins.a);
    addPC(&vm->state,ins.bx);
}

void forLoopInst(instruction i,LuaVM *vm) {
    struct code_format ins = AsBx(i);
    ins.a++;
    push_value(&vm->state,ins.a + 2);
    push_value(&vm->state,ins.a);
    Arith(&vm->state,LUAPP_OPADD);
    replace(&vm->state,ins.a);

    bool isStep = to_int(&vm->state,ins.a + 2) >= 0;
    if((isStep && Compare(&vm->state,ins.a,ins.a + 1,LUAPP_OPLE)) || \
        (!isStep && Compare(&vm->state,ins.a + 1,ins.a,LUAPP_OPLE))) {
        addPC(&vm->state,ins.bx);
        copy_value(&vm->state,ins.a,ins.a + 3);
    }
}

void newTableInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;
    CreateTable(&vm->state,fb2int(ins.b),fb2int(ins.c));
    replace(&vm->state,ins.a);
}

void setTableInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;

    getRK(&vm->state,ins.b);
    getRK(&vm->state,ins.c);
    SetTable(&vm->state, ins.a);

}

void setListInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;

    if(ins.c > 0)
        ins.c--;
    else
        ins.c = Ax(fetch(&vm->state)).ax;

    int64_t idx = (int64_t)((int64_t )ins.c * LFIELDS_PER_FLUSH);

    for(uint64_t j = 1;j <= ins.b;j++) {
        idx++;
        push_value(&vm->state,ins.a + j);
        SetI(&vm->state,ins.a,idx);
    }
}

void getTableInst(instruction i,LuaVM *vm) {
    struct code_format ins = ABC(i);
    ins.a++;
    ins.b++;

    getRK(&vm->state,ins.c);
    GetTable(&vm->state,ins.b);
    replace(&vm->state,ins.a);
}

