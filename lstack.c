
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lstack.h"
#include "lerror.h"
#include "lvalue.h"
#include "consts.h"

LuaValue * newLuaValue(int type,void * data) {
    LuaValue * val = (LuaValue *)malloc(sizeof(LuaValue));
    if(val == NULL)
        panic(OOM);
    val->type = type;
    val->data = data;
    return val;
}
void freeLuaValue(LuaValue * val) {
    free(val);
}
LuaStack * newLuaStack(uint64_t size) {
    LuaStack * s = (LuaStack *)malloc(sizeof(LuaStack));
    if(s == NULL)
        panic(OOM);
    s->slots = (LuaValue *)malloc(sizeof(LuaValue) * size);
    if(s->slots == NULL)
        panic(OOM);
    //memset(s->slots,0,sizeof(LuaValue) * size);
    for(uint64_t i = 0;i < size;i++) {
        s->slots[i].type = LUAPP_TNONE;
        s->slots[i].data = NULL;
    }
    s->top = 0;
    s->stack_len = size;
    return s;
}
void freeLuaStack(LuaStack * stack) {
    for (uint64_t i = 0;i < stack->top;i++) {
        switch(typeOf(stack->slots[i])) {
            case LUAPP_TFLOAT:
            case LUAPP_TSTRING:
                free(stack->slots[i].data);
                break;
        }
    }
    free(stack->slots);
    free(stack);
}
void check(LuaStack * stack,uint64_t n) {
    /*
     * 检查栈中的空间是否还可以推入至少n个值,如不满足该条件
     * 则对slots进行扩容
     */
    uint64_t len = stack->stack_len - stack->top;
    LuaValue * s = NULL;
    if(len < n) {
        s = (LuaValue *) malloc(sizeof(LuaValue) * (stack->stack_len + n));
        if(s == NULL)
            panic(OOM);
        memset(s,0,sizeof(LuaValue) * (stack->stack_len + n));
        for(uint i = 0;i < stack->top - 1;i++)
            s[i] = stack->slots[i];
        free(stack->slots);
        stack->slots = s;
    }
}
void push (LuaStack * stack,LuaValue val) {
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

    LuaValue * val = newLuaValue(-1,NULL);
    *val = stack->slots[stack->top];
    stack->slots[stack->top].type = LUAPP_TNIL;
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
        return &(stack->slots[absidx - 1]);
    return NULL;
}
void set(LuaStack *stack,int64_t idx,LuaValue * val) {
    uint64_t absidx = absIndex(stack,idx);
    if(isValid(stack,idx)) {
        stack->slots[absidx - 1] = *val;
        return;
    }
    panic("invalid index");
}
void reverse(LuaStack * stack,int64_t from,int64_t to) {
    LuaValue * slots = stack->slots;
    LuaValue tmp;
    while (from < to) {
        tmp = slots[from];
        slots[from] = slots[to];
        slots[to] = tmp;
        from++;
        to--;
    }
}