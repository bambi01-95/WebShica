#ifndef ENTITY_H
#define ENTITY_H
#include "../Opcode/opcode.h"
#include "../Error/error.h"
#include "../GC/gc.h"
typedef union Object Object;
typedef Object *oop;

typedef enum kind{
	Undeclar = 0,
	IntVal,
	FloVal,
	StrVal,
	IntArray,
	Stack,
	IntQue3,
	Thread,
	EventHandler,
	Agent,
} kind_t;

struct IntVal{
	kind_t kind; // kind of the integer
	int value;
};
struct FloVal{
	kind_t kind; // kind of the float
	double value;
};
struct StrVal{
	kind_t kind; // kind of the string
	char *value;
};

struct IntArray{
	kind_t kind; // kind of the array
	int size, capacity;
	int *elements;
};

struct Stack{
	kind_t kind;
	int size, capacity;
	oop *elements;
};

struct IntQue3{
	kind_t kind; // kind of the queue
	char tail, head,size,nArgs;
	oop que[3];
};
typedef int (*opfunc)(oop q);

struct Thread{
	kind_t kind;
	int inProgress; // 0 - not started, 1 - running, 2 - finished
	int apos;
	int cpos;
	int rbp; // base pointer
	int pc; // program counter
	oop queue;
	oop stack;
};

struct EventHandler{
	kind_t kind;
	int size;
	int EventH;
	oop *data; // event handler data (stack)
	oop *threads; // thread that this handler belongs to
}; 

struct Agent{
	kind_t kind;
	int id;
	int isActive;
	int nEvents;
	int pc;
	int rbp;
	oop stack;
	oop *eventHandlers;
};

union Object{
	kind_t kind;
	struct Agent Agent;
	struct EventHandler EventHandler;
	struct Thread Thread;
	struct IntArray IntArray;
	struct Stack Stack;
	struct IntQue3 IntQue3;	
	struct IntVal IntVal;
	struct FloVal FloVal;
	struct StrVal StrVal;
};

oop newIntVal(int value);
int IntVal_value(oop obj);



oop newFloVal(double value);
double FloVal_value(oop obj);
oop newStrVal(const char *value);
char* StrVal_value(oop obj);

oop intArray_init(void);
oop newStack(const int initVal);
oop pushStack(oop stack, oop value);
oop popStack(oop stack);
oop lastStack(oop stack);

void printStack(oop stack);
void intArray_push(oop a, int value);
void intArray_append(oop a, int value);
int intArray_pop(oop a);
int intArray_last(oop a);

#define IntQue3Size 3

oop newQue3(int nArgs);
void enqueue3(oop eh, oop value);
oop dequeue3(oop thread);

struct EventTable{
	int (*eh)(oop eh); // event handler function
	int (*init)(oop eh); // initialize function
	int nArgs; // number of arguments for the event handler
	char nData; // number of data for the event handler
};

extern struct EventTable *EventTable;
void setEventTable(struct EventTable *tables);

oop newThread(int aPos, int cPos,int ehIndex);

oop newEventHandler(int ehIndex, int nThreads);

oop newAgent(int id, int nEvents);

extern oop *IrCodeList;
extern int nIrCode; // index of getIrCode
oop getIrCode(int index);

struct StdFuncTable{
	int (*stdfunc)(oop stack); // standard function
	int nArgs;
	int *argTypes;
	int retType;
	//return value
};
extern struct StdFuncTable *StdFuncTable;
void setStdFuncTable(struct StdFuncTable *tables);

int printAgent(oop agent);

#endif // ENTITY_H