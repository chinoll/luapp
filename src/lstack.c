
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
#include "memory.h"
#include "misc.h"
LuaStack **global_stack;
int global_stack_size;

LuaStack * newLuaStack(uint64_t size) {
    LuaStack * s = (LuaStack *)lmalloc(sizeof(LuaStack));
    if(NULL == s)
        panic(OOM);
    s->slots = (LuaValue **)lmalloc(sizeof(LuaValue *) * size);
    memset(s->slots,0, sizeof(LuaValue *) * size);

    s->top = 0;
    s->stack_len = size;
    s->pc = 0;
    s->prev = NULL;
    s->lua_closure = NULL;
    s->varargs = NULL;
    s->varargs_len = 0;
    for(int i = 0;i < global_stack_size;i++) {
	    if(NULL == global_stack[i]) {
		    global_stack[i] = s;
		    break;
	    }
	    if(i == (global_stack_size - 1))
		    EXPANDVALUE(global_stack, global_stack_size);
    }
    return s;
}

void freeLuaStack(LuaStack * stack) {
    lfree(stack->slots);
    lfree(stack);
    for(int i = 0;i < global_stack_size;i++)
	    if(global_stack[i] == stack)
		    global_stack[i] = NULL;
}

void checkStack (LuaStack * stack,uint64_t n) {
    /*
     * 检查栈中的空间是否还可以推入至少n个值,如不满足该条件
     * 则对slots进行扩容
     */

    uint64_t len = stack->stack_len - stack->top;
    LuaValue **s = NULL;
    if(len < n) {
        s = lmalloc(sizeof(LuaValue *) * (stack->stack_len + n));
        if(s == NULL)
            panic(OOM);
        memcpy(s,stack->slots, sizeof(void *) * (stack->top));
        memset(s + stack->top,0,sizeof(void *) *n);

        lfree(stack->slots);
        stack->slots = s;
    	stack->stack_len += n;
    }
}
void push (LuaStack * stack,LuaValue *val) {
    if (stack->top == stack->stack_len) {
        panic("stack overflow!");
    }

    stack->slots[stack->top] = val;
    stack->top++;
    if(val != NULL)
    	val->stack_count++;
}

LuaValue * pop(LuaStack *stack) {
    if(stack->top < 1) {
        panic("stack underflow!");
    }
    stack->top--;
    LuaValue * val = stack->slots[stack->top];
    stack->slots[stack->top] = NULL;
    val->stack_count--;
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
void  set(LuaStack *stack,int64_t idx,LuaValue * val) {
    uint64_t absidx = absIndex(stack,idx);
    if(isValid(stack,idx)) {
        stack->slots[absidx - 1] = val;
    	val->stack_count++;
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

LuaValue **popN(LuaStack *stack,int64_t n) {
    LuaValue **vals = lmalloc(sizeof(LuaValue *) * (n + 1));

    if(vals == NULL)
        panic(OOM);
    vals[n] = NULL;
    for(int64_t i = n - 1;i >= 0;i--)
        vals[i] = pop(stack);
    
    return vals;
}

void pushN(LuaStack *stack,LuaValue **vals,int64_t vals_len,int64_t n) {
    if(vals_len < 0)
        vals_len = 0;
    if(n < 0)
        n = vals_len;
    

    for(int64_t i = 0;i < n;i++) {
        if(i < vals_len)
            push(stack,vals[i]);
        else
            push(stack,newNil());
        
    }
}
