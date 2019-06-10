
#ifndef LUAPP_LSTACK_H
#define LUAPP_LSTACK_H

#include <stdint.h>
#include <stdbool.h>
#include "lvalue.h"

typedef  struct __lua_stack {
    LuaValue **slots;
    uint64_t stack_len;
    uint64_t top;           //stack top index
} LuaStack;

LuaStack * newLuaStack(uint64_t size);
void freeLuaStack(LuaStack * stack);
void checkStack(LuaStack *stack,uint64_t n);
void push(LuaStack *stack,LuaValue *val);
LuaValue * pop(LuaStack * stack);
uint64_t absIndex(LuaStack *stack,int64_t idx);
bool isValid(LuaStack *stack,int64_t idx);
LuaValue * get(LuaStack *stack,int64_t idx);
void set(LuaStack *stack,int64_t idx,LuaValue *val);
void reverse(LuaStack * stack,int64_t from,int64_t to);
#endif //LUAPP_LSTACK_H
