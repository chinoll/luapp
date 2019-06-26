#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>

#include "lerror.h"
#include "list.h"
#include "table.h"
#include "lvalue.h"
#include "consts.h"

static inline uint64_t XXH64_2(void * key, uint64_t len, uint64_t seed) {
    return XXH64(key, len,seed);
}

LuaTable *newLuaTable(uint64_t nArr,uint64_t nRec) {
    //分配并初始化一个table
    LuaTable *table = malloc(sizeof(LuaTable));
    if(table == NULL)
        panic(OOM);

    if(nArr > 0) {//数组的长度
        table->arr = malloc(sizeof(LuaValue) * nArr);
        if(table->arr == NULL)
            panic(OOM);
    }

    if(nRec > 0)
        table->map = newHashMap(XXH64_2);
    if(table->map == NULL)
        panic(OOM);
    return table;
}

LuaValue *__getMapItem(LuaTable *table,LuaValue *key) {
    /*在散列表中获取一个LuaValue实例
     * 如果table中没有，则返回一个Nil实例*/

    HashMap *map = table->map;
    LuaValue *value = NULL;

    value = __getHashMapItem(map,key->hashcode,key,key->len,key->equalFunc);

    if(value == NULL)
        return newLuaValue(LUAPP_TNIL,NULL,0);
    return value;
}
LuaValue *__getArrayItem(LuaTable *table,LuaValue *key) {
    return (LuaValue *)table->arr[(uint64_t)key->data];
}
LuaValue *__getTableItem(LuaTable *table,LuaValue *key) {
    /* 从table中获取一个LuaValue实例
     * 如果key的类型是数字，并且值小于数组长度时从数组中获取LuaValue实例，
     * 否则从散列表中查找
     */
    if(key->type == LUAPP_TNIL)
        return newLuaValue(LUAPP_TNIL,NULL,0);
    if(key->type == LUAPP_TINT && (uint64_t)key->data < table->arr_len)
        return __getArrayItem(table,key);
    else
        return __getMapItem(table,key);
}
LuaValue *getTableItem(LuaValue *val,LuaValue *key) {
    LuaTable * table = (LuaTable *)val->data;
    return __getTableItem(table,key);
}

void __putItemToMap(LuaTable *table,LuaValue *key,LuaValue *value) {
    /*将一个值放入散列表中*/
    HashMap *map = table->map;
    __putItemToHashMap(map,key->hashcode,key,value);
}

void __putItemToArray(LuaTable *table,LuaValue *key,LuaValue *value) {
    /*将一个值放入散列表中*/
    uint64_t *arr = table->arr;
    if(((LuaValue *)arr[(uint64_t)key->data])->type > LUAPP_TNIL)
        table->free_arr_count--;
    arr[(uint64_t)key->data] = (uint64_t)value;
}

void __putItemToTable(LuaTable *table,LuaValue *key,LuaValue *value) {
    /*将一个值放入table中
     * 如果key的类型时数字并且值小于数组长度时，将值放入数组，
     * 否则将值反如散列表中
     */
    if(key->type == LUAPP_TNIL)
        panic("key can't Nil");
    if(key->type == LUAPP_TINT && (uint64_t)key->data < table->arr_len)
        __putItemToArray(table,key,value);
    else
        __putItemToMap(table,key,value);
}
void putItemToTable(LuaValue *table,LuaValue *key,LuaValue *value) {
    LuaTable *ltable = (LuaTable *)table->data;
    __putItemToTable(ltable,key,value);
    addRef(value,table);
}
LuaValue *__deleteMapItem(LuaTable *table, LuaValue *key) {
    //从HashMap里删除一个LuaValue实例
    LuaValue *entry = __deleteHashMapItem(table->map,key->hashcode,key,key->len,NULL);
    return entry;
}
LuaValue *__deleteArrayItem(LuaTable *table,LuaValue *key) {
    //在数组中删除LuaValue实例
    LuaValue *entry = (LuaValue *)table->arr[(uint64_t)key->data];
    table->arr = NULL;
    return entry;
}

void *__deleteItem(LuaTable *table,LuaValue *key) {
    //从table中删除LuaValue实例
    LuaValue *val = NULL;
    if(key->type == LUAPP_TINT && (uint64_t)key->data < table->arr_len)
         val = __deleteArrayItem(table,key);    //从数组中获取值
     else
         val = __deleteMapItem(table,key);  //从散列表中获取值
     return val;
}

void deleteItem(LuaValue *tableval,LuaValue *key) {
    //面向LuaValue的接口
    LuaTable *table = (LuaTable *)tableval->data;
    LuaValue *val = __deleteItem(table,key);
    deleteRef(val,tableval);
}