
#ifndef LUAPP_LVALUE_H
#define LUAPP_LVALUE_H

#include <stdbool.h>
#include <stddef.h>
#include "lbinchunk.h"
#include "list.h"

/*
 * 比较函数
 * 比较两个变量是否相等
 * 相等返回0
 */
typedef int (*compareFunc)(const void *, const void *, size_t);

#define DefaultRefListSize 5                    //引用链默认大小
#define DefaultRefListExpandSize    5   //引用链默认扩展大小

typedef struct __luavalue {
    void * data;
    int type;                                   //数据类型
    uint64_t len;                           //data字段长度
    int64_t ref_count;                  //引用计数
    bool convertStatus;               //转换状态
    uint64_t hashcode;
    compareFunc equalFunc;      //比较函数
    struct __luavalue **ref_list;   //引用链
    uint64_t ref_list_len;                  //引用链长度
    uint64_t gc_ref_count;               //被Gc Roots对象引用的次数
    int hasRoot;                                //是不是gcRoot对象
    list objlist;
    list gclist;
}LuaValue;

static inline int typeOf(LuaValue *value) {
    return value->type;
}


#define incRef(x) (x)->ref_count++
#define decRef(x) (x)->ref_count--
#define getRef(x) (x)->ref_count
#define DefaultRefList 10

void addRef(LuaValue *val,LuaValue *ref);
void deleteRef(LuaValue *val,LuaValue *ref);

LuaValue * newLuaValue(int type,void * data,uint64_t len);
void initLuaValue(LuaValue *val,int type,void *data,uint64_t len);
void freeLuaValue(LuaValue * val);
void setLuaValue(LuaValue *val, int type, void *data, uint64_t len);

int __intCmp(const void * s1, const void * s2, size_t len);
int __floatCmp(const void *s1, const void *s2, size_t len);

uint64_t addrhash(void * key,uint64_t len,uint64_t seed);
void __expanRefList(LuaValue *val);

#endif //LUAPP_LVALUE_H
