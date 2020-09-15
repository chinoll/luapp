#ifndef LUAPP_CLOSURE_H
#define LUAPP_CLOSURE_H
#include "lbinchunk.h"
#include "lvalue.h"
typedef struct __lua_closure Closure;
#include "lstate.h"

typedef struct __lua_upvalue {
    LuaValue *val;
} LuaUpvalue;

typedef struct __lua_closure {
    Prototype *proto;
    CFunc CFunction;
    LuaUpvalue **upvals;
    int upvals_len;
} Closure;

extern LuaUpvalue **global_upvals;
extern int global_upvals_size;

Closure * newLuaClosure(Prototype * proto);
Closure * newCClosure(CFunc f, int64_t nupvals);
void freeLuaClosure(Closure *closure);
LuaUpvalue * newLuaUpvalue(LuaValue * val);
void freeLuaUpvalue(LuaUpvalue *up);
LuaUpvalue *cloneLuaUpvalue(LuaUpvalue *up);
#endif
