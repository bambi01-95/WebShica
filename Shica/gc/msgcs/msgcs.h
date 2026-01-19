#ifndef MSGCS_H
#define MSGCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef NDEBUG // NDEBUGが「定義されていない」場合（デバッグビルド）
# define gc_debug_log(fmt, ...) (void)0
#else // NDEBUGが「定義されている」場合（リリースビルド）
# define gc_debug_log(fmt, ...) printf(fmt, __VA_ARGS__)
#endif

extern unsigned long gc_total;

#define MAXROOTS 64
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
    void **roots[MAXROOTS];
    unsigned nroots;
    gc_header *memory;  // start of memory
    gc_header *memend;  // end of memory (first byte after)
    gc_header *memnext; // next block to consider when allocating
};
typedef struct gc_context gc_context;

//-----------------------------
// USE ORIGINAL GC CONTEXT
unsigned int getOriginalGCtxNRoots(void); // get the number of original GC contexts
void setOriginalGCtxNRoots(unsigned int n); // set the number of original GC contexts
gc_context *origin_gc_ctx_ptr(void); // get pointer to the original GC context
//-----------------------------
// USE CURRENT GC CONTEXT   
extern gc_context *current_gc_ctx; // current GC context
//-----------------------------




gc_context *newGCContext(const int size); // create a new GC context with given size

void gc_check_ctx(const gc_context *g); // check the validity of the GC context



// msgcs.h に追加
#define GC_PTR(p) (p && ((void *)current_gc_ctx->memory <= p) && (p < (void *)current_gc_ctx->memend))

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

int gc_collectWithCleanup();

void *gc_beAtomic(void *p);
void *gc_alloc_atomic(int size);

#endif // MSGCS_H