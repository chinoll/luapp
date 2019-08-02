#ifndef LUAPP_CONVERT_H
#define LUAPP_CONVERT_H

#include <stdint.h>
#include "lvalue.h"
#include "lstack.h"
//类型转换函数
LuaValue * ConvertToInt(LuaValue *t);
LuaValue * ConvertToFloat(LuaValue *t);
LuaValue * ConvertToString(LuaValue *t);
LuaValue * ConvertToBool(LuaValue *t);

#define ToIntX(stack,idx) ConvertToInt(get(stack,idx))
#define ToFloatX(stack,idx) ConvertToFloat(get(stack,idx))
#define ToStringX(stack,idx) ConvertToString(get(stack,idx))
#define ToBoolX(stack,idx) ConvertToBool(get(stack,idx))

int32_t fb2int(int32_t x);
int32_t int2fb(uint32_t x);
#endif //LUAPP_CONVERT_H
