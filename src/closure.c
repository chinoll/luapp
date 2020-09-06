#include <stdlib.h>
#include "lerror.h"
#include "closure.h"
#include "memory.h"
#include "lbinchunk.h"
#include "lstate.h"

Closure *newLuaClosure(Prototype *proto,CFunc f) {
    Closure * closure = (Closure *)lmalloc(sizeof(Closure));
    if(closure == NULL)
        panic(OOM);
    closure->proto = proto;
    closure->CFunction = f;
    return closure;
}

void freeLuaClosure(Closure *closure) {
	lfree(closure);
}
