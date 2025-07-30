/*
=======================================================================
== WARNING: THIS GARBAGE COLLECTOR IS DESIGNED FOR SINGLE-THREAD USE ONLY.
== DO NOT USE IN A MULTI-THREADED ENVIRONMENT.
=======================================================================
*/

#ifndef MSGCS_C
#define MSGCS_C
#include "msgcs.h"
#include "fatal.h"

/* MEMO
 * Initialize the garbage collector with a given size of memory.
 * The memory is allocated and initialized to zero.
 * gc_ctx is a global context that holds whole memory.
 * | ------------------- gc_ctx -------------------- |
 * | gc_ctx1 | gc_ctx2 |  ...  | gc_ctxN_1 | gc_ctxN |
 */

unsigned int nctx = 0; // number of processes (contexts) using the GC
unsigned long gc_total = 0; // total memory allocated by the GC
struct gc_context gc_ctx = {
    // .roots = {0},// for momorize each process context pointers
    .nroots = 0, // capacity of process context pointers
    .memory = 0,
    .memend = 0,
    .memnext = 0
};

void gc_init(const int size)
{
    gc_ctx.memory = (void *)malloc(size);
    memset(gc_ctx.memory, 0, size);

    gc_ctx.memend = (void *)gc_ctx.memory + size;
    gc_ctx.memnext = gc_ctx.memory;
    // memory begins as a single free block the size of the entire memory
    gc_ctx.memory->size = size;
    gc_ctx.memory->busy = 0;
    gc_ctx.memory->mark = 0;
    gc_ctx.memory->atom = 0;
}

gc_context *ctx = &gc_ctx; // current context



void gc_pushRoot(const void *varp)	// push a new variable address onto the root stack
{
    if (ctx->nroots == MAXROOTS)fatal("gc root table full\n");
    ctx->roots[ctx->nroots++] = (void **)varp;
}

#ifdef NDEBUG
void gc_popRoot(void)    // remove the topmost variable address from the root stack
{
    --ctx->nroots;
}
#else // !NDEBUG -- enforce LIFO popping of roots
void gc_popRoot(void *varp,const char *name)    // pop a variable, checking it was the topmost on the stack
{
    assert(ctx->nroots > 0);
    --ctx->nroots;
    if (varp != ctx->roots[ctx->nroots])fatal("GC root '%s' popped out of order\n");
}
#endif //NDBUG

void gc_popRoots(const int n)
{
    assert(ctx->nroots >= n);
    ctx->nroots -= n;
}


void gc_defaultMarkFunction(void *ptr){
    gc_header *hdr = (gc_header *)ptr - 1;	// object address to header address
    if (hdr->atom) return;			// atomic objects do not contain pointers
    void *end = (void*)((char*)(hdr) + hdr->size);
    while (ptr < end) {
        gc_mark(*(void**)ptr); // dereference and pass to gc_mark
        ptr = (void*)((char*)ptr + sizeof(void*)); // increment pointer
    }
}
void gc_defaultCollectFunction(void){
    // This function can be overridden by the application to mark additional pointers
    // that are not in the root set or in objects.
    return;
}
// the application should provide a mark function that accurately marks only valid pointers
gc_markFunction_t gc_markFunction = gc_defaultMarkFunction;
gc_collectFunction_t gc_collectFunction = gc_defaultCollectFunction;




// TIP: it is used to mark the tip pointer address of array or atomic object
void gc_markOnly(void *ptr)
{
    if (!GC_PTR(ptr)) return;			// NULL or outside memory
    gc_header *here = (gc_header *)ptr - 1;	// object to header
    assert(ctx->memory <= here);
    assert(here < ctx->memend);
    assert(here->busy);
    if (here->mark) return;			// stop if already marked
    here->mark = 1;
}

// TIP: it is used to any object that is not marked
void gc_mark(void *ptr)
{
    if (!GC_PTR(ptr)) return;			// NULL or outside memory
    gc_header *here = (gc_header *)ptr - 1;	// object to header
    assert(ctx->memory <= here);
    assert(here < ctx->memend);
    assert(here->busy);
    if (here->mark) return;			// stop if already marked
    here->mark = 1;
    if (here->atom) return;			// stop if atomic (no pointers)
    gc_markFunction(ptr);			// recursively mark object contents
}


