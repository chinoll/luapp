
#ifndef LUAPP_LSTATE_H
#define LUAPP_LSTATE_H

#include <stdbool.h>
#include <math.h>
#include "lstack.h"
#include "consts.h"

typedef int64_t ArithOp;        //按位和算术运算符
typedef int64_t CompareOp; //比较运算符

typedef struct __lua_state {
    LuaStack * stack;
} LuaState;
LuaState * newLuaState(void);
void freeLuaState(LuaState * state);
static inline uint64_t get_top(LuaState * state) {
    return state->stack->top;
}
void popN(LuaState * state,uint64_t n);
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
void Concat(LuaState * state,int64_t n);


typedef struct __operator {
  int64_t (*intFunc)(int64_t,int64_t);
  double (*floatFunc)(double,double);
} operator;




#endif //LUAPP_LSTATE_H
