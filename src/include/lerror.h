#ifndef __LUAPP_LERROR_H
#define __LUAPP_LERROR_H

#include <stdio.h>
#define panic(str) \
do {\
    fflush(stdout); \
    fprintf(stderr,"%s:%d:%s\n",__FILE__,__LINE__,str);\
    exit(1);    \
} while(0)
#define OOM "Out of memory"
#endif  //__LUAPP_LERROR_H
