#ifndef LUAPP_TABLE_H
#define LUAPP_TABLE_H

#include <xxhash.h>
#include <stdint.h>
#include <stdint.h>
#include "list.h"
#include "lvalue.h"
#include "hashmap.h"
#include "table.h"

typedef struct __luatable {
    uint64_t *arr;
    uint64_t arr_len;   //arr字段的长度
    uint64_t free_arr_count;  //空闲arr字段个数
    HashMap *map;
} LuaTable;


LuaTable *newLuaTable(uint64_t nArr, uint64_t nRec);

LuaValue *__getMapItem(LuaTable *table, LuaValue *key);
LuaValue *__getArrayItem(LuaTable *table, LuaValue *key);
LuaValue *__getTableItem(LuaTable *table, LuaValue *key);
LuaValue *getTableItem(LuaValue *val,LuaValue *key);

void __putItemToMap(LuaTable *table, LuaValue *key, LuaValue *value);
void __putItemToArray(LuaTable *table, LuaValue *key, LuaValue *value);
void __putItemToTable(LuaTable *table, LuaValue *key, LuaValue *value);
void putItemTable(LuaValue *val,LuaValue *key,LuaValue *value);

LuaValue * __deleteMapItem(LuaTable *table,LuaValue *key);
LuaValue * __deleteArrayItem(LuaTable *table,LuaValue *key);
void *__deleteItem(LuaTable *table,LuaValue *key);
void deleteItem(LuaValue *val,LuaValue *key);

void __shrinkArray(LuaTable *table);

void __expandTable(LuaTable *table);
void __expandArray(LuaTable *table);
void __expandMap(LuaTable *table);


#endif //LUAPP_TABLE_H
