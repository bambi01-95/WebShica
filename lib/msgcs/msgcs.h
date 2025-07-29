#ifndef MSGCS_H
#define MSGCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef NDEBUG // NDEBUGが「定義されていない」場合（デバッグビルド）
# define gc_debug_log(fmt, ...) printf(fmt, __VA_ARGS__)
# define set_ctx(I) ({ \
    assert(I < gc_ctx.nroots); /* Iはインデックスなので<=ではなく<が適切 */ \
    assert(gc_ctx.roots[I] != NULL); \
    ctx = (gc_context*)gc_ctx.roots[I]; \
})
#else // NDEBUGが「定義されている」場合（リリースビルド）
# define gc_debug_log(fmt, ...) printf(fmt, __VA_ARGS__);
# define set_ctx(I) ctx = (gc_context*)gc_ctx.roots[I]
#endif

extern unsigned long gc_total;

#define MAXROOTS 1024
#define MAXCONTEXTS 16

struct gc_header
{
    unsigned int size;		// size of this block, including header
    unsigned 	 busy : 1;	// this block is busy (reachable)
    unsigned 	 mark : 1;	// this block is marked (reachable) during GC
    unsigned 	 atom : 1;	// the data stored in this block contains no pointers
    // unsigned padding : 29; // アライメント調整
};
typedef struct gc_header gc_header;

struct gc_context
{
    union {
        void **roots[MAXROOTS];
        void **subContexts[MAXCONTEXTS];
    };
    unsigned nroots;
    gc_header *memory;  // start of memory
    gc_header *memend;  // end of memory (first byte after)
    gc_header *memnext; // next block to consider when allocating
};
typedef struct gc_context gc_context;
extern unsigned int nctx; // number of processes (contexts) using the GC
extern gc_context gc_ctx; // whole memory context
extern gc_context *ctx; // current context

// msgcs.h に追加
#define GC_PTR(p) (p && ((void *)gc_ctx.memory <= p) && (p < (void *)gc_ctx.memend))

void gc_init(int size);

void gc_separateContext(const int nprocesses, const int nactiveprocesses);
gc_context *gc_getContextSlot(void); // get a new context for a new process

void gc_pushRoot(const void *varp);
#define GC_PUSH(TYPE, VAR, INIT)		\
    TYPE VAR = INIT;				\
    gc_pushRoot((void *)&VAR)

#ifdef NDEBUG
void gc_popRoot(void);
#define GC_POP(VAR)				\
    gc_popRoot()

#else // !NDEBUG -- enforce LIFO popping of roots
void gc_popRoot(void *varp,const char *name);
#define GC_POP(VAR)				\
    gc_popRoot((void *)&VAR, #VAR)

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


void *gc_beAtomic(void *p);
void *gc_alloc_atomic(int size);

#endif // MSGCS_H