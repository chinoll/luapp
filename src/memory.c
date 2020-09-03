#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "lerror.h"
int i;
void *lmalloc(size_t size) {
    void *ptr = malloc(size);
    //if(ptr == (void *)0x80151e0)
//	    printf("ERROR!!!");
    //printf("%s():%d:ptr:%p\n",__FUNCTION__,i++,ptr);
    return ptr;
}

void lfree(void *ptr) {
    /*if(ptr == (void *)0x4bb7c90)
	    printf("i:%d\n",i);
    if(i == 353){
	    printf("ptr:%p\n",ptr);
	    printf("ERROR!!!\n");
    }*/
    //i++;
    //printf("%s():ptr:%p\n",__FUNCTION__,ptr);
    if(ptr == NULL)
	    printf("\n");
    free(ptr);
}
