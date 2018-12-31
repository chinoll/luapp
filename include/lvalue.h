
#ifndef LUAPP_LVALUE_H
#define LUAPP_LVALUE_H

#include <stdbool.h>
#include "lbinchunk.h"
typedef struct __luavalue {
    void * data;
    int type;       //数据类型
    bool convertStatus; //转换状态;
}LuaValue;
static inline int typeOf(LuaValue value) {
    return value.type;
}
#endif //LUAPP_LVALUE_H
