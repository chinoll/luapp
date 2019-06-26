
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "lstack.h"
#include "lerror.h"
#include "lvalue.h"
#include "consts.h"
#include "gc.h"
#include "hashmap.h"

LuaStack * newLuaStack(uint64_t size) {
    LuaStack * s = (LuaStack *)malloc(sizeof(LuaStack));
    if(s == NULL)
        panic(OOM);
    s->slots = (LuaValue **)malloc(sizeof(uint64_t *) * size);
    memset(s->slots,0, sizeof(uint64_t *) * size);

    s->top = 0;
    s->stack_len = size;
    return s;
}

void freeLuaStack(LuaStack * stack) {
    free(stack->slots);
    free(stack);
}

void checkStack (LuaStack * stack,uint64_t n) {
    /*
     * 检查栈中的空间是否还可以推入至少n个值,如不满足该条件
     * 则对slots进行扩容
     */

    uint64_t len = stack->stack_len - stack->top;
    LuaValue **s = NULL;
    if(len < n) {
        s = (LuaValue **)malloc(sizeof(LuaValue *) * (stack->stack_len + n));
        if(s == NULL)
            panic(OOM);
        memcpy(s,stack->slots, sizeof(void *) * (stack->top - 1));
        memset(s + stack->top - 1,0,sizeof(void *) * (stack->stack_len + n));

        free(stack->slots);
        stack->slots = s;
    }
}
void push (LuaStack * stack,LuaValue *val) {
    if (stack->top == stack->stack_len) {
        panic("stack overflow!");
    }
    stack->slots[stack->top] = val;
    stack->top++;
}
LuaValue * pop(LuaStack *stack) {
    if(stack->top < 1) {
        panic("stack underflow!");
    }
    stack->top--;
    LuaValue * val = stack->slots[stack->top];
    stack->slots[stack->top] = NULL;
    return val;
}
uint64_t absIndex(LuaStack * stack,int64_t idx) {
    /*
     * 将索引转换成绝对索引
     */
    if (idx >= 0)
        return idx;
    return idx + stack->top + 1;
}
bool isValid(LuaStack *stack,int64_t idx) {
    /*
     * 判断索引是否有效
     */
    uint64_t absIdx = absIndex(stack,idx);
    return absIdx > 0 && absIdx <= stack->top;
}
LuaValue *  get(LuaStack * stack,int64_t idx) {
    uint64_t absidx = absIndex(stack,idx);
    if(isValid(stack,idx))
        return (LuaValue *)(stack->slots[absidx - 1]);
    return NULL;
}
void set(LuaStack *stack,int64_t idx,LuaValue * val) {
    uint64_t absidx = absIndex(stack,idx);
    if(isValid(stack,idx)) {
        stack->slots[absidx - 1] = val;
        return;
    }
    panic("invalid index");
}
void reverse(LuaStack * stack,int64_t from,int64_t to) {
    LuaValue **slots = stack->slots;
    void *tmp;
    while (from < to) {
        tmp = slots[from];
        slots[from] = slots[to];
        slots[to] = tmp;
        from++;
        to--;
    }
}