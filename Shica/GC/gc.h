#ifndef GC_H
#define GC_H

#ifdef MSGC
#include "./msgc/msgc.h"
#define malloc(size) gc_alloc(size)
#define calloc(n, size) gc_alloc((n) * (size))
#define realloc(ptr, size) gc_realloc(ptr, size)
#define strdup(s) gc_strdup(s)
#elif defined(MSGCS)
#include "./msgcs/msgcs.h"
#define malloc(size) gc_alloc(size)
#define calloc(n, size) gc_alloc((n) * (size))
#define realloc(ptr, size) gc_realloc(ptr, size)
#define strdup(s) gc_strdup(s)
#else
  #error "GC is not defined, please define MSGC or MSGCS"
#endif


#endif // GC_H