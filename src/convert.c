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


LuaValue * ConvertToFloat(LuaValue *t) {
    if(t == NULL || t->data == NULL)
        panic("The value of t and t->data cannot be NULL");

    double *n = malloc(sizeof(double));
    if(n == NULL)
        panic(OOM);

    *n = 0.0;
    switch(t->type) {
        case LUAPP_TFLOAT:
            *n = *(double *)t->data;
            break;
        case LUAPP_TINT:
            *n = (double)(*(int64_t *)t->data);
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
    if(t == NULL || t->data == NULL)
        panic("The value of t and t->data cannot be NULL");

    LuaValue * val = newLuaValue(LUAPP_TSTRING,NULL,0);
    switch(t->type) {
        case LUAPP_TSTRING: {
            setLuaValue(val,LUAPP_TSTRING,t->data,t->len);
            val->convertStatus = true;
            break;
        }
        case LUAPP_TFLOAT: {
            char * str = (char *)malloc(sizeof(char) * 30);
            if(str == NULL)
                panic(OOM);
            memset(str,0,30* sizeof(char));
            sprintf(str,"%lf",*(double *)t->data);
            setLuaValue(val,LUAPP_TSTRING,str,30);
            break;
        }
        case LUAPP_TINT: {
            char * str = (char *)malloc(sizeof(char) * 20);
            if(str == NULL)
                panic(OOM);
            memset(str,0,20 * sizeof(char));
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

