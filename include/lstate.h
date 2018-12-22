
#ifndef LUAPP_LSTATE_H
#define LUAPP_LSTATE_H

#include <stdbool.h>
#include "lstack.h"
#include "consts.h"
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
#define isString(state,idx) \
{   \
    int t = type(state,idx);    \
    (t == LUA_TSTRING || t == LUA_TNUMBER); \
}
bool isNumber(LuaState * state,int64_t idx);
bool isInteger(LuaState * state,int64_t idx);
bool to_bool(LuaState * state,int64_t idx);
double to_number(LuaState * state,int64_t);
int64_t to_int(LuaState * state,int64_t idx);
char * to_string(LuaState * state,int64_t idx);
#endif //LUAPP_LSTATE_H
