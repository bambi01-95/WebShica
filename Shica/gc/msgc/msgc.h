#ifndef MSGC_H
#define MSGC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXROOTS 1024
extern unsigned long gc_total; // total memory allocated by the GC

extern void **roots[MAXROOTS];	// pointers to the variables pointing to objects
extern int nroots;	// number of variables addresses in the roots stack

struct gc_header
{
    unsigned int size;		// size of this block, including header
    unsigned 	 busy : 1;	// this block is busy (reachable)
    unsigned 	 mark : 1;	// this block is marked (reachable) during GC
    unsigned 	 atom : 1;	// the data stored in this block contains no pointers
    // unsigned padding : 29; // アライメント調整
};

typedef struct gc_header gc_header;

void gc_init(int size);

void gc_pushRoot(const void *varp);
#define GC_PUSH(TYPE, VAR, INIT)		\
    TYPE VAR = INIT;				\
    gc_pushRoot((void *)&VAR)


#ifdef NDEBUG
void gc_popRoot(void);
#define GC_POP(VAR)				\
    gc_popRoot()
# define gc_debug_log(fmt, ...) printf(fmt, __VA_ARGS__)
#else // !NDEBUG -- enforce LIFO popping of roots
void gc_popRoot(void *varp,const char *name);
#define GC_POP(VAR)				\
    gc_popRoot((void *)&VAR, #VAR)
# define gc_debug_log(fmt, ...) ;
#endif //NDBUG

void gc_popRoots(int n)	;

void gc_mark(void *ptr);	// mark an object as reachable, then call the mark function...
void gc_markOnly(void *ptr)	;

typedef void (*gc_markFunction_t)(void *);
typedef void (*gc_collectFunction_t)(void);
void gc_defaultMarkFunction(void *ptr);
void gc_defaultCollectFunction(void);
extern gc_markFunction_t gc_markFunction;
extern gc_collectFunction_t gc_collectFunction;


void *gc_alloc(int lbs);
void gc_free(void *ptr);
int gc_collect(void);
char *gc_strdup(const char *s);
void *gc_realloc(void *oldptr, int newsize);

int gc_collectWithCleanup();

void *gc_beAtomic(void *p);
void *gc_alloc_atomic(int size);



#endif //MSGC_H