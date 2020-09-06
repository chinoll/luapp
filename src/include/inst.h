#ifndef LUAPP_INST_H
#define LUAPP_INST_H

#include "opcode.h"
#include "lvm.h"

//移动和跳转指令
void moveInst(instruction i);
void jmpInst(instruction i);

//加载指令
void loadNilInst(instruction i);
void loadBoolInst(instruction i);
void loadKInst(instruction i);
void loadKxInst(instruction i);

//算术运算指令
void addInst(instruction i);
void subInst(instruction i);
void mulInst(instruction i);
void modInst(instruction i);
void powInst(instruction i);
void divInst(instruction i);
void idivInst(instruction i);
void bandInst(instruction i);
void borInst(instruction i);
void bxorInst(instruction i);
void shlInst(instruction i);
void shrInst(instruction i);
void unmInst(instruction i);
void bnotInst(instruction i);


//长度和拼接指令
void lenInst(instruction i);
void concatInst(instruction i);

//比较指令
void eqInst(instruction i);
void ltInst(instruction i);
void leInst(instruction i);

//逻辑运算指令
void notInst(instruction i);

void testSetInst(instruction i);
void testInst(instruction i);

//for循环指令
void forPrepInst(instruction i);
void forLoopInst(instruction i);

void getTableInst(instruction i);
void setTableInst(instruction i);
void newTableInst(instruction i);
void setListInst(instruction i);

//call
void callInst(instruction i);
void returnInst(instruction i);
void varargInst(instruction i);
void tailcallInst(instruction i);
void selfInst(instruction i);
void closureInst(instruction i);
void getTabUpInst(instruction i);
#endif //LUAPP_INST_H
