#ifndef LUAPP_TABLE_H
#define LUAPP_TABLE_H

#include <xxhash.h>
#include <stdint.h>
#include <stdint.h>
#include "list.h"
#include "lvalue.h"
#include "hashmap.h"
#include "table.h"
#include "lstack.h"

typedef struct __luatable {
    struct __luatable *metatable;   //元表
    LuaValue **arr;
    uint64_t arr_len;   //arr字段的长度
    uint64_t free_arr_count;  //空闲arr字段个数
    HashMap *map;
    HashMap *keys;
    bool changed;
} LuaTable;
LuaTable *newLuaTable(uint64_t nArr, uint64_t nRec);

LuaValue *getTableItem(LuaValue *val,LuaValue *key);
void putItemTable(LuaValue *val,LuaValue *key,LuaValue *value);

void deleteItem(LuaValue *val,LuaValue *key);

uint64_t tableLen(LuaValue *val);
void freeLuaTable(void *ptr);
LuaValue *__getTableItem(LuaTable *table,LuaValue *key);
bool hasMetafield(LuaTable *table, const char *str);
LuaValue *nextKey(LuaValue *table, LuaValue *key);
void initKeys(LuaValue *ltable);
#endif //LUAPP_TABLE_H
