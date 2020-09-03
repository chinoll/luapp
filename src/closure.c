#include <stdlib.h>
#include "lerror.h"
#include "closure.h"
#include "memory.h"
#include "lbinchunk.h"

Closure *newLuaClosure(Prototype *proto) {
    Closure * closure = (Closure *)lmalloc(sizeof(Closure));
    if(closure == NULL)
        panic(OOM);
    closure->proto = proto;
    return closure;
}

void freeLuaClosure(Closure *closure) {
	lfree(closure);
}
