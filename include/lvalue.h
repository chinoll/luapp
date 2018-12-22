
#ifndef LUAPP_LVALUE_H
#define LUAPP_LVALUE_H

#include "lbinchunk.h"
typedef struct __type LuaValue;
static inline int typeOf(LuaValue value) {
    return value.type;
}
#endif //LUAPP_LVALUE_H
