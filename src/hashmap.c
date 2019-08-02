#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.h"
#include "list.h"
#include "consts.h"
#include "lvalue.h"

#define defaultEntrySize 10u
#define gethash(hashcode,map) (uint64_t)((hashcode) % (1u << (map)->len))
#define loadLimit 600u //散列表中的最多元素数量占比(60%)

void *hashMapInit(HashMap *map) {
    //初始化HashMap
    uint64_t size = 1 << defaultEntrySize;
    map->list = malloc(sizeof(list) *  size);
    if(map->list == NULL)
        return NULL;

    for(uint64_t i = 0;i < size;i++)
        list_init(map->list + i);

    map->len = defaultEntrySize;
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

HashMap *newHashMap(void) {
    /*分配并初始化HashMap结构*/

    HashMap *map = malloc(sizeof(HashMap));
    if(map == NULL)
        return NULL;

    return hashMapInit(map);
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
    HashMapEntry *entry = getHashMapEntry(map,hashcode,key,len,equalFunc);
    if(entry)
        return entry->value;
    else
        return NULL;
}
/*void *getHashMapItem(HashMap *map,void *key,uint64_t len, compareFunc equalFunc) {
    //从表中获取值
    uint64_t hashcode = map->hash(key,len,HASH_SEED);
    return __getHashMapItem(map,hashcode,key,len,equalFunc);
}*/

int expandHashMap(HashMap *map) {
    /*扩展散列表，扩展后的长度是原来长度的2倍*/
    list *entry;
    map->len++;
    uint64_t size = 1u << map->len; //计算散列表的大小
    list *new = malloc(sizeof(list) *  size);
    if(new == NULL)
        return -1;

    for(uint64_t i = 0;i < size;i++)
        list_init(new + i);

    hashmap_for_each(entry,map) {
        if(list_empty(entry))
            continue;

        list *pos;
        list *pos2 = entry->next;
        list_for_each(pos,entry) {
            HashMapEntry *e = container_of(pos2,HashMapEntry,list);
            list_add(&e->list,&new[e->hashcode % size]);
            pos2 = pos;
        }
    }

    free(map->list);
    map->list = new;
    return 0;
}

int __putItemToHashMap(HashMap *map, uint64_t hashcode, void *key, uint64_t len, void *value,compareFunc eq) {
    /*
     * 将一个元素放入散列表
     * @hashcode:key的散列值
     * @key:键
     * @value:值
     */
    if(((map->count << defaultEntrySize) >> map->len) >= loadLimit) {
        //如果散列表内的entry数量达到散列表长度的60%则扩展散列表
        if(expandHashMap(map) == -1) {
            fprintf(stderr,"Expanding hash table failed!\n");
            exit(1);
        }
    }
    uint64_t hash = hashcode % ((uint64_t )1 << map->len);
    HashMapEntry *entry = getHashMapEntry(map,hashcode,key,len,eq);
    if(entry == NULL) {
        entry = newHashMapEntry(key, value, hashcode);
        list_add(&entry->list, &map->list[hash]);
        map->count++;
        return 0;
    } else
        return -1;
}
/*
 * 将元素放入散列表map
 * @map:已经被初始化的散列表
 * @key:被散列的健
 * @len:键的长度
 * @value:放入散列表的值
*/
/*void putItemToHashMap(HashMap *map, void *key, uint64_t len, void *value) {


    uint64_t hashcode = map->hash(key,len,HASH_SEED);
    __putItemToHashMap(map,hashcode,key,value);
}*/

void deleteHashMapEntry(HashMap *map, HashMapEntry * entry) {
    //从散列表map中删除元素entry
    list_del(&entry->list);
    free(entry);
    map->count--;
}
void *__deleteHashMapItem(HashMap *map, uint64_t hashcode, void *key, uint64_t len, compareFunc equalFunc) {
    HashMapEntry *entry = getHashMapEntry(map,hashcode,key,len,equalFunc);
    if(entry == NULL)
        return NULL;
    void *val = entry->value;
    deleteHashMapEntry(map, entry);
    return val;
}
/*从散列表中删除entry并返回值*/
/*void *deleteHashMapItem(HashMap *map, void *key, uint64_t len, compareFunc equalFunc) {

    uint64_t hashcode = map->hash(key, sizeof(len),HASH_SEED);
    return __deleteHashMapItem(map,hashcode,key,len,equalFunc);
}*/

void freeHashMap(HashMap *map) {
    //释放散列表
    list *entry;
    hashmap_for_each(entry,map) {
        if(list_empty(entry))
            continue;

        list *pos;
        list *n = entry->next;
        list_for_each(pos,entry->next) {
            //遍历链表，逐个释放元素
            HashMapEntry *mentry = container_of(n,HashMapEntry,list);
            list_del(&mentry->list);
            free(mentry);
        }
    }
    free(map->list);
    free(map);
}


