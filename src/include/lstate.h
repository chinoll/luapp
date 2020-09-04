
#ifndef LUAPP_LSTATE_H
#define LUAPP_LSTATE_H

#include <stdbool.h>
#include <math.h>
#include "lstack.h"
#include "consts.h"
#include "lbinchunk.h"
#include "lerror.h"
typedef int64_t ArithOp;        //按位和算术运算符
typedef int64_t CompareOp; //比较运算符

#define DefaultStackSize (20)

typedef struct __lua_state {
    LuaStack * stack;
} LuaState;

typedef struct __operator {
    int64_t (*intFunc)(int64_t,int64_t);
    double (*floatFunc)(double,double);
} operator;

LuaState * newLuaState(Prototype *prototype);
void freeLuaState(LuaState * state);
static inline uint64_t get_top(LuaState * state) {
    return state->stack->top;
}


void copy_value(LuaState * state,int64_t fromIdx,int64_t toIdx);
void push_value(LuaState * state,int64_t idx);
void replace(LuaState * state,int64_t idx);
#define  insert_value(state,idx) rotate(state,idx,1)
void remove_value(LuaState * state,int64_t idx);
void rotate(LuaState * state,int64_t idx,int64_t n);
void set_top(LuaState * state,int64_t idx);
//Push
void push_nil(LuaState * state);
void push_bool(LuaState * state,bool b);
void push_int(LuaState * state,int64_t n);
void push_num(LuaState * state, double n);
void push_string(LuaState * state,char * str);
//access
char * type_name(int tp);
int type(LuaState * state,int64_t idx);
#define isNone(state,idx) (type(state,idx) == LUA_TNONE)
#define isNil(state,idx) (type(state,idx) == LUA_TNIL)
#define isNoneOrNil(state,idx) (type(state,idx) <= LUA_TNIL)
#define isBool(state,idx) (type(state,idx) == LUA_TBOOLEAN)
static inline bool isString(LuaState * state,int64_t idx) {
    int t = type(state,idx);
    return t == LUAPP_TSTRING || t == LUAPP_TINT;
}
bool isNumber(LuaState * state,int64_t idx);
bool isInteger(LuaState * state,int64_t idx);
bool to_bool(LuaState * state,int64_t idx);
double to_number(LuaState * state,int64_t);
int64_t to_int(LuaState * state,int64_t idx);
char * to_string(LuaState * state,int64_t idx);

void Arith(LuaState * state,ArithOp op);
bool Compare(LuaState * state,int64_t idx1,int64_t idx2,CompareOp op);
void Len(LuaState * state,int64_t idx);
void Concat(LuaState * state,int64_t n,int32_t b);

static inline uint64_t getPC(LuaState *state) {
    return state->stack->pc;
}

static inline void addPC(LuaState *state,int64_t n) {
    state->stack->pc += n;
}

static inline uint32_t fetch(LuaState *state) {

    //取指函数
    //根据PC索引从指令表里取出当前指令，再将PC加1

    uint32_t i = state->stack->lua_closure->proto->code[state->stack->pc];
    state->stack->pc++;
    return i;
}

static inline void getConst(LuaState *state,int32_t idx) {

    //根据索引从常量表里，取出常量值，将其推入栈顶
    Type t = state->stack->lua_closure->proto->constants[idx];
    switch(t.type) {
        case TAG_NIL:
            push_nil(state);
            break;
        case TAG_BOOLEAN:
            push_bool(state,(bool)t.data);
            break;
        case TAG_INTEGER:
            push_int(state,(int64_t)t.data);
            break;
        case TAG_LONG_STR:
        case TAG_SHORT_STR:
            push_string(state,(char *)t.data);
            break;
        case TAG_NUMBER:
            push_num(state,(*(double *)t.data));
            break;
        default:
            panic("Error!");
            break;
    }
}

static inline void getRK(LuaState *state,int32_t rk) {

    //将某个常量推入栈顶，或者将某个索引处的栈值推入栈顶
    if(rk > 0xff) { //constant
        getConst(state, rk & 0xff);
    } else {    //register
        push_value(state,rk + 1);
    }
}
void CreateTable(LuaState *state,uint64_t nArr,uint64_t nRec);
int __getTable(LuaStack *stack,LuaValue *table,LuaValue *key);
int GetTable(LuaState *state,int64_t idx);
int GetField(LuaState *state, int64_t idx,char *k);
void __setTable(LuaValue *t,LuaValue *k,LuaValue *v);
void SetTable(LuaState *state, int64_t idx);
void SetI(LuaState *state, int64_t idx, int64_t i);

//函数调用相关
void callLuaClosure(LuaState *state,int64_t nargs,int64_t nresults,Closure *closure);
int64_t Load(FILE *fp, char *mode);
void Call(LuaState *state,int64_t nargs, int64_t nresults);
void LoadVararg(LuaState *state,int64_t n);
#endif //LUAPP_LSTATE_H
