//
// Created by cocoa on 18-12-2.
//
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "opcode.h"
#include "lerror.h"
const opcode_struct opcodes[] = {
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"MOVE"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgN,.opMode = IABx,"LOADK"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgN,.argCMode = OpArgN,.opMode = IABx,"LOADKX"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"LOADBOOL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"LOADNIL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"GETUPVAL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgK,.opMode = IABC,"GETTABUP"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgK,.opMode = IABC,"GETTABLE"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SETTABUP"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"SETUPVAL"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SETTABLE"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"NEWTABLE"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgK,.opMode = IABC,"SELF"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"ADD"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SUB"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"MUL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"MOD"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"POW"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"DIV"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"IDIV"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"BAND"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"BOR"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"BXOR"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SHL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"SHR"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"UNM"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"BNOT"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"NOT"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IABC,"LEN"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgR,.opMode = IABC,"CONCAT"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"JMP"},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"EQ"},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"LT"},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgK,.argCMode = OpArgK,.opMode = IABC,"LE"},
        {.testFlag = 1,.setAFlag = 0,.argBMode = OpArgN,.argCMode = OpArgU,.opMode = IABC,"TEST"},
        {.testFlag = 1,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgU,.opMode = IABC,"TESTSET"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"CALL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"TAILCALL"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"RETURN"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"FORLOOP"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"FORPREP"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgN,.argCMode = OpArgU,.opMode = IABC,"TFORCALL"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgR,.argCMode = OpArgN,.opMode = IAsBx,"TFORLOOP"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IABC,"SETLIST"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABx,"CLOSURE"},
        {.testFlag = 0,.setAFlag = 1,.argBMode = OpArgU,.argCMode = OpArgN,.opMode = IABC,"VARARG"},
        {.testFlag = 0,.setAFlag = 0,.argBMode = OpArgU,.argCMode = OpArgU,.opMode = IAx,"EXTRAARG"}
};

struct code_format * ABC(uint32_t ins) {
    // iABC instruction
    struct code_format * cm = malloc(sizeof(struct code_format));
    if(cm == NULL)
        panic(OOM);
    cm->a = ins >> 6 & (uint32_t)0xff;
    cm->c  = ins >> 14 & (uint32_t)0x1ff;
    cm->b = ins >> 23 & (uint32_t)0x1ff;
    return cm;
}
struct code_format * ABx(uint32_t ins) {
    // iABx instruction
    struct code_format * cm = malloc(sizeof(struct code_format));
    if (cm == NULL)
        panic(OOM);
    cm->a = ins >> 6 & (uint32_t)0xff;
    cm->bx = ins >> 14;
    return cm;
}
struct code_format * AsBx(uint32_t ins) {
    // iAsBx instruction
    struct code_format * cm;
    cm = ABx(ins);
    cm->bx -= MAXARG_sBx;
    return cm;
}
struct code_format * Ax(uint32_t ins) {
    // iAx instruction
    struct code_format * cm = malloc(sizeof(struct code_format));
    if(cm == NULL)
        panic(OOM);
    cm->ax = ins >> 6;
    return cm;
}
