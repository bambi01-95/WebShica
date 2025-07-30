#ifndef PARSER_H
#define PARSER_H

#ifdef MSGC
#include "../gc/msgc/msgc.h"
#define malloc(size) gc_alloc(size)
#define calloc(n, size) gc_alloc((n) * (size))
#define realloc(ptr, size) gc_realloc(ptr, size)
#define strdup(s) gc_strdup(s)
#else
#include <stdlib.h>
#include <string.h>
#endif




#ifdef MSGC
#undef malloc
#undef calloc
#undef realloc
#undef strdup
#endif // MSGC

#endif // PARSER_H