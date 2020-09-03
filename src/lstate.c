#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "lstate.h"
#include "lerror.h"
#include "lstack.h"
#include "consts.h"
#include "lmath.h"
#include "convert.h"
#include "lbinchunk.h"
#include "table.h"
#include "lvm.h"
#include "memory.h"
#include "gc.h"

operator operators[] = {//算术运算函数
        {iadd,fadd},
        {isub,fsub},
        {imul,fmul},
        {imod,fmod},
        {NULL,pow},
        {NULL,fdiv},
        {iidiv,fidiv},
        {band,NULL},
        {bor,NULL},
        {bxor,NULL},
        {shl,NULL},
        {shr,NULL},
        {iunm,funm},
        {bnot,NULL}
};
static LuaValue * __arith(LuaValue *a,LuaValue *b,operator op);

static bool __eq(LuaValue *a,LuaValue *b);
static bool __lt(LuaValue *a,LuaValue *b);
static bool __le(LuaValue *a,LuaValue *b);

LuaState * newLuaState(Prototype *prototype) {
    LuaState * state = (LuaState *)lmalloc(sizeof(LuaState));
    if(state == NULL)
        panic(OOM);
    state->stack = newLuaStack(DefaultStackSize);
    return state;
}
void freeLuaState(LuaState * state) {
    LuaStack *s = state->stack;
    for(LuaStack *stack = state->stack->prev;stack != NULL;stack = state->stack->prev) {
    	freeLuaStack(s);
	s = stack;
    }
    lfree(state);
}

void copy_value(LuaState * state,int64_t fromIdx,int64_t toIdx) {
    //将一个值复制到另一个位置
    LuaValue *val = get(state->stack,fromIdx);
    set(state->stack,toIdx,val);
}

void push_value(LuaState * state,int64_t idx) {
    //把指定索引处的值推入栈顶

    LuaValue * val = get(state->stack,idx);
    push(state->stack,val);
}
void push_nil(LuaState * state) {
    LuaValue * val = newNil();
    push(state->stack,val);
}
void push_bool(LuaState * state,bool b) {
   LuaValue * val = newLuaValue(LUAPP_TBOOLEAN,(void *)b, sizeof(bool));
   push(state->stack,val);

}
void push_int(LuaState * state,int64_t n) {
    LuaValue * val = newInt(n);
    push(state->stack,val);
}
void push_num(LuaState * state,double n) {
    LuaValue * val = newFloat(n);
    push(state->stack,val);
}
void push_string(LuaState * state,char * s) {
    LuaValue * val = newStr(s);
    push(state->stack,val);
}

void replace(LuaState * state,int64_t idx) {
    //将栈顶的值弹出，然后写入指定位置
    LuaValue * val = pop(state->stack);
    //uint64_t top = state->stack->top;
    //if(top < idx && top < state->stack->stack_len)
    //    set_top(state,state->stack->top + 1);
    set(state->stack,idx,val);

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
    popN(state->stack,1);

}

int type(LuaState * state,int64_t idx) {
    //根据索引值返回值的类型
    if(isValid(state->stack,idx)) {
        LuaValue * val = get(state->stack,idx);
        return typeOf(val);
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
	case LUAPP_TCLOSURE:
	    return "closure";
        default:
            return "userdata";
    }
}

bool isNumber(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    if(val->type == LUAPP_TFLOAT || val->type == LUAPP_TINT)
        return true;
    return false;
}

bool isInteger(LuaState * state,int64_t idx) {
    LuaValue * val = get(state->stack,idx);
    if(val->type == LUAPP_TINT)
        return true;
    return false;
}

bool to_bool(LuaState * state,int64_t idx) {
    //将栈的idx处的值转换为布尔值，不改变栈的状态
    LuaValue * val =ToBoolX(state->stack,idx);
    bool b = (bool)val->data;
    return b;
}
double to_number(LuaState * state,int64_t idx) {
    //将栈的idx处的值转换为浮点数，不改变栈的状态
    LuaValue * val = ToFloatX(state->stack,idx);
    double n = *(double *)val->data;

    return n;
}
int64_t to_int(LuaState * state,int64_t idx) {
    //将栈的idx处的值转换为整数，不改变栈的状态
    LuaValue * val = ToIntX(state->stack,idx);
    int64_t n = (int64_t)val->data;
    return n;
}
char * to_string(LuaState * state,int64_t idx) {
    LuaValue * val =ToStringX(state->stack,idx);
    char *str = val->data;
    return str;
}