int gc_collect(void)
{
    // phase one: transitively trace the object graph starting at each root variable, setting
    // the mark bit in every object visited.
    // objects with the mark bit already set can be ignored since they have already been visited.

    int nfree = 0, nbusy = 0;			// count memory in use and free
    gc_collectFunction();			// run pre-collection function to mark static roots
    for (int i = 0;  i < ctx->nroots;  ++i)		// mark the pointers stored in each root variable
	gc_mark(*ctx->roots[i]);

    // phase two: sweep the memory looking for objects that are busy but do not have their
    // mark bit set.
    // these objects are unreachable from any of the roots, cannot ever take part in future
    // computation, and so can be collected as garbage.
    
    for ( gc_header *here = ctx->memory;		// iterate over all objects in memory
	  here < ctx->memend;
	  here = (void *)here + here->size ) {
        gc_debug_log("%p %c%c%c %d\n", (void *)here + sizeof(*here),
                here->busy ? 'B' : '-', here->atom ? 'A' : '-', here->mark ? 'M' : '-',
                here->size);
        if (here->mark) {			// block is marked reachable: do not reclaim
            here->mark = 0;
            assert(here->busy);			// if it is not allocated, the mutator has a bug
            nbusy += here->size;
            continue;
        }
        // block is not marked, is unreachable, and is therefore garbage
        gc_debug_log("%p RECLAIM\n", (void *)here + sizeof(*here));
        here->busy = 0;				// reclaim the block
        here->mark = 0;
        here->atom = 0;
        for (;;) {				// coalesce all following free blocks into this one
            gc_header *next = (void *)here + here->size;
            if (next == ctx->memend) break;	// current block is the last
            if (next->mark) break;		// next block is reachable
            assert(ctx->memory < next);
            assert(next < ctx->memend);
            gc_debug_log("%p EXTEND %p %d\n",
                    (void *)here + sizeof(*here),
                    (void *)next + sizeof(*next),
                    next->size);
            here->size += next->size;		// absorb following block into this one
        }
        nfree += here->size;
    }
    ctx->memnext = ctx->memory;		// start allocating at the start of memory
# ifndef NDEBUG
    printf("\r[GC %d used %d free]\r\n", nbusy, nfree);
    fflush(stdout);
# endif
    return nbusy;
}

