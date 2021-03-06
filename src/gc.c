#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>
#include <sys/time.h>
#include <stdbool.h>

#include "list.h"
#include "lvalue.h"
#include "gc.h"
#include "lstack.h"
#include "lvm.h"
#include "memory.h"
list rootSet;
uint64_t period;
uint64_t getMillisecond(void) {
    //获取时间戳
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

void mark(LuaStack *stack) {
    //标记已死亡的对象
    LuaStack *stack1 = newLuaStack(128,NULL);
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

    //遍历全局变量
    LuaValue *_env = getTableItem(vm->state->registery, newInt(LUAPP_RIDX_GLOBALS));    //全局变量
    LuaTable *table = _env->data;
    if(table->map != NULL) {
        HashMapEntry **entrys = getAllHashMapEntry(table->map);
        for(int i = 0;NULL != entrys[i];i++) {
            push(stack1, entrys[i]->key);
            push(stack1, entrys[i]->value);
            LuaValue *node;
            for(;;) {
                int flag = true;
                node = pop(stack1);

                node->mark = true;

                for(uint64_t j = 0;j < node->ref_list_len;j++) {    //遍历键-值关联的对象
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
        lfree(entrys);
    }
    freeLuaStack(stack1);
}
void sweep(void) {
    //清除对象
    list *pos;
    list *next = rootSet.next;
    list_for_each(pos,rootSet.next) {
        LuaValue *val = container_of(next,LuaValue,next);
        if(val->mark == false && val->stack_count == 0 && val->end_clean == false){
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

void update_period(void) {
    period += getMillisecond() + GCPERIOD;
}