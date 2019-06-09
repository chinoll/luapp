#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.h"
#include "list.h"
#include "consts.h"
#include "lvalue.h"

#define defaultEntrySize 10
#define gethash(hashcode,map) (hashcode) % (1 << (map)->len)
#define loadLimit 600 //散列表中的最多元素数量占比(60%)

void *hashMapInit(HashMap *map,hashFunc hashfunc) {
    //初始化HashMap
    uint64_t size = 1 << defaultEntrySize;
    map->list = malloc(sizeof(list) *  size);
    if(map->list == NULL)
        return NULL;

    for(uint64_t i = 0;i < size;i++)
        list_init(map->list + i);

    map->len = defaultEntrySize;
    map->hash = hashfunc;
    map->count = 0;
    return map;
}

HashMapEntry *newHashMapEntry(void *key, void *value, uint64_t hashcode) {
    /*分配并初始化新的entry*/
    HashMapEntry *entry = malloc(sizeof(HashMapEntry));
    if(entry == NULL)
        return NULL;

    entry->key = key;
    entry->value = value;
    entry->hashcode = hashcode;

    list_init(&entry->list);
    return entry;
}

HashMap *newHashMap(hashFunc hashfunc) {
    /*分配并初始化HashMap结构*/

    HashMap *map = malloc(sizeof(HashMap));
    if(map == NULL)
        return NULL;

    return hashMapInit(map,hashfunc);
}

/*
 * 在散列表中查找一个值
 * @map:已经被初始化的散列表
 * @hashcode:哈希码
 * @len:key的长度
 * @equalFunc:比较函数
 */
HashMapEntry *getHashMapEntry(HashMap *map, uint64_t hashcode, void *key, uint64_t len, compareFunc equalFunc) {

    if(equalFunc == NULL)   //如果比较函数为NULL，则使用默认的比较函数
        equalFunc = memcmp;
    uint64_t hash = gethash(hashcode,map);  //获取hash

    list *pos;
    list_for_each(pos,&map->list[hash]) {//遍历链表
        HashMapEntry *entry = container_of(pos,HashMapEntry,list);
        if(hashcode == entry->hashcode)
            if(equalFunc(entry->key,key,len) == 0)  //处理碰撞
                return entry;
    }
    return NULL;
}

void * __getHashMapItem(HashMap *map, uint64_t hashcode, void *key, uint64_t len, compareFunc equalFunc) {
    return getHashMapEntry(map,hashcode,key,len,equalFunc)->value;
}
void *getHashMapItem(HashMap *map,void *key,uint64_t len, compareFunc equalFunc) {
    //从表中获取值
    uint64_t hashcode = map->hash(key,len,HASH_SEED);
    return __getHashMapItem(map,hashcode,key,len,equalFunc);
}

void expandHashMap(HashMap *map) {
    /*扩展散列表，扩展后的长度是原来长度的2倍*/
    list *entry;
    map->len++;
    uint64_t size = 1 << map->len;
    list *new = malloc(sizeof(list) *  size);
    for(uint64_t i = 0;i < size;i++)
        list_init(new + i);
    hashmap_for_each(entry,map) {
        if(list_empty(entry))
            continue;

        list *pos;
        list *pos2 = NULL;
        list_for_each(pos,entry) {
            if(pos2 == NULL) {
                pos2 = pos;
                continue;
            }

            HashMapEntry *entry = container_of(pos2,HashMapEntry,list);printf("%ld ",entry->hashcode % size);
            list_add(&entry->list,&new[entry->hashcode % size]);
            pos2 = pos;
        }
    }
    free(map->list);
    map->list = new;
}
void __putItemToHashMap(HashMap *map, uint64_t hashcode, void *key, void *value) {
    if(((map->count << defaultEntrySize) >> map->len) >= loadLimit) {
        //如果散列表内的entry数量达到散列表长度的60%则扩展散列表
        expandHashMap(map);
    }
    uint64_t hash = hashcode % (1 << map->len);
    HashMapEntry *entry = newHashMapEntry(key,value,hashcode);
    list_add(&entry->list,&map->list[hash]);
    map->count++;
}
/*
 * 将元素放入散列表map
 * @map:已经被初始化的散列表
 * @key:被散列的健
 * @len:键的长度
 * @value:放入散列表的值
 */
void putItemToHashMap(HashMap *map, void *key, uint64_t len, void *value) {
    uint64_t hashcode = map->hash(key,len,HASH_SEED);
    __putItemToHashMap(map,hashcode,key,value);
}

void deleteHashMapEntry(HashMap *map, HashMapEntry * entry) {
    //从散列表map中删除元素entry
    list_del(&entry->list);
    free(entry);
    map->count--;
}
void *__deleteHashMapItem(HashMap *map, uint64_t hashcode, void *key, uint64_t len, compareFunc equalFunc) {
    HashMapEntry *entry = getHashMapEntry(map,hashcode,key,len,equalFunc);
    void *val = entry->value;
    deleteHashMapEntry(map, entry);
    return val;
}
void *deleteHashMapItem(HashMap *map, void *key, uint64_t len, compareFunc equalFunc) {
    /*从散列表中删除entry并返回值*/
    uint64_t hashcode = map->hash(key, sizeof(len),HASH_SEED);
    return __deleteHashMapItem(map,hashcode,key,len,equalFunc);
}

void __reHash(HashMap *map, uint64_t oldhashcode, void *old_key, uint64_t old_len, uint64_t newhashcode, void *new_key, uint64_t new_len, compareFunc equalFunc) {

    HashMapEntry *entry = getHashMapEntry(map,oldhashcode,old_key,old_len,equalFunc);
    list_del(&entry->list);
    uint64_t hash = entry->hashcode % map->len;
    list_add(&entry->list,&map->list[hash]);

}

void reHash(HashMap *map,void *old_key,uint64_t old_len, void *new_key, uint64_t new_len, compareFunc equalFunc) {
    /*重新散列元素*/
    uint64_t oldhashcode = map->hash(old_key,old_len,HASH_SEED);
    uint64_t newhashcode = map->hash(new_key,new_len,HASH_SEED);
    __reHash(map,oldhashcode,old_key,old_len,newhashcode,new_key,new_len,equalFunc);
}


