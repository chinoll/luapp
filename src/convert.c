#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <xxhash.h>
#include "lvalue.h"
#include "convert.h"
#include "lerror.h"
#include "consts.h"
#include "lstack.h"
#include "memory.h"

LuaValue * ConvertToFloat(LuaValue *t) {
    if(t == NULL || t->data == NULL)
        panic("The value of t and t->data cannot be NULL");

    double *n = lmalloc(sizeof(double));
    if(n == NULL)
        panic(OOM);

    *n = 0.0;
    switch(t->type) {
        case LUAPP_TFLOAT:
            *n = *(double *)t->data;
            break;
        case LUAPP_TINT:
            *n = (double)((int64_t)t->data);
            break;
        case LUAPP_TSTRING:
            *n = atof(t->data);
            break;
        default: {
            LuaValue *val = newLuaValue(LUAPP_TFLOAT, n, sizeof(double));
            val->convertStatus = false;
            return val;
        }
    }
    LuaValue *val = newLuaValue(LUAPP_TFLOAT,n, sizeof(double));
    val->convertStatus = true;
    return val;
}

LuaValue * ConvertToInt(LuaValue *t) {
    if(t == NULL)
        panic("The value of t cannot be NULL");
    LuaValue * val = newLuaValue(LUAPP_TINT,NULL,0);

    switch(t->type) {
        case LUAPP_TINT:
            setLuaValue(val,t->type,t->data,0);
            break;
        case LUAPP_TFLOAT:
            setLuaValue(val,t->type, (void *)((int64_t)(*(double *)t->data)),0);
            break;
        case LUAPP_TSTRING:
            setLuaValue(val,t->type,(void *)atoll(t->data),0);
            break;
        default:
            return val;
    }
    val->convertStatus = true;
    return val;
}

LuaValue * ConvertToBool(LuaValue *t) {
    //将值t转换为布尔值
    if(t == NULL)
        panic("The value of t cannot be NULL");

   bool b;
   bool converstatus;

    switch(t->type) {
        case LUAPP_TNIL:
            b = false;
            break;
        case LUAPP_TBOOLEAN: {
            b = t->data;
            converstatus = true;
            break;
        }
        default:
            b = true;
    }

    LuaValue *val = newLuaValue(LUAPP_TBOOLEAN,(void *)b,0);
    val->convertStatus = converstatus;

    return val;
}

LuaValue * ConvertToString(LuaValue *t) {
    //将值t转换为字符串
    if(t == NULL)
        panic("The value of t cannot be NULL");

    LuaValue * val = newLuaValue(LUAPP_TSTRING,NULL,0);
    switch(t->type) {
        case LUAPP_TSTRING: {
            char *str = lmalloc(t->len + 1);
            strcpy(str,t->data);
            setLuaValue(val,LUAPP_TSTRING,str,t->len);
            val->convertStatus = true;
            break;
        }
        case LUAPP_TFLOAT: {
            char * str = (char *)lmalloc(sizeof(char) * 30);
            if(str == NULL)
                panic(OOM);
            memset(str,0,30* sizeof(char));
            sprintf(str,"%lf",*(double *)t->data);
            setLuaValue(val,LUAPP_TSTRING,str,30);
            break;
        }
        case LUAPP_TINT: {
            char * str = (char *)lmalloc(sizeof(char) * 20);
            if(str == NULL)
                panic(OOM);
            //memset(str,0,20 * sizeof(char));
            sprintf(str,"%ld",(int64_t)t->data);
            setLuaValue(val,LUAPP_TSTRING,str,20);
            break;
        }
        default: {
            setLuaValue(val,LUAPP_TSTRING,NULL,0);
            break;
        }
    }
    return val;
}

int32_t fb2int(int32_t x) {
    if(x < 8)
        return x;
    else
        return ((x & 7) + 8) << (uint32_t)((x >> 3) - 1);
}

int32_t int2fb(uint32_t x) {
    int e = 0;
    if(x < 8) {
        x = (x + 0xf) >>4u;
        e += 4;
    }
    for(;x >= (8u << 1u);e++) {
        x = (x + 1u) >> 1u;
        e++;
    }

    return ((e + 1u)) << 3u | (x - 8);
}
