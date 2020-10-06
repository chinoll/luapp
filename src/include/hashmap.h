#ifndef LUAPP_HASHMAP_H
#define LUAPP_HASHMAP_H

#include <stdint.h>
#include "list.h"
#include "lvalue.h"

typedef uint64_t (*hashFunc)(void *, uint64_t, uint64_t);

typedef struct __hashmapentry {
    void *value;
    void *key;
    uint64_t hashcode;
    list list;
    list nextkey;
} HashMapEntry;

typedef struct hashmap {
    list *list;
    list keyset;
    uint64_t len;          //散列表的长度的对数，实际长度为2<<len
    uint64_t count;     //散列表中的entry数量
} HashMap;

#define hashmap_for_each(entry,hashmap) for((entry) = (hashmap)->list;(entry) <  ((hashmap)->list + (1u << (hashmap)->len));(entry)++)
HashMap *newHashMap(void);
void *hashMapInit(HashMap *map);
HashMapEntry *newHashMapEntry(void *key,void *value,uint64_t hashcode);

void *__getHashMapItem(HashMap *map, uint64_t hashcode, void *key, uint64_t len, compareFunc equalFunc);
void *getHashMapItem(HashMap *map, void *key, uint64_t len, compareFunc equalFunc);

HashMapEntry * __putItemToHashMap(HashMap *map, uint64_t hashcode, void *key,uint64_t len, void *value,compareFunc eq);
//void putItemToHashMap(HashMap *map, void *key, uint64_t len, void *value);

int expandHashMap(HashMap *map);

void *__deleteHashMapItem(HashMap *map,uint64_t hashcode, void *key, \
        uint64_t len,compareFunc equalFunc);
/*void *deleteHashMapItem(HashMap *map, void *key, uint64_t len, \
        compareFunc equalFunc);

void __reHash(HashMap *map, uint64_t oldhashcode, void *old_key, uint64_t old_len, \
        uint64_t newhashcode, void *new_key, uint64_t new_len, compareFunc equalFunc);
void reHash(HashMap *map, void *old_key, uint64_t old_len, void *new_key, \
        uint64_t new_len, compareFunc equalFunc);
*/
void deleteHashMapEntry(HashMap *map, HashMapEntry * entry);

void freeHashMap(HashMap *map);
void **getAllKey(HashMap *map);
void **getAllValue(HashMap *map);
HashMapEntry **getAllHashMapEntry(HashMap *map);
#endif //LUAPP_HASHMAP_H
 