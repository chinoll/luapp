#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lvalue.h"
#include "convert.h"
#include "lerror.h"
#include "consts.h"
#include "lstack.h"

LuaValue * ConvertToFloat(LuaValue *t) {
    if(t == NULL || t->data == NULL)
        panic("The value of t and t->data cannot be NULL");
    LuaValue *val = newLuaValue(LUAPP_TFLOAT,NULL);
    double *n = malloc(sizeof(double));
    if(n == NULL)
        panic(OOM);
    val->data = (double *)n;
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
        default:
            val->data = (void *)n;
            val->convertStatus = false;
            return val;
    }
    val->convertStatus = true;
    return val;
}

LuaValue * ConvertToInt(LuaValue *t) {
    if(t == NULL)
        panic("The value of t cannot be NULL");
    LuaValue * val = newLuaValue(LUAPP_TINT,NULL);
    switch(t->type) {
        case LUAPP_TINT:
            val->data = t->data;
            break;
        case LUAPP_TFLOAT:
            val->data = (void *)((int64_t)(*(double *)t->data));
            break;
        case LUAPP_TSTRING:
            val->data = (void *)atoll(t->data);
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
    LuaValue *val = newLuaValue(LUAPP_TFLOAT,NULL);
    switch(t->type) {
        case LUAPP_TNIL:
            val->data = (void *)false;
            break;
        case LUAPP_TBOOLEAN: {
            val->data = t->data;
            val->convertStatus = true;
            break;
        }
        default:
            val->data = (void *)true;
    }
    return val;
}

LuaValue * ConvertToString(LuaValue *t) {
    //将值t转换为字符串
    if(t == NULL || t->data == NULL)
        panic("The value of t and t->data cannot be NULL");
    LuaValue * val = newLuaValue(LUAPP_TSTRING,NULL);
    switch(t->type) {
        case LUAPP_TSTRING: {
            val->data = t->data;
            val->convertStatus = true;
            break;
        }
        case LUAPP_TFLOAT: {
            char * str = (char *)malloc(sizeof(char) * 30);
            if(str == NULL)
                panic(OOM);
            sprintf(str,"%lf",*(double *)t->data);
            val->data = str;
            break;
        }
        case LUAPP_TINT:{
            char * str = (char *)malloc(sizeof(char) * 20);
            if(str == NULL)
                panic(OOM);
            sprintf(str,"%lld",t->data);
            val->data = str;
            break;
        }
        default: {
            val->data = (void *)"";
            break;
        }
    }
    return val;
}

