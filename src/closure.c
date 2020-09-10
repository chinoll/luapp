#include <stdlib.h>
#include <string.h>
#include "lerror.h"
#include "closure.h"
#include "memory.h"
#include "lbinchunk.h"
#include "lstate.h"

Closure *newLuaClosure(Prototype *proto) {
    Closure * closure = (Closure *)lmalloc(sizeof(Closure));
    if(NULL == closure)
        panic(OOM);
    closure->proto = proto;
    closure->CFunction = NULL;
    
    if(proto->upvalues_len > 0) {
        closure->upvals = malloc(sizeof(LuaUpvalue *) * proto->upvalues_len);
        if(NULL == closure->upvals)
            panic(OOM);
        memset(closure->upvals, 0, sizeof(LuaUpvalue *) * proto->upvalues_len);
        closure->upvals_len = proto->upvalues_len;
    }
    return closure;
}

void freeLuaClosure(Closure *closure) {
	if(NULL != closure->upvals) {
        for(int i = 0;i < closure->upvals_len;i++) {
            if(NULL == closure->upvals[i])
                continue;
            lfree(closure->upvals[i]);
        }
        lfree(closure->upvals);
    }
    lfree(closure);
}

Closure * newCClosure(CFunc f, int64_t nupvals) {
    Closure * closure = (Closure *)lmalloc(sizeof(Closure));
    if(closure == NULL)
        panic(OOM);
    closure->proto = NULL;
    closure->CFunction = f;
    if(nupvals > 0) {
        closure->upvals = lmalloc(sizeof(LuaUpvalue *) * nupvals);
        if(NULL == closure->upvals)
            panic(OOM);
        memset(closure->upvals, 0, sizeof(LuaUpvalue *) * nupvals);
        closure->upvals_len = nupvals;
    }
    return closure;
}

LuaUpvalue * newLuaUpvalue(LuaValue * val) {
    LuaUpvalue *up = lmalloc(sizeof(LuaUpvalue));
    if(NULL == up)
        panic(OOM);
    up->val = val;
    return up;
}

void freeLuaUpvalue(LuaUpvalue *up) {
    lfree(up);
}