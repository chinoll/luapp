#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lstate.h"
#include "lerror.h"
#include "lstack.h"
#include "consts.h"
LuaState * newLuaState(void) {
    LuaState * state = malloc(sizeof(LuaState));
    if(state == NULL)
        panic(OOM);
    state->stack = newLuaStack(20);
    return state;
}
void freeLuaState(LuaState * state) {
    freeLuaStack(state->stack);
    free(state);
}
void popN(LuaState * state,uint64_t n) {
    //从栈顶弹出n个值
    LuaValue * val;
    for(uint64_t i = 0;i < n;i++) {
        val = pop(state->stack);
        switch(typeOf(*val)) {
            case LUAPP_TFLOAT:
            case LUAPP_TSTRING:
                free(val->data);
                break;
        }
        free(val);
    }
}
void copy_value(LuaState * state,int64_t fromIdx,int64_t toIdx) {
    //将一个值复制到另一个位置
    LuaValue *val = get(state->stack,fromIdx);
    set(state->stack,toIdx,val);
}
void push_value(LuaState * state,int64_t idx) {
    //把指定索引处的值推入栈顶

    LuaValue * val = get(state->stack,idx);
    push(state->stack,*val);
}
void push_nil(LuaState * state) {
    LuaValue * val = newLuaValue(LUAPP_TNIL,NULL);
    push(state->stack,*val);
    freeLuaValue(val);
}
void push_bool(LuaState * state,bool b) {
   LuaValue * val = newLuaValue(LUAPP_TBOOLEAN,(void *)b);
   push(state->stack,*val);
   freeLuaValue(val);
}
void push_int(LuaState * state,int64_t n) {
    LuaValue * val = newLuaValue(LUAPP_TINT,(void *)n);
    push(state->stack,*val);
    freeLuaValue(val);
}
void push_num(LuaState * state,double n) {
    double * t = (double *)malloc(sizeof(double));
    *t = n;
    LuaValue * val = newLuaValue(LUAPP_TFLOAT,(void *)t);
    push(state->stack,*val);
    freeLuaValue(val);
}
void push_string(LuaState * state,char * s) {
    uint64_t len = strlen(s);
    char * str = (char *)malloc(sizeof(char)*len);
    if(str == NULL)
        panic(OOM);
    strcpy(str,s);
    LuaValue * val = newLuaValue(LUAPP_TSTRING,str);
    push(state->stack,*val);
    freeLuaValue(val);
}
void replace(LuaState * state,int64_t idx) {
    //将栈顶的值弹出，然后写入指定位置
    LuaValue * val = pop(state->stack);
    set(state->stack,idx,val);
    freeLuaValue(val);
}
void rotate(LuaState * state,int64_t idx,int64_t n) {
    //将[idx,top]索引区间的值朝栈顶方向旋转n个位置
    uint64_t t = state->stack->top - 1;
    uint64_t p = absIndex(state->stack,idx)  - 1;
    uint64_t m;
    if(n >= 0)
        m = t - n;
    else
        m = p - n - 1;
    reverse(state->stack,p,m);
    reverse(state->stack,m + 1,t);
    reverse(state->stack,p,t);
}
void remove_value(LuaState * state,int64_t idx) {
    //删除指定索引处的值
    rotate(state,idx,-1);
    popN(state,1);
}
int type(LuaState * state,int64_t idx) {
    //根据索引值返回值的类型
    if(isValid(state->stack,idx)) {
        LuaValue * val = get(state->stack,idx);
        return typeOf(*val);
    }
    return LUAPP_TNONE;
}
char * type_name(int tp) {
    //返回值的类型的字符串
    switch(tp) {
        case LUAPP_TNONE:
            return "no value";
        case LUAPP_TNIL:
            return "nil";
        case LUAPP_TBOOLEAN:
            return "boolean";
        case LUAPP_TINT:
        case LUAPP_TFLOAT:
            return "number";
        case LUAPP_TSTRING:
            return "string";
        case LUAPP_TTABLE:
            return "table";
        case LUAPP_TFUNCTION:
            return "function";
        case LUAPP_TTHREAD:
            return "thread";
        default:
            return "userdata";
    }
}
bool isNumber(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    if(val->type == LUAPP_TFLOAT || val->type == LUAPP_TINT)
        return true;
}
bool isInteger(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    if(val->type == LUAPP_TINT)
        return true;
}
bool to_bool(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    switch(val->type) {
        case LUAPP_TNIL:
            return false;
        case LUAPP_TBOOLEAN:
            return ((bool)val->data);
        default:
            return true;
    }
}
double to_number(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    if(val->type == LUAPP_TINT)
        return (double) (int64_t)val->data;
    return *(double *)val->data;

}
int64_t to_int(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    return (int64_t)val->data;
}
char * to_string(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    switch(val->type) {
        case LUAPP_TSTRING:
            return (char *)val->data;
        case LUAPP_TFLOAT: {
            char * t = (char *)malloc(sizeof(char) * 30);
            if(t == NULL)
                panic(OOM);
            sprintf(t,"%lf",val->data);
            return t;
        }
        case LUAPP_TINT:{
            char * t = (char *)malloc(sizeof(char) * 20);
            if(t == NULL)
                panic(OOM);
            sprintf(t,"%lld",val->data);
            return t;
        }
        default:
            return "";
    }
}
void set_top(LuaState * state,int64_t idx) {
    //将栈顶设置为指定值
    int64_t new = absIndex(state->stack,idx);
    if(new < 0)
        panic("stack underflow!");
    int64_t n = state->stack->top - new;
    if (n > 0) {
        for(int64_t i = 0;i < n;i++) {
            LuaValue * val = pop(state->stack);
            switch(typeOf(*val)) {
                case LUAPP_TSTRING:
                case LUAPP_TFLOAT:
                    free(val->data);
            }
            free(val);
        }
    } else if (n < 0) {
        for(int64_t i = 0;i > n;i--) {
            push_nil(state);
        }
    }
}