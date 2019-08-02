#include <stdlib.h>
#include <xxhash.h>
#include <stdbool.h>
#include <string.h>
#include "lerror.h"
#include "lvalue.h"
#include "consts.h"
#include "hashmap.h"
#include "list.h"
#include "gc.h"
#include "table.h"

uint64_t addrhash(void * key,uint64_t len,uint64_t seed) {
    /*将LuaValue结构的内存地址进行散列计算*/
    uint64_t addr = (uint64_t)key;
    return XXH64(&addr, sizeof(uint64_t), seed);
}
uint64_t dataHash(void *input,uint64_t length) {
    return XXH64((const void *)input,length,HASH_SEED);
}

LuaValue * newLuaValue(int type,void * data,uint64_t len) {
    //分配并初始化一个LuaValue结构体

    LuaValue * val = (LuaValue *)malloc(sizeof(LuaValue));
    if(val == (LuaValue *)0x55555556b9e0 || val == (LuaValue *)0x55555556b940)
        printf("discover!");
    if(val == NULL)
        panic(OOM);
    initLuaValue(val,type,data,len);
    return val;
}

int intCmp(const void * s1, const void * s2, size_t n) {
    int64_t n1 = (int64_t)((LuaValue *)s1)->data;
    int64_t n2 = (int64_t)((LuaValue *)s2)->data;
    return (int)(n1 - n2);
}
int floatCmp(const void *s1,const void *s2,size_t n) {
    int64_t *n1,*n2;
    n1 = (int64_t *)((LuaValue *)s1)->data;
    n2 = (int64_t *)((LuaValue *)s2)->data;
    return (int)(*n1 - *n2);
}
int stringCmp(const void *s1,const void *s2,size_t n) {
    const char *n1 = ((LuaValue *)s1)->data;
    const char *n2 = ((LuaValue *)s2)->data;
    return strcmp(n1,n2);
}

void initLuaValue(LuaValue *val,int type,void *data,uint64_t len) {
    //初始化LuaValue结构体
    val->type = type;
    val->data = data;
    val->convertStatus = false;
    val->ref_list_len = DefaultRefListExpandSize;
    val->len = len;
    val->addr_hashcode = addrhash(val, sizeof(void *),HASH_SEED);
    switch(type) {
        case LUAPP_TINT:
            val->eqfunc = intCmp;
            uint64_t data1 = (uint64_t)data;
            val->data_hashcode = dataHash(&data1, len);
            break;
        case LUAPP_TFLOAT:
            val->eqfunc = floatCmp;
            val->data_hashcode = dataHash(data,len);

            break;
        case LUAPP_TSTRING:
            val->eqfunc = stringCmp;
            val->data_hashcode = dataHash(data,len);
            break;
        default:
            val->eqfunc = memcmp;
    }
    val->ref_list = (LuaValue **)malloc(sizeof(LuaValue *) * DefaultRefListSize);
    memset((void *)val->ref_list, 0,DefaultRefListSize * sizeof(LuaValue *));
    list_init(&val->next);
    val->mark = false;
    list_add(&val->next,&rootSet);
}

void freeLuaValue(LuaValue * val) {
    switch(val->type) {
        case LUAPP_TFLOAT:
        case LUAPP_TSTRING:
            free(val->data);
            break;
        case LUAPP_TTABLE:
            freeLuaTable(val->data);
    }
    list_del(&val->next);
    free(val->ref_list);
    free(val);
}

void setLuaValue(LuaValue *val, int type, void *data, uint64_t len) {
    val->data = data;
    val->type = type;
    val->len = len;
    val->data_hashcode = dataHash(data,len);
}
/*
 * 扩展引用数组
 */
void __expanRefList(LuaValue *val) {
    LuaValue **ref_list = val->ref_list;
    val->ref_list = (LuaValue **)malloc(sizeof(LuaValue *) * (val->ref_list_len + DefaultRefListExpandSize));
    memcpy(val->ref_list,ref_list,val->ref_list_len * sizeof(LuaValue *));
    memset(val->ref_list + val->ref_list_len, 0, DefaultRefListExpandSize * sizeof(LuaValue *));
    val->ref_list_len += DefaultRefListExpandSize;
    free(ref_list);
}
/*
 * 为val对象添加一个引用
 */
void addRef(LuaValue *val,LuaValue *ref) {
    int flag = 1;

    for(uint64_t i = 0;i < ref->ref_list_len;i++) {
        //查找val对象是否被引用
        if(ref->ref_list[i] == NULL)
            continue;
        if(ref->ref_list[i]->addr_hashcode == val->addr_hashcode)
            if(memcmp(ref->ref_list[i], val, sizeof(LuaValue)) == 0)
                return;
    }

    for(uint64_t i = 0;i <ref->ref_list_len;i++) {
        //添加引用
        if(ref->ref_list[i] == NULL) {
            ref->ref_list[i] = val;
            flag = 0;
            break;
        }
    }
    if(flag)    //扩展引用数组
        __expanRefList(ref);
}
/*
 * 从ref中删除对val的引用
 */
void deleteRef(LuaValue *val,LuaValue *ref) {
    for(uint64_t i = 0;i < ref->ref_list_len;i++) {
        if(ref->ref_list[i]->addr_hashcode == val->addr_hashcode)
            if(memcmp(val,ref->ref_list[i], sizeof(LuaValue)) == 0) {
                ref->ref_list[i] = NULL;
                break;
            }
    }
}

LuaValue *newStr(const char *str) {
    uint64_t len = strlen(str) + 1;
    char *str1 = malloc(len);
    if(str1 == NULL)
        panic(OOM);
    strcpy(str1,str);
    return newLuaValue(LUAPP_TSTRING,(void *)str1,len);
}

LuaValue *newFloat(double n) {
    double *x = malloc(sizeof(double));
    if(x == NULL)
        return NULL;

    *x=  n;
    return newLuaValue(LUAPP_TFLOAT,(void *)x,LUAPP_TFLOAT);
}

LuaValue *NewTable(uint64_t nArr,uint64_t nRec) {
    LuaTable *table = newLuaTable(nArr,nRec);
    return newLuaValue(LUAPP_TTABLE,table,0);
}