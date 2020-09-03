#ifndef LUAPP_CLOSURE_H
#define LUAPP_CLOSURE_H
#include "lbinchunk.h"

typedef struct __lua_closure {
    Prototype *proto;
} Closure;

Closure * newLuaClosure(Prototype * proto);
void freeLuaClosure(Closure *closure);
#endif