void set_top(LuaState * state,int64_t idx) {
    //将栈顶设置为指定值
    int64_t new = absIndex(state->stack,idx);
    if(new < 0)
        panic("stack underflow!");
    int64_t n = state->stack->top - new;
    if (n > 0) {
        for(int64_t i = 0;i < n;i++)
            pop(state->stack);
    } else if (n < 0) {
        for(int64_t i = 0;i > n;i--) {
            push_nil(state);
        }
    }
}

void Arith(LuaState * state,ArithOp op) {
    //执行算术和按位运算,将结果推入栈顶
    LuaValue *a,*b,*res;
    b = pop(state->stack);
    if(op != LUAPP_OPUNM && op != LUAPP_OPBNOT)
        a = pop(state->stack);
    else    //遇到单目运算符的情况
        a = b;
    if((res = __arith(a,b,operators[op])) != NULL)
        push(state->stack,res);
    else
        panic("arithmetic error!");

}
static LuaValue * __arith(LuaValue *a,LuaValue *b,operator op) {
    //进行实际的算术运算，并返回结果
    if(a == NULL || b == NULL)
        return NULL;
    int64_t ires = 0;
    double fres = 0;
    LuaValue * res;
    int flag = 0;

    if(op.floatFunc == NULL) {
        LuaValue * x = ConvertToInt(a);
        if(x->convertStatus == true) {
           LuaValue * y = ConvertToInt(b);
           if(y->convertStatus == true) {
               ires =  op.intFunc((int64_t)x->data,(int64_t)y->data);   //对整数进行算术运算
           }
        }
    } else {
        if(op.intFunc != NULL) {
            LuaValue *x = ConvertToInt(a);
            if(x->convertStatus == true) {
                LuaValue *y = ConvertToInt(b);
                if(y->convertStatus == true) {
                    ires = op.intFunc((int64_t)x->data,(int64_t)y->data);
                }
            }
        } else {
            LuaValue * x = ConvertToFloat(a);
            if(x->convertStatus == true) {
                LuaValue *y = ConvertToFloat(b);
                if(y->convertStatus == true) {
                    fres = op.floatFunc(*(double *)x->data,*(double *)y->data); //对浮点数进行算术运算
                    flag = 1;
                }
            }
        }
    }
    if(flag)
        res = newFloat(fres);
    else
        res = newInt(ires);
    return res;
}

bool Compare(LuaState * state,int64_t idx1,int64_t idx2,CompareOp op) {
    //对指定索引处的值进行比较，返回结果，不改变栈的状态
    LuaValue * a = get(state->stack,idx1);
    LuaValue * b = get(state->stack,idx2);

    switch(op) {
        case LUAPP_OPEQ:
            return __eq(a,b);
        case LUAPP_OPLT:
            return __lt(a,b);
        case LUAPP_OPLE:
            return __le(a,b);
        default:
            panic("invalid compare op!");
    }
}
bool __eq(LuaValue *a,LuaValue *b) {
    //比较a和b的值是否相等
    switch(a->type) {
        case LUAPP_TNIL:
            return b->type == LUAPP_TNIL;
        case LUAPP_TBOOLEAN: {
            LuaValue *val = ConvertToBool(b);
            bool c = (bool) val->data == (bool) a->data;
            bool convertStatus = val->convertStatus;
            if (convertStatus && c)
                return true;
            else
                return false;
        }
        case LUAPP_TSTRING: {
            LuaValue * val = ConvertToString(b);
            bool c = strcmp(val->data,a->data);
            bool convertStatus = val->convertStatus;
            if(convertStatus && c)
                return true;
            else
                return false;
        }
        case LUAPP_TINT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToInt(b);
            bool c = (int64_t)val->data == (int64_t)a->data;
            return c;
        }
        case LUAPP_TFLOAT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToFloat(b);
            bool c = *(double *)val->data == *(double *)a->data;
            return c;
            }
        default:
            //暂时不处理其他的数据类型
            return false;
        }
    }
