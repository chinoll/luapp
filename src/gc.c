#include <stdlib.h>
#include <stdint.h>
#include <xxhash.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "list.h"
#include "lerror.h"
#include "hashmap.h"
#include "lvalue.h"
#include "gc.h"

uint64_t getMillisecond(void) {
    //获取时间戳
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

void GC(void) {
    /*垃圾回收机制*/
    list *pos;
    list_for_each(pos,&GcList) {
        //回收对象
        LuaValue *val = container_of(pos,LuaValue,gclist);
        list_del(pos);
        list_del(&val->objlist);
        freeLuaValue(val);
    }
    list_for_each(pos,&ObjList) {
        //标记对象，将未被引用的对象加入回收队列
        LuaValue *val = container_of(pos,LuaValue,objlist);
        if(val->hasRoot)
            continue;
        if(val->gc_ref_count == 0)
            list_add(&val->gclist,&GcList);
    }
}

void GCall(void) {
    list *pos;
    list *next = ObjList.next;
    list_for_each(pos,(&ObjList)->next) {
        LuaValue *val = container_of(next,LuaValue,objlist);
        list_del(next);
        freeLuaValue(val);
        next = pos;
    }
}
