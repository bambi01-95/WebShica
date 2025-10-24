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

kind_t getKind(oop o)
{
    if ((((intptr_t)o) & TAGBITS) == TAG_INT_ENT) return IntVal;
    if ((((intptr_t)o) & TAGBITS) == TAG_FLT_ENT) return FloVal;
    return o->kind;
}

#define newEntity(TYPE) _newEntity(sizeof(struct TYPE), TYPE)

oop _checkObject(oop obj, kind_t kind, char *file, int line){
    if (getKind(obj) != kind) {
		fprintf(stderr, "\n%s:%d: expected kind %d got kind %d\n", file, line, kind, getKind(obj));
		exit(1);
    }
    return obj;
}


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

//FIXME
oop dupStack(oop stack)
{
	return stack;
}

void enqueue3(const oop eh,const oop newStack)//value should be stack
{
	oop *threads = eh->EventHandler.threads;


	for (int i = 0; i < eh->EventHandler.size; ++i) {
		oop q = threads[i]->Thread.queue;
		if (q->IntQue3.size >= IntQue3Size) {
			fprintf(stderr, "enqueue3: queue is full\n");//need to fix this
			q->IntQue3.size = IntQue3Size; // reset size to max
			//exit(1);
		}
		q->IntQue3.que[q->IntQue3.tail] = i==0 ? newStack : dupStack(newStack);
		q->IntQue3.tail = (q->IntQue3.tail + 1) % IntQue3Size;
		q->IntQue3.size++;
	}
}

oop dequeue3(oop thread)
{
	oop q = thread->Thread.queue;
	if (q->IntQue3.size == 0) {
		return NULL; // queue is empty
	}
	oop stack = thread->Thread.stack =  q->IntQue3.que[q->IntQue3.head];

	q->IntQue3.head = (q->IntQue3.head + 1) % IntQue3Size;
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

oop newInstance(int nFeilds)
{
	GC_PUSH(oop, instance, newEntity(Instance));
	instance->Instance.nFields = nFeilds;
	instance->Instance.fields = (oop*)gc_beAtomic(gc_alloc(sizeof(oop) * nFeilds));
	for(int i=0; i<nFeilds; i++){
		instance->Instance.fields[i] = NULL;
	}
	GC_POP(instance);
	return instance;
}

oop newAny(int markbit, int nData)
{
	GC_PUSH(oop, any, newEntity(Any));
	any->Any.markbit = markbit;
	any->Any.nData = nData;
	if(nData > 0){
		any->Any.data = (void**)gc_beAtomic(gc_alloc(sizeof(void*) * nData));
	}else{
		any->Any.data = NULL;
	}
	GC_POP(any);
	return any;
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

eo_func_t *EventObjectFuncTable = NULL;
void setEventObjectFuncTable(eo_func_t *tables)
{
	EventObjectFuncTable = tables;
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


void printObj(oop obj, int indent)
{
	printf("%*s", indent*2, "");
	switch(getKind(obj))
	{
	case Undeclar:{
		printf("Undeclar\n");
		break;
	}
	case IntVal:{
		printf("%d\n", IntVal_value(obj));
		break;
	}
	case FloVal:{
		printf("%f\n", FloVal_value(obj));
		break;
	}
	case StrVal:{
		printf("%s\n", getObj(obj, StrVal, value));
		break;
	}
	case IntArray:{
		int size = getObj(obj, IntArray, size);
		int *ele = getObj(obj, IntArray, elements);
		for(int i = 0; i<size; i++)printf("%*s%d", indent*2, "", ele[i]);
		break;
	}
	case Stack:{
		printf("Stack\n");
		int size = getObj(obj, Stack, size);
		oop *ele = getObj(obj, Stack, elements);
		for(int i = 0; i< size; i++)printObj(ele[i],indent + 1);
		break;
	}
	case IntQue3:{
		printf("IntQue3\n");
		int pos = obj->IntQue3.head;
		for(int i = 0; i < obj->IntQue3.size; i++){
			if(obj->IntQue3.que[pos] != NULL){
				printObj(obj->IntQue3.que[pos], indent + 1); 
			}
			pos = (pos + 1) % IntQue3Size;
		}
		break;
	}
	case Thread:{
		printf("Thread\n");
		printObj(getObj(obj, Thread, stack), indent + 1);
		printObj(getObj(obj, Thread, queue), indent + 1);
		break;
	}
	case EventHandler:{
		printf("EventHandler (data doesnot output)\n");
		int size = getObj(obj, EventHandler, size);
		oop *threads = getObj(obj, EventHandler, threads);
		for(int i = 0; i< size; i++)printObj(threads[i], indent + 1);
		break;
	}
	case Agent:{
		printf("Agent\n");
		printObj(getObj(obj,Agent,stack), indent + 1);
		int nEH = getObj(obj, Agent, nEvents);
		oop* ehs = getObj(obj, Agent, eventHandlers);
		for(int i = 0; i<nEH; i++)printObj(ehs[i], indent + 1);
		break;
	}
	case Instance:{
		printf("Instance (no output)\n");
		break;
	}
	case Any:{
		printf("Any (not supported now)\n");
		break;
	}
	}
	return;
}
#endif // OBJECT_C