#ifndef ENTITY_C
#define ENTITY_C

#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include "entity.h"




static ent _newEntity(size_t size, kind_t kind)
{
	ent e = (ent)gc_alloc(size);
	if (!e) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	e->kind = kind;
	return e;
}
#define newEntity(TYPE) _newEntity(sizeof(struct TYPE), TYPE)

ent intArray_init(void)
{
	GC_PUSH(ent, a, newEntity(IntArray));
	a->IntArray.elements = NULL;
	a->IntArray.elements = (int*)gc_beAtomic(gc_alloc(sizeof(int) * 4));
	a->IntArray.size     = 0;
	a->IntArray.capacity = 4;
	GC_POP(a);
	return a;
}

ent newStack(const int initVal)
{
	GC_PUSH(ent, a, newEntity(IntArray));
	a->IntArray.elements = NULL;
	a->IntArray.elements = (int*)gc_beAtomic(gc_alloc(sizeof(int) * 10));
	a->IntArray.elements[0] = initVal; // rbp
	a->IntArray.size     = 1;// rbp is always 0 at the start
	a->IntArray.capacity = 10;
	GC_POP(a);
	return a;
}
void printStack(ent stack)
{
	printf("Stack: \n");
	for (int i = 0;  i < stack->IntArray.size;  ++i) {
		printf("%d %d\n", i, stack->IntArray.elements[i]);
	}
	printf("\n");
}
/* !!!! FOR VM !!!! */
void intArray_push(ent a, int value)
{

	if (a->IntArray.size >= a->IntArray.capacity) {
		a->IntArray.capacity = a->IntArray.capacity ? a->IntArray.capacity + 10000 : 10000;
		// printf("intArray_push: size %d >= capacity %d\n", a->size, a->capacity);
		// printf("              %p\n",a);
		gc_pushRoot((void*)a);
		a->IntArray.elements = realloc(a->IntArray.elements, sizeof(int) * a->IntArray.capacity);
		gc_popRoots(1);
	}
	a->IntArray.elements[a->IntArray.size++] = value;
}

/* !!!! FOR COMPILE !!!! */
void intArray_append(ent a, int value)
{
	if (a->IntArray.size >= a->IntArray.capacity) {
		a->IntArray.capacity = a->IntArray.capacity ? a->IntArray.capacity * 2 : 4;
		a->IntArray.elements = realloc(a->IntArray.elements, sizeof(int) * a->IntArray.capacity);
	}
	a->IntArray.elements[a->IntArray.size++] = value;
}
int intArray_pop(ent a)
{
	if (a->IntArray.size == 0) fatal("pop: stack is empty");
	return a->IntArray.elements[--a->IntArray.size];
}
int intArray_last(ent a)
{
	if (a->IntArray.size == 0) fatal("last: stack is empty");
	return a->IntArray.elements[a->IntArray.size - 1];
}

// IntQue3

ent newQue3(int nArgs)
{
	GC_PUSH(ent, q, newEntity(IntQue3));
	q->IntQue3.nArgs = nArgs;
	q->IntQue3.tail = q->IntQue3.head = q->IntQue3.size = 0;
	for (int i = 0;  i < IntQue3Size;  ++i) {
		q->IntQue3.que[i] = (int*)gc_beAtomic(gc_alloc(sizeof(int) * nArgs));
	}
	GC_POP(q);
	return q;
}

void enqueue3(ent eh, int *value)
{
	ent *threads = eh->EventHandler.threads;

	for (int i = 0; i < eh->EventHandler.size; ++i) {
		ent q = threads[i]->Thread.queue;
		if (q->IntQue3.size >= IntQue3Size) {
			fprintf(stderr, "enqueue3: queue is full\n");//need to fix this
			q->IntQue3.size = IntQue3Size; // reset size to max
			//exit(1);
		}
		for (int j = 0; j < q->IntQue3.nArgs; ++j) {
			q->IntQue3.que[q->IntQue3.tail][j] = value[j];
		}
		q->IntQue3.tail = (q->IntQue3.tail + 1) % 3;
		q->IntQue3.size++;
	}
}

