//
// Created by cocoa on 18-12-2.
//
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "opcode.h"
#include "lerror.h"

struct code_format * ABC(instruction ins) {
    // iABC instruction
    struct code_format * cm = malloc(sizeof(struct code_format));
    if(cm == NULL)
        panic(OOM);
    cm->a = ins >> 6 & (uint32_t)0xff;
    cm->c  = ins >> 14 & (uint32_t)0x1ff;
    cm->b = ins >> 23 & (uint32_t)0x1ff;
    return cm;
}
struct code_format * ABx(instruction ins) {
    // iABx instruction
    struct code_format * cm = malloc(sizeof(struct code_format));
    if (cm == NULL)
        panic(OOM);
    cm->a = ins >> 6 & (uint32_t)0xff;
    cm->bx = ins >> 14;
    return cm;
}
struct code_format * AsBx(instruction ins) {
    // iAsBx instruction
    struct code_format * cm;
    cm = ABx(ins);
    cm->bx -= MAXARG_sBx;
    return cm;
}
struct code_format * Ax(instruction ins) {
    // iAx instruction
    struct code_format * cm = malloc(sizeof(struct code_format));
    if(cm == NULL)
        panic(OOM);
    cm->ax = ins >> 6;
    return cm;
}