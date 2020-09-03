#include <stdlib.h>
#include <stdint.h>
#include "lbinchunk.h"
#include "lvm.h"
#include "opcode.h"
#include "lerror.h"
#include "lstate.h"
#include "inst.h"
#include "memory.h"

LuaVM *vm;
uint64_t period;
const code_struct codes[] = {
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"MOVE",moveInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgN,.opMode = IABx,"LOADK",loadKInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgN,.argCMode = OpArgN,.opMode = IABx,"LOADKX",loadKxInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"LOADBOOL",loadBoolInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"LOADNIL",loadNilInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"GETUPVAL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgK,.opMode = IABC,"GETTABUP"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgK,.opMode = IABC,"GETTABLE",getTableInst},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SETTABUP"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"SETUPVAL"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SETTABLE",setTableInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"NEWTABLE",newTableInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgK,.opMode = IABC,"SELF",selfInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"ADD",addInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SUB",subInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"MUL",mulInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"MOD",modInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"POW",powInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"DIV",divInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"IDIV",idivInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"BAND",bandInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"BOR",borInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"BXOR",bxorInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SHL",shlInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SHR",shrInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"UNM",unmInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"BNOT",bnotInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"NOT",notInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"LEN",lenInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgR,.opMode = IABC,"CONCAT",concatInst},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"JMP",jmpInst},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"EQ",eqInst},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"LT",ltInst},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"LE",leInst},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgN,.argCMode = OpArgU,.opMode = IABC,"TEST",testInst},
        {.testFlag = 1,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgU,.opMode = IABC,"TESTSET",testSetInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"CALL",callInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"TAILCALL",tailcallInst},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"RETURN",returnInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"FORLOOP",forLoopInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"FORPREP",forPrepInst},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgN,.argCMode = OpArgU,.opMode = IABC,"TFORCALL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"TFORLOOP"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"SETLIST",setListInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABx,"CLOSURE",closureInst},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"VARARG",varargInst},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IAx,"EXTRAARG"},
};

LuaVM *NewLuaVM(Prototype *prototype) {
    LuaState *state = newLuaState(prototype);
    LuaVM *lvm = lmalloc(sizeof(LuaVM));
    if(lvm == NULL)
        panic(OOM);
    lvm->state = state;

    return lvm;
}
void freeLuaVM(LuaVM *lvm) {
    freeLuaState(lvm->state);
    free(lvm);
}
void ExecuteInstruction(LuaVM *lvm,instruction i) {
    void (*action)(instruction i) = codes[get_opcode(i)].action;
    if(action != NULL)
        action(i);
    else
        panic(codes[get_opcode(i)].name);
}
