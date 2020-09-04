#ifndef LUAPP_MISC_H
#define LUAPP_MISC_H
#include "memory.h"
#include "lerror.h"
#include <string.h>

/*
    初始化type **类型的变量
    @val:被初始化的全局变量
    @val_len:记录val长度的整型变量
    @size:要分配的大小
*/
#define INITVALUE(val, val_len, size) \
{   \
    val = lmalloc(size * sizeof(typeof(val))); \
    if(NULL == val) \
        panic(OOM); \
    memset(val, 0, size * sizeof(typeof(val)));    \
    val_len = size; \
}

/*
    扩展type **变量
    @val:被扩展的变量
    @val_len:记录val长度的整形变量
*/
#define EXPANDVALUE(val, val_len) \
{   \
    val_len += 8;    \
    typeof(val) val2 = lmalloc(val_len * sizeof(typeof(val))); \
    if(NULL == val2)    \
        panic(OOM); \
    memset(val2, 0, val_len * sizeof(typeof(val))); \
    memcpy(val2, val, sizeof(sizeof(typeof(val))) * (val_len - 8));  \
    lfree(val); \
    val = val2; \
}

/*
    释放type **变量
    @val:被释放的变量
    @val_len:记录val长度的整型变量
    @freefunc:释放val变量的函数
*/
#define FREEVALUE(val, val_len, freefunc)    \
{   \
    for(typeof(val_len) i = 0;i < val_len;i++) {    \
        if(NULL == val[i])  \
            continue;   \
        freefunc(val[i]);   \
    }   \
    lfree(val); \
}

#endif