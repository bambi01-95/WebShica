#ifndef OBJECT_C
#define OBJECT_C

#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include "object.h"

#define TAGBITS 3
#define TAG_INT_ENT 0x1
#define TAG_FLT_ENT 0x2


static oop _newEntity(size_t size, kind_t kind)
{
	oop e = (oop)gc_alloc(size);
	if (!e) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	e->kind = kind;
	return e;
}
#define newEntity(TYPE) _newEntity(sizeof(struct TYPE), TYPE)

oop newIntVal(int value)
{
    return (oop)(((intptr_t)value << TAGBITS) | TAG_INT_ENT);
}

int IntVal_value(oop obj)
{
    return (intptr_t)obj >> TAGBITS;
}

oop newFloVal(double value)
{
    return (oop)(((intptr_t)value << TAGBITS) | TAG_FLT_ENT);
}

double FloVal_value(oop obj)
{
    union { intptr_t i;  double d; } u = { .i = (intptr_t)obj };
    return u.d;
}

oop newStrVal(const char *value)
{
	if(value == NULL){
		oop obj = newEntity(StrVal);
		obj->StrVal.value = NULL;
		return obj;
	}
	gc_pushRoot((void*)&value);
	oop obj = newEntity(StrVal);
	obj->StrVal.value = strdup(value);
	gc_popRoots(1);
	return obj;
}

char* StrVal_value(oop obj)
{
	if (obj->kind != StrVal) {
		fprintf(stderr, "expected StrVal got %d\n", obj->kind);
		exit(1);
	}
	return obj->StrVal.value;
}

oop intArray_init(void)
{
	GC_PUSH(oop, a, newEntity(IntArray));
	a->IntArray.elements = NULL;
	a->IntArray.elements = (int*)gc_beAtomic(gc_alloc(sizeof(int) * 4));
	a->IntArray.size     = 0;
	a->IntArray.capacity = 4;
	GC_POP(a);
	return a;
}

oop newStack(const int initVal)
{
	GC_PUSH(oop, a, newEntity(Stack));
	a->Stack.elements = NULL;
	a->Stack.elements = (oop*)gc_beAtomic(gc_alloc(sizeof(oop) * 10));
	a->Stack.elements[0] = newIntVal(initVal); // rbp 
	a->Stack.size     = 1;// rbp is always 0 at the start
	a->Stack.capacity = 10;
	GC_POP(a);
	return a;
}

oop pushStack(oop stack, oop value)
{
	assert(stack->kind == Stack);
	if (stack->Stack.size >= stack->Stack.capacity) {
		stack->Stack.capacity = stack->Stack.capacity ? stack->Stack.capacity * 2 : 4;
		gc_pushRoot((void*)stack);
		stack->Stack.elements = realloc(stack->Stack.elements, sizeof(oop) * stack->Stack.capacity);
		gc_popRoots(1);
	}
	stack->Stack.elements[stack->Stack.size++] = value;
	return stack;
}
oop popStack(oop stack)
{
	assert(stack->kind == Stack);
	if (stack->Stack.size == 0) fatal("pop: stack is empty");
	return stack->Stack.elements[--stack->Stack.size];
}
oop lastStack(oop stack)
{
	assert(stack->kind == Stack);
	if (stack->Stack.size == 0) fatal("last: stack is empty");
	return stack->Stack.elements[stack->Stack.size - 1];
}


void printStack(oop stack)
{
	printf("Stack: \n");
	for (int i = 0;  i < stack->IntArray.size;  ++i) {
		printf("%d %d\n", i, stack->IntArray.elements[i]);
	}
	printf("\n");
}
/* !!!! FOR VM !!!! */
void intArray_push(oop a, int value)
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
void intArray_append(oop a, int value)
{
	if (a->IntArray.size >= a->IntArray.capacity) {
		a->IntArray.capacity = a->IntArray.capacity ? a->IntArray.capacity * 2 : 4;
		a->IntArray.elements = realloc(a->IntArray.elements, sizeof(int) * a->IntArray.capacity);
	}
	a->IntArray.elements[a->IntArray.size++] = value;
}
int intArray_pop(oop a)
{
	if (a->IntArray.size == 0) fatal("pop: stack is empty");
	return a->IntArray.elements[--a->IntArray.size];
}
int intArray_last(oop a)
{
	if (a->IntArray.size == 0) fatal("last: stack is empty");
	return a->IntArray.elements[a->IntArray.size - 1];
}