void *gc_alloc(const int lbs){
    gc_total += lbs;
    if (lbs <= 0) return NULL;		// no allocation for zero or negative size
    // round up the allocation size to a multiple of the pointer size
    // TIP: It is used to align the size of the block to pointer size
    //      e.g. if pointer size is 8 bytes, then
    //      if lbs is 5, then size will be 16 (8 + sizeof(gc_header))
    int size = (lbs + sizeof(gc_header) + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
    assert(size >= sizeof(gc_header) + lbs); // ensure size is large enough
    gc_header *start = ctx->memnext, *here = start;	// start looking for a free block at memnext
    for (int retries = 0;  retries < 2;  ++retries) {	// try twice, before and after collecting
        do{
            gc_debug_log("%p ? %i %d\n", (void*)((char*)here + sizeof(*here)), here->size, here->busy);
            // this block is free and large enough
            if (!here->busy && here->size >= size) {	
                // split this block into two if the second block is large enough to hold a pointer
                if (here->size > size + sizeof(gc_header) + sizeof(long)) { // split it
                    gc_header *next = (gc_header *)((char *)here + size);
                    next->size = here->size - size;	// set size of the new block
                    here->size = size;			// shrink this block
                    next->busy = 0;			// make new next block free
                    next->mark = 0;			// reset mark flag
                    next->atom = 0;			// reset atom flag
                }
                ctx->memnext = (gc_header *)((char *)here + here->size);
                if (ctx->memnext == ctx->memend) ctx->memnext = ctx->memory;	// wraps back to start at the end
                assert(ctx->memory  <= ctx->memnext);
                assert(ctx->memnext <  ctx->memend);
                here->busy = 1;				// this block is now allocated
                gc_debug_log("%p ALLOC %d %d\n", (void*)((char*)here + sizeof(*here)), lbs, here->size);
                return (void*)(here + 1); // return pointer to the allocated
            }
            here = (gc_header *)((char *)here + here->size); // move to the next block
            if(here == ctx->memend) here = ctx->memory; // wrap around to the start
            assert(ctx->memory <= here);
            assert(here < ctx->memend);
        }while(here != start); // loop until we find a suitable block
        if(retries)break; // if we have already retried, break
        gc_collect(); // collect garbage and try again
    }
    printf("gc_alloc: no suitable block found for %d bytes\n", lbs);
    return NULL; // no suitable block found
}


char *gc_strdup(const char *s)
{
    int len = strlen(s);
    char *mem = (char*)gc_alloc(len + 1); // allocate memory for the string
    gc_debug_log("gc_strdup: allocated %d bytes for string '%s'\n", len, s);
    memcpy(mem, s, len); // copy the string into the allocated memory
    mem[len] = 0; // null-terminate the string
    return mem; // return the pointer to the allocated string
}

// TIP: it is used for atomic object (int *, char *, etc.)
void *gc_beAtomic(void *p){
    ((gc_header *)p)[-1].atom = 1; // set the atom flag in the header
    return p; // return the pointer to the object
}

// TIP: it is used to allocate atomic object (int *, char *, etc.)
void *gc_alloc_atomic(int size)
{
    return gc_beAtomic(gc_alloc(size)); // allocate memory and mark it as atomic
}

void *gc_realloc(void *oldptr,const int newsize)
{
    if (!oldptr) return gc_alloc(newsize); // if old pointer is NULL, allocate new memory
    gc_header *oldhdr = (gc_header *)oldptr - 1; // get the header of the old block
    int oldsize = oldhdr->size - sizeof(gc_header); // calculate the size of the old block
    if ( oldsize >= newsize		// object will fit into original block and
	 && oldsize < newsize * 2	// fills at least half of the newly requested size
       )
    return oldptr;			// don't change the size of the block
    gc_pushRoot(&oldptr);		// push the old pointer to the root stack
    void *newptr = oldhdr->atom ? 
        gc_alloc_atomic(newsize) : gc_alloc(newsize); // allocate new memory
    --ctx->nroots; // pop the old pointer from the root stack
    int len = newsize < oldsize ? newsize : oldsize; // determine the length to copy
    memcpy(newptr, oldptr, len); // copy the old data to the new memory
    gc_debug_log("gc_realloc: resized from %d to %d bytes\n", oldsize, newsize);
    return newptr; // return the pointer to the new memory
}
void gc_free(void *ptr)
{
    if (!GC_PTR(ptr)) return;			// NULL or outside memory
    gc_header *here = (gc_header *)ptr - 1;	// object to header
    assert(ctx->memory <= here);
    assert(here < ctx->memend);
    assert(here->busy);
    if (here->mark) return;			// stop if already marked
    here->busy = 0;				// reclaim the block
    here->mark = 0;
    here->atom = 0;
}

void print_gc_header(const gc_header *ptr)
{
    gc_header *hdr = (gc_header *)ptr - 1; // convert pointer to header
    printf("pointer:   %p\n", ptr);
    printf("gc_header: %p\n", hdr);
    printf("  size: %u\n", hdr->size);
    printf("  busy: %u\n", hdr->busy);
    printf("  mark: %u\n", hdr->mark);
    printf("  atom: %u\n", hdr->atom);
}
void print_gc_context(const gc_context *ctx)
{
    printf("gc_context: %p\n", ctx);
    printf("  roots: %u\n", ctx->nroots);
    printf("  memory: %p\n", ctx->memory);
    printf("  memend: %p\n", ctx->memend);
    printf("  memnext: %p\n", ctx->memnext);
    for (unsigned i = 0; i < ctx->nroots; ++i) {
        printf("  root[%u]: %p\n", i, ctx->roots[i]);
    }
}
/* MEMO
 * Separate the context for each process.
 * It divides the memory into equal blocks for each process.
 * Each block has its own roots and memory management.
 * TOTAL_MEMORY_SIZE == nprocesses * block_size
 * | ------------------- gc_ctx -------------------- |
 * | gc_ctx1 | gc_ctx2 |  ...  | gc_ctxN_1 | gc_ctxN |
 */

/*
 * WARNING: This is fixed nprocesses number.
 * It is not dynamic and should be set before calling this function.
*/
void gc_separateContext(const int nprocesses, const int nactiveprocesses)
{
    if (nprocesses < 1 || nprocesses > MAXCONTEXTS) {
        printf("gc_separateContext: invalid number of processes %d\n", nprocesses);
        return;
    }
    if (nactiveprocesses < 0 || nactiveprocesses > nprocesses) {
        printf("gc_separateContext: invalid number of active processes %d\n", nactiveprocesses);
        return;
    }
    // ctx should be initialized before this function is called
    gc_ctx.nroots = 0; // reset root count for the new context
    memset(gc_ctx.memory, 0, gc_ctx.memend - gc_ctx.memory); //clean memory 0
    unsigned int size = (char *)gc_ctx.memend - (char *)gc_ctx.memory;
    unsigned int block_size = size / nprocesses; // divide memory into equal blocks
    gc_header *block_start = (gc_header *)((char *)gc_ctx.memory + 1);/*hdr*/;
    // |hdr(1)|gc_context|memory|
    gc_debug_log("Whole memory size %ubytes, %d process, and %d bytes\n", size , nprocesses, block_size);
    gc_header *start = (gc_header *)((char *)gc_ctx.memory + 1); // start of the memory block
    for(int i = 0; i < nprocesses; i++) {
        // allocate a new context block
        gc_context *block = (gc_context *)block_start; // cast the start of the block to gc_context
        // initialize the block header
        gc_header *hdr = (gc_header *)block_start - 1; // get the header of the block
        hdr->size = block_size;
        hdr->busy = hdr->mark = hdr->atom = 0;
        // store the pointer to the new context in the roots array
        gc_ctx.nroots++;
        gc_ctx.roots[i] = (void **)block; // store the pointer to the new context in the roots array
        // memory for the new context
        block->memory = (gc_header *)((char *)block_start + sizeof(gc_context)); // set start of memory for this context
        if(i == nprocesses - 1) {
            block->memend = gc_ctx.memend; // last block goes to the end of memory
        } else {
            block->memend = (gc_header *)((char *)block_start + block_size - 1); // set end of memory for this context
        }
        // initialize the new context
        block->memnext = block->memory; // next block to consider when allocating
        block->nroots = 0; // reset root count for the new context
        // initialize the block header
        block->memory->size = block_size - sizeof(gc_context) - 1;// |hdr|gc_context|memory|
        block->memory->busy = block->memory->mark = block->memory->atom = 0; 
        assert(block->memory < block->memend); // ensure the memory is within bounds
        gc_debug_log("[%2d]: start pos%8ld bytes, end pos%8ld bytes, block size%8ld bytes\n", i,
            (char *)block - (char *)start,
            (char *)block->memend - (char *)start,
            (char *)block->memend - (char *)block + 1);
        assert((char *)block->memend - (char *)block + 1 >= block_size);
        block_start = (gc_header *)((char *)block + block_size);
    }
    nctx = nactiveprocesses; // set the number of active processes
    return ; // return to the main context
}

gc_context *gc_getContextSlot(void)
{
    if(nctx == gc_ctx.nroots) {
        printf("gc_getContextSlot: maximum number of contexts reached %d\n", nctx);
        return NULL; // no more contexts can be added
    }
    nctx++; // increment the number of contexts
    return (gc_context *)gc_ctx.roots[nctx++];
}

#if TESTGC
#include <stdio.h>
#include <assert.h>

typedef struct Link Link;

struct Link {
    int  data;
    Link *next;
};

struct Link *newLink(int data, Link *next)
{
    Link *link = gc_alloc(sizeof(*link));
    link->data = data;
    link->next = next;
    return link;
}

void markLink(Link *ptr)
{
    gc_mark(ptr->next);
}
//gcc -DTESTGC -o msgcs msgcs.c fatal.c
int main(){
    int context_size = sizeof(gc_context);
    int header_size = sizeof(gc_header);
    printf("block: |hdr(%d)| ctx(%d)|memory(S - (hdr+ctx))|\n", header_size, context_size);

    // gc_init(1024 * 1024); // initialize the garbage collector with 1 MB of memory
    // // gc_separateContext(4, 0); // separate memory for 4 processes

    // // gc_context *ctx1 = gc_getContextSlot(); // get a new context slot
    // // assert(ctx1 != NULL); // ensure we got a valid context


    // // gc_pushRoot(&ctx1); // push the context to the root stack
    // // printf("context slot %p allocated\n", ctx1);
    // // printf("context start address: %p\n", ctx1->memory);
    // // printf("context end address:   %p\n", ctx1->memend);
    // // printf("context next address:  %p\n", ctx1->memnext);
    // // printf("context size : %ld bytes\n\n", (char *)ctx1->memend - (char *)ctx1->memory);

    // // ctx = ctx1; // set the current context to the new context
    // // gc_debug_log("Requesting 10 bytes of atomic memory %lu\n", 10 * sizeof(int));

    // gc_markFunction = (gc_markFunction_t)markLink;

    // Link *list = 0;

    // gc_pushRoot(&list);

    // for (;;) {
    //     list = 0;
    //     for (int i = 0;  i < 10;  ++i) {
    //         list = newLink(i, list);
    //         static int cycle = 0;
    //         if ((random() & 1023) < ((cycle = cycle + 1) & 1023)) gc_collect();
    //     }
    //     for (Link *l = list;  l;  l = l->next)
    //         printf("%d ", l->data);
    //     printf("\n");
    // }
    // gc_popRoot(&list, "list"); // pop the list from the root stack

    // return 0;

    gc_init(1024 * 1024); // initialize the garbage collector with 1 MB of memory
    gc_separateContext(4, 0); // separate memory for 4 processes

    gc_context *ctx1 = gc_getContextSlot(); // get a new context slot
    assert(ctx1 != NULL); // ensure we got a valid context

    gc_pushRoot(&ctx1); // push the context to the root stack
    printf("context slot %p allocated\n", ctx1);
    printf("context start address: %p\n", ctx1->memory);
    printf("context end address:   %p\n", ctx1->memend);
    printf("context next address:  %p\n", ctx1->memnext);
    printf("context size : %ld bytes\n\n", (char *)ctx1->memend - (char *)ctx1->memory);

    ctx = ctx1; // set the current context to the new context
    gc_debug_log("Requesting 10 bytes of atomic memory %lu\n", 10 * sizeof(int));
    int *arr = (int *)gc_alloc_atomic(10 * sizeof(int)); // allocate an atomic array of 10 integers
    
    for (int i = 0; i < 10; i++) arr[i] = i; // initialize the array
    for (int i = 0; i < 10; i++) {
        printf("arr[%d] = %d\n", i, arr[i]); // print the array elements
    }
    print_gc_header((gc_header *)arr); // print the header of the allocated memory
    gc_pushRoot((void *)arr); // push the array to the root stack
    gc_mark(arr); // mark the array as reachable
    gc_collect(); // collect garbage
    for(int i = 0; i < 10; i++) {
        printf("arr[%d] = %d\n", i, arr[i]); // print the array elements after garbage collection
    }
    gc_popRoot((void *)arr, "arr"); // pop the array from the root stack
    gc_collect(); // collect garbage again
    ctx = &gc_ctx; // reset the current context to the global context
    GC_POP(ctx1); // pop the context from the root stack
    return 0;
}


#endif // TESTGC

#endif // MSGCS_C