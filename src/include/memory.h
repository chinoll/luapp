#ifndef LUAPP_MEMORY_H
#define LUAPP_MEMORY_H
#include <stddef.h>
void *lmalloc(size_t size);
void lfree(void *ptr);
#endif