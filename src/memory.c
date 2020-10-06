#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "lerror.h"
void *lmalloc(size_t size) {
    void *ptr = malloc(size);
    return ptr;
}

void lfree(void *ptr) {
    if(NULL == ptr) {
        printf("ERROR!!!");
        exit(0);
    }
    free(ptr);
}