bool __lt(LuaValue *a,LuaValue *b) {
    //a是否小于b
    switch(a->type) {
        case LUAPP_TSTRING: {
            LuaValue * val = ConvertToString(b);
            bool c = strcmp(val->data,a->data) < 0;
            bool convertStatus = val->convertStatus;
            if(convertStatus && c)
                return true;
            else
                return false;
        }
        case LUAPP_TINT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToInt(b);
            bool c = (int64_t)val->data < (int64_t)a->data;
            return c;
        }
        case LUAPP_TFLOAT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToFloat(b);
            bool c = *(double *)val->data < *(double *)a->data;
            return c;
        }
        default:
            //暂时不处理其他的数据类型
            return false;
    }
}
bool __le(LuaValue *a,LuaValue *b) {
    //a是否小于等于b
    switch(a->type) {
        case LUAPP_TSTRING: {
            LuaValue * val = ConvertToString(b);
            bool c = strcmp(val->data,a->data) <= 0;
            bool convertStatus = val->convertStatus;
            if(convertStatus && c)
                return true;
            else
                return false;
        }
        case LUAPP_TINT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToInt(b);
            bool c =  (int64_t)a->data <= (int64_t)val->data;
            return c;
        }
        case LUAPP_TFLOAT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToFloat(b);
            bool c = *(double *)val->data <= *(double *)a->data;
            return c;
        }
        default:
            //暂时不处理其他的数据类型
            return false;
    }
}

void Len(LuaState * state,int64_t idx) {
    //获取指定索引处值的长度,将长度推入栈顶
    LuaValue *val = get(state->stack,idx);
    LuaValue *res = NULL;
    if(val->type == LUAPP_TSTRING)
        res = newInt(strlen(val->data));
    else if(val->type == LUAPP_TTABLE)
        res = newInt(val->len);
    else
        panic("length error!");

    push(state->stack,res);
}

void Concat(LuaState * state,int64_t n,int32_t b) {
    //从栈顶弹出n个值，对值进行拼接，把结果推入栈顶
    //uint64_t b = state->stack->top - 1;
    if(n == 0) {
        LuaValue * val = newStr("");
        push(state->stack,val);
    } else if(n >= 2) {
        for(int64_t i = 1;i < n;i++) {
            if(isString(state, -1) && isString(state, -2)) {
                char *str1,*str2;
                str1 = to_string(state,-2);
                str2 = to_string(state,-1);
                char * str = lmalloc(sizeof(char)*strlen(str1) + sizeof(char)*strlen(str2) + 1);
                if(str == NULL)
                    panic(OOM);

                //lfree(str2);
                strcpy(str,str1);
                strcpy(str + strlen(str1),str2);

                pop(state->stack);
                pop(state->stack);

                LuaValue * val1 = newStr(str);
                push(state->stack,val1);
                lfree(str);
                continue;
            }
            panic("concatenation error!");
        }
    }
    //n等于1时，什么都不做
}

void CreateTable(LuaState *state,uint64_t nArr,uint64_t nRec) {
    //创建一个表，并将表推入栈中
    //@nArr:数组长度
    //@nRec:是否创建散列表，大于0则创建散列表
    LuaValue *val = NewTable(nArr,nRec);
    push(state->stack,val);
}

int __getTable(LuaStack *stack, LuaValue *table, LuaValue *key) {
    //从表中获取一个值，并将值推入栈中
    if(typeOf(table) != LUAPP_TTABLE)
        panic("Not a table!");
    LuaValue *res = getTableItem(table,key);
    push(stack,res);
    return typeOf(res);
}

int GetTable(LuaState *state, const int64_t idx) {
    //从表中获取一个值
    //@idx:表在栈中的位置
    LuaValue *t = get(state->stack,idx);
    LuaValue *k = pop(state->stack);

    return __getTable(state->stack, t, k);
}

int GetField(LuaState *state, const int64_t idx, char * k) {
    //从表中获取一个值
    LuaValue *t = get(state->stack, idx);
    LuaValue *v = newStr(k);

    return __getTable(state->stack,t,v);
}

