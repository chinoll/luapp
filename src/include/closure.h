#ifndef LUAPP_CLOSURE_H
#define LUAPP_CLOSURE_H
#include "lbinchunk.h"

typedef struct __lua_closure Closure;
#include "lstate.h"

typedef struct __lua_closure {
    Prototype *proto;
    CFunc CFunction;
} Closure;

Closure * newLuaClosure(Prototype * proto,CFunc f);
void freeLuaClosure(Closure *closure);
#endif
