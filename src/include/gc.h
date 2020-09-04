#ifndef LUAPP_GC_H
#define LUAPP_GC_H

#include "list.h"
#include "hashmap.h"
#include "lstack.h"

extern list rootSet;
extern uint64_t period;

#define ISGC (period < getMillisecond())
#define GCPERIOD 1000 //垃圾回收的默认周期，1000ms

void GC(LuaStack *stack);
void GCall(void);
uint64_t getMillisecond(void);
void update_period(void);
#endif //LUAPP_GC_H