int GetI(LuaState *state,const int64_t idx,const int64_t i) {
    //从表中获取一个值
    LuaValue *t = get(state->stack,idx);
    LuaValue *v = newInt(i);
    return __getTable(state->stack,t,v);
}
void __setTable(LuaValue *t,LuaValue *k,LuaValue *v) {
    //修改/添加/删除表中的值
    if (typeOf(t) == LUAPP_TTABLE) {
        putItemTable(t,k,v);
        return;
    }
    panic("Not a table!");
}

void SetTable(LuaState *state,const int64_t idx) {
    //修改/添加/删除表中的值
    LuaValue *t = get(state->stack,idx);
    LuaValue *v = pop(state->stack);
    LuaValue *k = pop(state->stack);
    __setTable(t,k,v);
}

void SetI(LuaState *state, const int64_t idx, int64_t i) {
    //修改/添加/删除表中的值
    LuaValue *t = get(state->stack,idx);
    LuaValue *v = pop(state->stack);
    LuaValue *k = newInt(i);
    __setTable(t,k,v);
}

void pushLuaStack(LuaState *state,LuaStack *stack) {
    stack->prev = state->stack;
    state->stack = stack;
}

void popLuaStack(LuaState *state) {
    LuaStack *stack = state->stack;
    state->stack = state->stack->prev;
    stack->prev = NULL;
}

void runLuaClosure(LuaState *state) {
    while(true) {
        uint64_t pc = getPC(state);
        instruction inst = fetch(state);
        ExecuteInstruction(vm, inst);
        if(debug_level > 1) {
            printf("[%ld] %s ", pc + 1, codes[get_opcode(inst)].name);
            printStack(state);
        }
        //if(period < getMillisecond())
            GC(state->stack);   //进行垃圾回收
        if(get_opcode(inst) == OP_RETURN)
            break;
    }
}

void callLuaRunClosure(LuaState *state,int64_t nargs,int64_t nresults,Closure *closure) {
    uint8_t nregs = closure->proto->max_stack_size;
    uint8_t nparams = closure->proto->num_params;
    LuaStack *newStack = newLuaStack(nregs + DefaultStackSize);
    newStack->lua_closure = closure;

    LuaValue **funcAndarg = popN(state->stack,nparams);
    pushN(newStack,funcAndarg + 1,nparams - 1,nparams);

    if(nargs > nparams && (closure->proto->is_vararg == 1)){
            newStack->varargs = funcAndarg + 1;
            newStack->varargs_len = 0;
    }
    if(newStack->varargs_len < 0)
        newStack->varargs_len = 0;
    pushLuaStack(state,newStack);
    set_top(state,nregs);
    runLuaClosure(state);
    popLuaStack(state);

    if(nresults != 0) {
        LuaValue **vals = popN(newStack,newStack->top - nregs);
        int64_t len;
        for(len = 0;vals[len] != NULL;len++)
            continue;
        checkStack(state->stack,len);
        pushN(state->stack,vals,len,len);
        lfree(vals);
    }
    lfree(funcAndarg);
}
int64_t Load(FILE *chunk,char *mode) {
    Prototype *proto = Undump(chunk);
    vm = NewLuaVM(proto);
    push(vm->state->stack,newLuaValue(LUAPP_TCLOSURE,newLuaClosure(proto),sizeof(Closure)));
    return 0;
}

void Call(LuaState *state,int64_t nargs,int64_t nresults) {
    LuaValue *val = get(state->stack,-(nargs + 1));
    int jk = 0;
    if(val != NULL && val->type == LUAPP_TCLOSURE) {
        Closure *closure = (Closure *)val->data;
	jk++;
        if(debug_level > 0)
            printf("call %s<%d,%d>\n",closure->proto->source,closure->proto->line_def,closure->proto->last_line_def);
        callLuaRunClosure(state,nargs,nresults,closure);
    } else {
        printf("not function!\n");
    }
}

void LoadVararg(LuaState *state,int64_t n) {
    if(n < 0) {
        n = state->stack->varargs_len;
    }

    checkStack(state->stack,n);
    pushN(state->stack,state->stack->varargs,state->stack->varargs_len,n);
}
