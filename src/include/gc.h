#ifndef LUAPP_GC_H
#define LUAPP_GC_H

#include "list.h"
#include "hashmap.h"
#include "lstack.h"

list rootSet;
#define GCperiod 1000 //垃圾回收的默认周期，1000ms

void GC(LuaStack *stack);
void GCall(void);
uint64_t getMillisecond(void);

#endif //LUAPP_GC_H

