#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>
#include <sys/time.h>
#include <stdbool.h>

#include "list.h"
#include "lvalue.h"
#include "gc.h"
#include "lstack.h"

list rootSet;
uint64_t getMillisecond(void) {
    //获取时间戳
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

void mark(LuaStack *stack) {
    //标记已死亡的对象
    LuaStack *stack1 = newLuaStack(128);
    for(uint64_t i = 1;i <= stack->top ;i++) {
        push(stack1,get(stack,i));  //遍历栈
        LuaValue *node;
        while(!luaStackEmpty(stack)) {
            int flag = true;
            node = pop(stack1);

            node->mark = true;

            for(uint64_t j = 0;j < node->ref_list_len;j++) {
                if(node->ref_list[j] != NULL) {
                    checkStack(stack1,1);
                    push(stack1, node->ref_list[j]);
                    node->ref_list[j]->mark = true;
                    flag = false;
                    }
            }
            if(flag)
                break;
        }
    }
    freeLuaStack(stack1);
}
void sweep(void) {
    //清楚对象
    list *pos;
    list *next = rootSet.next;
    list_for_each(pos,rootSet.next) {
        LuaValue *val = container_of(next,LuaValue,next);
        if(val->mark == false && val->stack_count == 0){
            freeLuaValue(val);
	}
        else
            val->mark = false;
        next = pos;
    }
}
void GC(LuaStack *stack) {
    /* 标记-清除算法*/
    mark(stack);
    sweep();
}

void GCall(void) {
    list *pos;
    list *n = rootSet.next;
    list_for_each(pos,rootSet.next) {
        LuaValue *val = container_of(n,LuaValue,next);
        freeLuaValue(val);
        n = pos;
    }
}