// IntQue3

oop newQue3(int nArgs)
{
	GC_PUSH(oop, q, newEntity(IntQue3));
	q->IntQue3.nArgs = nArgs;
	q->IntQue3.tail = q->IntQue3.head = q->IntQue3.size = 0;
	for (int i = 0;  i < IntQue3Size;  ++i) {
		q->IntQue3.que[i] = (oop)gc_beAtomic(gc_alloc(sizeof(oop) * nArgs));
	}
	GC_POP(q);
	return q;
}

void enqueue3(oop eh, oop value)//value should be stack
{
	oop *threads = eh->EventHandler.threads;

	for (int i = 0; i < eh->EventHandler.size; ++i) {
		oop q = threads[i]->Thread.queue;
		if (q->IntQue3.size >= IntQue3Size) {
			fprintf(stderr, "enqueue3: queue is full\n");//need to fix this
			q->IntQue3.size = IntQue3Size; // reset size to max
			//exit(1);
		}
		q->IntQue3.que[q->IntQue3.tail] = value;
		q->IntQue3.tail = (q->IntQue3.tail + 1) % 3;
		q->IntQue3.size++;
	}
}

oop dequeue3(oop thread)
{
	oop q = thread->Thread.queue;
	if (q->IntQue3.size == 0) {
		return NULL; // queue is empty
	}
	oop stack = thread->Thread.stack =  newStack(0);
	oop x = q->IntQue3.que[q->IntQue3.head];
	pushStack(stack, x);

	q->IntQue3.head = (q->IntQue3.head + 1) % 3;
	q->IntQue3.size--;
	thread->Thread.inProgress = 1; // mark thread as in progress
	thread->Thread.rbp = 0;
	thread->Thread.pc = thread->Thread.apos; // reset program counter to the start position
	return stack;
}

oop newThread(int aPos, int cPos,int ehIndex)
{
	GC_PUSH(oop, thread, newEntity(Thread));  
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

oop newEventHandler(int ehIndex, int nThreads)
{
	GC_PUSH(oop, eh, newEntity(EventHandler));

	eh->EventHandler.size = nThreads;
	eh->EventHandler.EventH = ehIndex;
	eh->EventHandler.data = NULL;
	eh->EventHandler.threads = NULL;

	int nData = EventTable[ehIndex].nData;
	if(nData > 0) {
		eh->EventHandler.data = (oop*)gc_beAtomic(gc_alloc(sizeof(oop) * nData));
	} else {
		eh->EventHandler.data = NULL; // no data
	}
	eh->EventHandler.threads = (oop*)gc_beAtomic(gc_alloc(sizeof(oop) * nThreads));

	GC_POP(eh);
	return eh;
};

oop *IrCodeList = NULL;
int nIrCode = 0; // index of getIrCode
oop getIrCode(int index){
	if(index < nIrCode && index >= 0){
		return IrCodeList[index];
	} 
	if(index >= nIrCode){
		IrCodeList = realloc(IrCodeList, sizeof(oop) * (index + 1));	
		IrCodeList[index] = intArray_init();
		nIrCode = index + 1;
	}
	return IrCodeList[index];
}

oop newAgent(int id, int nEvents)
{
	oop agent = newEntity(Agent);
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
		agent->Agent.eventHandlers = (oop*)gc_beAtomic(gc_alloc(sizeof(oop) * nEvents));
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

struct EventObjectTable *EventObjectTable = NULL;
void setEventObjectTable(struct EventObjectTable *table)
{
	EventObjectTable = table;
}

int printAgent(oop agent)
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

#endif // OBJECT_C