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
    //将栈的idx处的值转换为布尔值，不改变栈的状态
    LuaValue * val =ToBoolX(state->stack,idx);
    bool b = (bool)val->data;
    freeLuaValue(val);
    return b;
}
double to_number(LuaState * state,int64_t idx) {
    //将栈的idx处的值转换为浮点数，不改变栈的状态
    LuaValue * val = ToFloatX(state->stack,idx);
    double n = *(double *)val->data;
    free(val->data);
    freeLuaValue(val);

    return n;
}
int64_t to_int(LuaState * state,int64_t idx) {
    //将栈的idx处的值转换为整数，不改变栈的状态
    LuaValue * val = ToIntX(state->stack,idx);
    int64_t n = (int64_t)val->data;
    freeLuaValue(val);
    return n;
}
char * to_string(LuaState * state,int64_t idx) {
    LuaValue * val =ToStringX(state->stack,idx);
    char *str = val->data;
    freeLuaValue(val);
    return str;

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

void Arith(LuaState * state,ArithOp op) {
    //执行算术和按位运算,将结果推入栈顶
    LuaValue *a,*b,*res;
    b = pop(state->stack);
    if(op != LUAPP_OPNUM && op != LUAPP_OPBNOT)
        a = pop(state->stack);
    else    //遇到单目运算符的情况
        a = b;
    if((res = __arith(a,b,operators[op])) != NULL) {
        push(state->stack,*res);
        freeLuaValue(res);
    } else {
        panic("arithmetic error!");
    }
    switch(b->type) {
        case LUAPP_TFLOAT:
        case LUAPP_TSTRING:
            free(b->data);
            break;
    }
    freeLuaValue(b);
    if(op != LUAPP_OPNUM && op != LUAPP_OPBNOT) {
        switch (a->type) {
            case LUAPP_TFLOAT:
            case LUAPP_TSTRING:
                free(a->data);
                break;
        }
        freeLuaValue(a);
    }
}
static LuaValue * __arith(LuaValue *a,LuaValue *b,operator op) {
    //进行实际的算术运算，并返回结果
    if(a == NULL || b == NULL || a->data == NULL || b->data == NULL)
        return NULL;
    int64_t ires;
    double fres;
    LuaValue * res;
    int flag = 0;
    if(op.floatFunc == NULL) {
        LuaValue * x = ConvertToInt(a);
        if(x->convertStatus == true) {
           LuaValue * y = ConvertToInt(b);
           if(y->convertStatus == true) {
               ires =  op.intFunc((int64_t)x->data,(int64_t)y->data);   //对整数进行算术运算
           }
           freeLuaValue(y);
        }
        freeLuaValue(x);
    } else {
        if(op.intFunc != NULL) {
            LuaValue *x = ConvertToInt(a);
            if(x->convertStatus == true) {
                LuaValue *y = ConvertToInt(b);
                if(y->convertStatus == true) {
                    ires = op.intFunc((int64_t)x->data,(int64_t)y->data);
                }
                freeLuaValue(y);
            }
            freeLuaValue(x);
        } else {
            LuaValue * x = ConvertToFloat(a);
            if(x->convertStatus == true) {
                LuaValue *y = ConvertToFloat(b);
                if(y->convertStatus == true) {
                    fres = op.floatFunc(*(double *)x->data,*(double *)y->data); //对浮点数进行算术运算
                    flag = 1;
                }
                freeLuaValue(y);
            }
            freeLuaValue(x);
        }
    }
    if(flag) {
        res = newLuaValue(LUAPP_TFLOAT,NULL);
        if(res == NULL)
            panic(OOM);
        double * n = (double *)malloc(sizeof(double));
        if(n == NULL)
            panic(OOM);
        *n = fres;
        res->data = (void *)n;
    } else {
        res = newLuaValue(LUAPP_TINT,(void *)ires);
        if(res == NULL)
            panic(OOM);
    }
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
            bool b = (bool) val->data == (bool) a->data;
            bool convertStatus = val->convertStatus;
            freeLuaValue(val);
            if (convertStatus && b)
                return true;
            else
                return false;
        }
        case LUAPP_TSTRING: {
            LuaValue * val = ConvertToString(b);
            bool b = strcmp(val->data,a->data);
            bool convertStatus = val->convertStatus;
            freeLuaValue(val);
            if(convertStatus && b)
                return true;
            else
                return false;
        }
        case LUAPP_TINT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToInt(b);
            printf("%d %d",a->data,val->data);
            bool b = (int64_t)val->data == (int64_t)a->data;
            freeLuaValue(val);
            return b;
        }
        case LUAPP_TFLOAT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToFloat(b);
            bool b = *(double *)val->data == *(double *)a->data;
            freeLuaValue(val);
            return b;
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
            bool b = strcmp(val->data,a->data) < 0;
            bool convertStatus = val->convertStatus;
            freeLuaValue(val);
            if(convertStatus && b)
                return true;
            else
                return false;
        }
        case LUAPP_TINT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToInt(b);
            bool b = (int64_t)val->data < (int64_t)a->data;
            freeLuaValue(val);
            return b;
        }
        case LUAPP_TFLOAT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToFloat(b);
            bool b = *(double *)val->data < *(double *)a->data;
            freeLuaValue(val);
            return b;
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
            bool b = strcmp(val->data,a->data) <= 0;
            bool convertStatus = val->convertStatus;
            freeLuaValue(val);
            if(convertStatus && b)
                return true;
            else
                return false;
        }
        case LUAPP_TINT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToInt(b);
            bool b = (int64_t)val->data <= (int64_t)a->data;
            freeLuaValue(val);
            return b;
        }
        case LUAPP_TFLOAT: {
            if(b->type != LUAPP_TINT && b->type != LUAPP_TFLOAT)
                return false;
            LuaValue *val = ConvertToFloat(b);
            bool b = *(double *)val->data <= *(double *)a->data;
            freeLuaValue(val);
            return b;
        }
        default:
            //暂时不处理其他的数据类型
            return false;
    }
}

void Len(LuaState * state,int64_t idx) {
    //获取指定索引处值的长度,将长度推入栈顶
    LuaValue *val = get(state->stack,idx);
    if(val->type != LUAPP_TSTRING)
        panic("length error!");
    LuaValue *res = newLuaValue(LUAPP_TINT,(void *)strlen(val->data));
    push(state->stack,*res);
    freeLuaValue(res);
}

void Concat(LuaState * state,int64_t n) {
    //从栈顶弹出n个值，对值进行拼接，把结果推入栈顶
    if(n == 0) {
        LuaValue * val = newLuaValue(LUAPP_TSTRING,"");
        push(state->stack,*val);
        freeLuaValue(val);
    } else if(n >= 2) {
        for(int64_t i = 1;i < n;i++) {
            if(isString(state,-1) && isString(state,-2)) {
                char *str1,*str2;
                str1 = to_string(state,-1);
                str2 = to_string(state,-2);
                char * str = malloc(sizeof(char)*strlen(str1) + sizeof(char)*strlen(str2));
                if(str == NULL)
                    panic(OOM);
                strcpy(str,str2);
                strcpy(str + strlen(str2),str1);
                freeLuaValue(pop(state->stack));
                freeLuaValue(pop(state->stack));
                free(str1);
                free(str2);
                LuaValue * val1 = newLuaValue(LUAPP_TSTRING,str);
                push(state->stack,*val1);
                freeLuaValue(val1);
                continue;
            }
            panic("concatenation error!");
        }
    }
    //n等于1时，什么都不做
}
