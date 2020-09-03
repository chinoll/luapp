
#ifndef LUAPP_LSTACK_H
#define LUAPP_LSTACK_H

#include <stdint.h>
#include <stdbool.h>
#include "lvalue.h"
#include "closure.h"
#define DEFAULT_GLOBAL_STACK_SIZE 8
typedef  struct __lua_stack {
    LuaValue **slots;
    struct __lua_stack *prev;
    Closure *lua_closure;
    LuaValue **varargs;
    int64_t varargs_len;
    uint64_t stack_len;
    uint64_t top;           //stack top index
    uint64_t pc;
} LuaStack;
#define luaStackEmpty(stack) ((stack)->top == 0)
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
LuaValue **popN(LuaStack *stack,int64_t n);
void pushN(LuaStack *stack,LuaValue **vals,int64_t vals_len,int64_t n);
void initStack(void);
void freeStack(void);
#endif //LUAPP_LSTACK_H
