#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>
#include <string.h>

#include "lerror.h"
#include "list.h"
#include "table.h"
#include "lvalue.h"
#include "consts.h"

LuaTable *newLuaTable(uint64_t nArr,uint64_t nRec) {
    //分配并初始化一个table
    LuaTable *table = malloc(sizeof(LuaTable));
    memset(table,0, sizeof(LuaTable));
    if(table == NULL)
        panic(OOM);

    if(nArr > 0) {//数组的长度
        table->arr = (LuaValue **)malloc(sizeof(LuaValue *) * nArr);
        table->arr_len = nArr;
        table->free_arr_count = nArr;
        memset(table->arr,0, sizeof(LuaValue *) * nArr);
        if(table->arr == NULL)
            panic(OOM);
    }

    if(nRec > 0) {
        table->map = newHashMap();
        if (table->map == NULL)
            panic(OOM);
    }
    return table;
}

static LuaValue *__getMapItem(LuaTable *table,LuaValue *key) {
    /*在散列表中获取一个LuaValue实例
     * 如果table中没有，则返回一个Nil实例*/

    HashMap *map = table->map;
    LuaValue *value = NULL;

    value = __getHashMapItem(map,key->data_hashcode,key,key->len,key->eqfunc);

    if(value == NULL)
        return newLuaValue(LUAPP_TNIL,NULL,0);
    return value;
}
static LuaValue *__getArrayItem(LuaTable *table,LuaValue *key) {
    return table->arr[(uint64_t)key->data - 1];
}
static LuaValue *__getTableItem(LuaTable *table,LuaValue *key) {
    /* 从table中获取一个LuaValue实例
     * 如果key的类型是数字，并且值小于数组长度时从数组中获取LuaValue实例，
     * 否则从散列表中查找
     */
    if(key->type == LUAPP_TNIL)
        return newLuaValue(LUAPP_TNIL,NULL,0);
    if(key->type == LUAPP_TINT && (uint64_t)key->data <= table->arr_len)
        return __getArrayItem(table,key);
    else
        return __getMapItem(table,key);
}
LuaValue *getTableItem(LuaValue *val,LuaValue *key) {
    LuaTable * table = (LuaTable *)val->data;
    return __getTableItem(table,key);
}

static int __putItemToMap(LuaTable *table,LuaValue *key,LuaValue *value) {
    /*将一个值放入散列表中*/
    HashMap *map = table->map;
    if(table->map == NULL) {
        table->map = newHashMap();
        map = table->map;
    }
    return __putItemToHashMap(map,key->data_hashcode,key, sizeof(LuaValue),value,key->eqfunc);
}

static void __shrinkArray(LuaTable *table) {
    for(uint64_t i = table->arr_len - 1;i > 0;i--) {
        if(table->arr[i] != NULL) {
            LuaValue **arr = malloc(i * sizeof(LuaValue *));
            if(arr == NULL)
                panic(OOM);
            memcpy(arr,table->arr,i * sizeof(LuaTable *));
        }
    }
}

static void __expandArray(LuaTable *table) {
    /*
     * 扩展Table的数组
     */
    uint64_t old = table->arr_len;
    table->arr_len = (table->arr_len/2 + table->arr_len);
    LuaValue **arr = (LuaValue **)malloc(table->arr_len * sizeof(LuaValue *));
    if(arr == NULL)
        panic(OOM);
    memset(arr, 0, table->arr_len * sizeof(LuaValue *));
    memcpy(arr,table->arr,old * sizeof(LuaValue *));
    free(table->arr);
    table->arr = arr;
    table->free_arr_count =  table->arr_len - (old - table->free_arr_count);    //计算空闲元素个数
}

static int __putItemToArray(LuaTable *table,LuaValue *key,LuaValue *value) {
    /*将一个值放入数组中*/
    int i = -1;
    if(table->free_arr_count <= table->arr_len/3)
        __expandArray(table);
    else if(table->free_arr_count >= table->arr_len/2)
        __shrinkArray(table);

    if(table->arr[(uint64_t)key->data - 1] == NULL) {
        table->free_arr_count--;
        i = 0;
    }

    table->arr[(uint64_t)key->data - 1] = value;
    return i;
}

static int __putItemToTable(LuaTable *table,LuaValue *key,LuaValue *value) {
    /*将一个值放入table中
     * 如果key的类型时数字并且值小于数组长度时，将值放入数组，
     * 否则将值反如散列表中
     */
    if(key->type == LUAPP_TNIL)
        panic("Key can't Nil");
    if(value->type == LUAPP_TNIL)
        return -1;
    if(key->type == LUAPP_TINT && (uint64_t)key->data <= table->arr_len)
        return __putItemToArray(table,key,value);
    else
         __putItemToMap(table,key,value);
    return -1;
}

void putItemTable(LuaValue *table,LuaValue *key,LuaValue *value) {
    /*
     * 将元素放入table中
     */
    if(__putItemToTable((LuaTable *)table->data,key,value) == 0) {
        //如果元素被放入数组中，则长度加1,如果被放入散列表中，则长度不增加
        table->len++;
    }
    addRef(value,table);
}

static LuaValue *__deleteMapItem(LuaTable *table, LuaValue *key) {
    //从HashMap里删除一个LuaValue实例
    LuaValue *entry = __deleteHashMapItem(table->map,key->data_hashcode,key,key->len,NULL);
    return entry;
}
static LuaValue *__deleteArrayItem(LuaTable *table,LuaValue *key) {
    //在数组中删除LuaValue实例
    LuaValue *entry = (LuaValue *)table->arr[(uint64_t)key->data];
    table->arr = NULL;
    return entry;
}

static void *__deleteItem(LuaTable *table,LuaValue *key) {
    //从table中删除LuaValue实例
    LuaValue *val = NULL;
    if (key->type != LUAPP_TINT || (uint64_t) key->data >= table->arr_len)
        val = __deleteMapItem(table, key);    //从数组中获取值
    else
        val = __deleteArrayItem(table, key);  //从散列表中获取值
    return val;
}

void deleteItem(LuaValue *tableval,LuaValue *key) {
    //面向LuaValue的接口
    LuaTable *table = (LuaTable *)tableval->data;
    LuaValue *val = __deleteItem(table,key);
    tableval->len--;
    deleteRef(val,tableval);
}

uint64_t tableLen(LuaValue *val) {
    //获取table数组和散列表的长度
    LuaTable *table = (LuaTable *)val->data;

    return table->arr_len + table->map->count;
}

void freeLuaTable(void *ptr) {
    //释放table
    LuaTable *table = (LuaTable *)ptr;
    free(table->arr);
    freeHashMap(table->map);
    free(table);
}
