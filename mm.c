#include <stdlib.h>
#include "mm.h"
#include "list.h"
#include "lerror.h"

Pool *pool_create(uint64_t num,uint64_t area_size,void (*ctor)(struct __pool *),void (*dtor)(struct __pool *)) {
    Pool *pool = (Pool *)malloc(sizeof(Pool));
    if(pool == NULL)
        panic(OOM);
    pool->buffer = malloc(area_size*num);
    if(pool->buffer == NULL)
        panic(OOM);

}
