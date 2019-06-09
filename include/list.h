
#ifndef LUAPP_LIST_H
#define LUAPP_LIST_H

#include <stddef.h>

typedef struct __list {
    struct __list *next,*prev;
} list;

#define LIST_INIT(l) {&(l),&(l)}
#define container_of(ptr,type,member)   \
({   \
    const typeof(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type,member));   \
})
static inline void list_init(list *list) {
    //初始化链表
    list->next = list;
    list->prev = list;
}
#define list_for_each(pos,head) for((pos) = (head)->next;(pos) != (head);(pos) = (pos)->next)
static inline void __list_add(struct __list *new,struct __list *prev,struct __list *next) {
    next->prev = new;
    new->next = next;
    prev->next = new;
    new->prev = prev;
}
static inline void list_add(struct __list *new,struct __list *list) {
    //将一个元素添加到链表里
    __list_add(new,list,list->next);
}
static inline void __list_del(struct __list *prev,struct __list *next) {
    next->prev = prev;
    prev->next = next;
}
static inline void list_del(struct __list *entry) {
    //从链表中删除一个元素
    __list_del(entry->prev,entry->next);
}
static inline int list_empty(struct __list *list) {
    //判断链表是否为空
    return list->next == list;
}

#endif //LUAPP_LIST_H
