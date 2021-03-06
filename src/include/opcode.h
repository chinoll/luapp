#ifndef __LUAPP_OPCODE_H
#define __LUAPP_OPCODE_H
#include <stdint.h>
//Encoding
#define IABC  0
#define IABx  1
#define IAsBx 2
#define IAx   3

#define MAXARG_Bx (((uint32_t)1<<18) - 1)
#define MAXARG_sBx ((MAXARG_Bx )>> 1)
#define get_opcode(c) ((c) & (uint32_t)0x3f)

enum {
    //VM instruction
  OP_MOVE,
  OP_LOADK,
  OP_LOADKX,
  OP_LOADBOOL,
  OP_LOADNIL,
  OP_GETUPVAL,
  OP_GETTABUP,
  OP_GETTABLE,
  OP_SETTABUP,
  OP_SETUPVAL,
  OP_SETTABLE,
  OP_NEWTABLE,
  OP_SELF,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_MOD,
  OP_POW,
  OP_DIV,
  OP_IDIV,
  OP_BAND,
  OP_BOR,
  OP_BXOR,
  OP_SHL,
  OP_SHR,
  OP_UNM,
  OP_BNOT,
  OP_NOT,
  OP_LEN,
  OP_CONCAT,
  OP_JMP,
  OP_EQ,
  OP_LT,
  OP_LE,
  OP_TEST,
  OP_TESTSET,
  OP_CALL,
  OP_TAILCALL,
  OP_RETURN,
  OP_FORLOOP,
  OP_FORPREP,
  OP_TFORCALL,
  OP_TFORLOOP,
  OP_SETLIST,
  OP_CLOSURE,
  OP_VARARG,
  OP_EXTRAARG
};

typedef uint32_t instruction;

struct code_format {
 /*
  * iABC: Use a,b and c fields
  * iABx: Use a,bx field
  * iAsBx:Use a,bx field
  * iAx: Use ax field
  */
    union {
        int32_t a;
        int32_t ax;
    };
    union {
        int32_t b;
        int32_t bx;
    };
    int32_t c;
};

//Get instrucion encoding format
struct code_format ABC(instruction ins);
struct code_format ABx(instruction ins);
struct code_format AsBx(instruction ins);
struct code_format Ax(instruction ins);
#endif
