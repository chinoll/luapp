#ifndef LUAPP_INST_H
#define LUAPP_INST_H

#include "opcode.h"
#include "lvm.h"

//移动和跳转指令
void moveInst(instruction i,LuaVM *vm);
void jmpInst(instruction i,LuaVM *vm);

//加载指令
void loadNilInst(instruction i,LuaVM *vm);
void loadBoolInst(instruction i,LuaVM *vm);
void loadKInst(instruction i,LuaVM *vm);
void loadKxInst(instruction i,LuaVM *vm);

//算术运算指令
void addInst(instruction i,LuaVM *vm);
void subInst(instruction i,LuaVM *vm);
void mulInst(instruction i,LuaVM *vm);
void modInst(instruction i,LuaVM *vm);
void powInst(instruction i,LuaVM *vm);
void divInst(instruction i,LuaVM *vm);
void idivInst(instruction i,LuaVM *vm);
void bandInst(instruction i,LuaVM *vm);
void borInst(instruction i,LuaVM *vm);
void bxorInst(instruction i,LuaVM *vm);
void shlInst(instruction i,LuaVM *vm);
void shrInst(instruction i,LuaVM *vm);
void unmInst(instruction i,LuaVM *vm);
void bnotInst(instruction i,LuaVM *vm);


//长度和拼接指令
void lenInst(instruction i,LuaVM *vm);
void concatInst(instruction i,LuaVM *vm);

//比较指令
void eqInst(instruction i,LuaVM *vm);
void ltInst(instruction i,LuaVM *vm);
void leInst(instruction i,LuaVM *vm);

//逻辑运算指令
void notInst(instruction i,LuaVM *vm);

void testSetInst(instruction i,LuaVM *vm);
void testInst(instruction i,LuaVM *vm);

//for循环指令
void forPrepInst(instruction i,LuaVM *vm);
void forLoopInst(instruction i,LuaVM *vm);

void getTableInst(instruction i,LuaVM *vm);
void setTableInst(instruction i,LuaVM *vm);
void newTableInst(instruction i,LuaVM *vm);
void setListInst(instruction i,LuaVM *vm);

#endif //LUAPP_INST_H
