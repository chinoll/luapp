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

uint64_t addrhash(void * key,uint64_t len,uint64_t seed) {
    /*将LuaValue结构的内存地址进行散列计算*/
    uint64_t addr = (uint64_t)key;
    return XXH64(&addr, sizeof(uint64_t), seed);
}

LuaValue * newLuaValue(int type,void * data,uint64_t len) {
    //分配并初始化一个LuaValue结构体

    LuaValue * val = (LuaValue *)malloc(sizeof(LuaValue));
    if(val == NULL)
        panic(OOM);
    initLuaValue(val,type,data,len);
    return val;
}

void initLuaValue(LuaValue *val,int type,void *data,uint64_t len) {
    //初始化LuaValue结构体
    val->type = type;
    val->data = data;
    val->convertStatus = false;
    val->ref_count = val->ref_list_len = val->gc_ref_count = val->hasRoot = 0;
    val->len = len;
    val->hashcode = addrhash(val, sizeof(void *),HASH_SEED);
    val->ref_list = (LuaValue **)malloc(sizeof(LuaValue *) * DefaultRefListSize);

    list_init(&val->objlist);
    list_init(&val->gclist);

    val->equalFunc = memcmp;

    list_add(&val->objlist,&ObjList);
}

void freeLuaValue(LuaValue * val) {
    switch(val->type) {
        case LUAPP_TFLOAT:
        case LUAPP_TSTRING:
            free(val->data);
            break;
    }
    free(val->ref_list);
    free(val);
}

void setLuaValue(LuaValue *val, int type, void *data, uint64_t len) {
    val->hashcode = (uint64_t)val;
    val->data = data;
    val->type = type;
    val->len = len;
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
 * 将ref对象的一个引用指向val对象
 */
void addRef(LuaValue *val,LuaValue *ref) {
    int flag = 1;

    for(uint64_t i = 0;i < ref->ref_list_len;i++) {
        //查找val对象是否被引用
        if(ref->ref_list[i]->hashcode == val->hashcode)
            if(memcmp(ref->ref_list[i], val, sizeof(LuaValue)) == 0)
                return;
    }

    for(uint64_t i = 0;i <ref->ref_list_len;i++) {
        //将ref对象的引用指向val
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
        if(ref->ref_list[i]->hashcode == val->hashcode)
            if(memcmp(val,ref->ref_list[i], sizeof(LuaValue)) == 0) {
                ref->ref_list[i] = NULL;
                break;
            }
    }
}