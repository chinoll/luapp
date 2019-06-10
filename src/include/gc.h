#ifndef LUAPP_GC_H
#define LUAPP_GC_H

#include "list.h"
#include "hashmap.h"

list GcList;    //需要被回收的对象
list ObjList;   //在内存中的对象

#define GCperiod 1000 //垃圾回收的默认周期，1000ms

void GC(void);
void GCall(void);
uint64_t getMillisecond(void);

#endif //LUAPP_GC_H

