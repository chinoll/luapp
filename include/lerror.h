#ifndef __LUA_ERROR_H
#define __LUA_ERROR_H

#include <stdio.h>
#define panic(str) \
do {\
    fprintf(stderr,"%s:%d:%s\n",__FILE__,__LINE__,str);\
    exit(1);    \
} while(0)
#define OOM "Out of memory"
#endif