ent dequeue3(ent thread)
{
	ent q = thread->Thread.queue;
	if (q->IntQue3.size == 0) {
		return NULL; // queue is empty
	}
	ent stack = thread->Thread.stack =  newStack(0);
	for(int i = 0; i < q->IntQue3.nArgs; ++i){
		int x = q->IntQue3.que[q->IntQue3.head][i];
		intArray_push(stack, x);
	}

	q->IntQue3.head = (q->IntQue3.head + 1) % 3;
	q->IntQue3.size--;
	thread->Thread.inProgress = 1; // mark thread as in progress
	thread->Thread.rbp = 0;
	thread->Thread.pc = thread->Thread.apos; // reset program counter to the start position
	return stack;
}

ent newThread(int aPos, int cPos,int ehIndex)
{
	GC_PUSH(ent, thread, newEntity(Thread));  
	thread->Thread.inProgress = 0; // not started
	thread->Thread.apos = aPos;
	thread->Thread.cpos = cPos;
	thread->Thread.queue = NULL;
	thread->Thread.stack = NULL;
	thread->Thread.rbp = 0; // base pointer
	thread->Thread.pc = 0; // program counter
	thread->Thread.queue = newQue3(EventTable[ehIndex].nArgs);
	GC_POP(thread);
	return thread;
}

ent newEventHandler(int ehIndex, int nThreads)
{
	GC_PUSH(ent, eh, newEntity(EventHandler));

	eh->EventHandler.size = nThreads;
	eh->EventHandler.EventH = ehIndex;
	eh->EventHandler.data = NULL;
	eh->EventHandler.threads = NULL;

	int nData = EventTable[ehIndex].nData;
	printf("%s %d: pass nData %d\n",__FILE__, __LINE__, nData);
	if(nData > 0) {
		eh->EventHandler.data = (int*)gc_beAtomic(gc_alloc(sizeof(int) * nData));
		for(int i = 0; i < nData; ++i){
			eh->EventHandler.data[i] = 0; // initialize data to 0
		}
	} else {
		eh->EventHandler.data = NULL; // no data
	}
	eh->EventHandler.threads = (ent*)gc_beAtomic(gc_alloc(sizeof(ent) * nThreads));

	GC_POP(eh);
	return eh;
};

ent *IrCodeList = NULL;
int nIrCode = 0; // index of getIrCode
ent getIrCode(int index){
	if(index < nIrCode && index >= 0){
		return IrCodeList[index];
	} 
	if(index >= nIrCode){
		IrCodeList = realloc(IrCodeList, sizeof(ent) * (index + 1));	
		IrCodeList[index] = intArray_init();
		nIrCode = index + 1;
	}
	return IrCodeList[index];
}

ent newAgent(int id, int nEvents)
{
	ent agent = newEntity(Agent);
	agent->Agent.id = id;
	agent->Agent.isActive = 0; // active by default
	agent->Agent.nEvents = nEvents;
	agent->Agent.pc = 0;
	agent->Agent.rbp = 0;
	agent->Agent.stack = newStack(0);
	if(nEvents <= 0) {
		agent->Agent.eventHandlers = NULL; // no event handlers
		agent->Agent.nEvents = 0; // no events
	}else{
		agent->Agent.eventHandlers = (ent*)gc_beAtomic(gc_alloc(sizeof(ent) * nEvents));
		for(int i=0; i<nEvents; i++){
			agent->Agent.eventHandlers[i] = NULL;
		}
	}
	return agent;
}



struct EventTable *EventTable = NULL;
void setEventTable(struct EventTable *table)
{
	EventTable = table;
}
struct StdFuncTable *StdFuncTable = NULL;
void setStdFuncTable(struct StdFuncTable *table)
{
	StdFuncTable = table;
}

int printAgent(ent agent)
{
	if(agent->kind != Agent) {
		printf("Error: Not an agent\n");
		return -1; // Error: not an agent
	}
	if(agent == NULL) {
		printf("Agent: NULL\n");
		return 0;
	}
	printf("Agent: id=%d, isActive=%d, nEvents=%d, pc=%d, rbp=%d\n",
		agent->Agent.id, agent->Agent.isActive, agent->Agent.nEvents,
		agent->Agent.pc, agent->Agent.rbp);
	return 1;
}

#endif // ENTITY_C