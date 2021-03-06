#ifndef LUAPP_LVM_H
#define LUAPP_LVM_H

#include <stdint.h>
#include "lstate.h"
#include "lbinchunk.h"
#include "opcode.h"

#define OpName(ins) opcodes[get_opcode(ins)].name
#define OpMode(ins) opcodes[get_opcode(ins)].opMode
#define BMode(ins)  opcodes[get_opcode(ins)].argBMode
#define CMode(ins) opcodes[get_opcode(ins)].argCMode

typedef struct luavm {
    LuaState *state;
} LuaVM;

extern LuaVM *vm;

typedef void (*instructionAction)(instruction);
typedef enum OpArg {
    //Operand type
            OpArgN,
    OpArgU,
    OpArgR,
    OpArgK
} OpArg;
typedef struct code {
    char testFlag;
    char setAFlag;
    OpArg argBMode;
    OpArg argCMode;
    char opMode;
    char * name;  //code name
    instructionAction action;
} code_struct;
extern const code_struct codes[];

LuaVM *NewLuaVM(Prototype *prototype);
void freeLuaVM(LuaVM *lvm);
void ExecuteInstruction(LuaVM *lvm,instruction i);

#endif //LUAPP_LVM_H
