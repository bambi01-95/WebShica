#ifndef ENTITY_H
#define ENTITY_H
#include "../Opcode/opcode.h"
#include "../Error/error.h"
#include "../GC/gc.h"
typedef union Entity Entity;
typedef Entity *ent;

typedef enum kind{
	Undeclar = 0,
	IntArray,
	IntQue3,
	Thread,
	EventHandler,
	Agent,
} kind_t;

struct IntArray{
	kind_t kind; // kind of the array
	int size, capacity;
	int *elements;
};

struct IntQue3{
	kind_t kind; // kind of the queue
	char tail, head,size,nArgs;
	int *que[3];
};
typedef int (*opfunc)(ent q);

struct Thread{
	kind_t kind;
	int inProgress; // 0 - not started, 1 - running, 2 - finished
	int apos;
	int cpos;
	int rbp; // base pointer
	int pc; // program counter
	ent queue;
	ent stack;
};

struct EventHandler{
	kind_t kind;
	int size;
	int EventH;
	int *data; // data for the event handler
	ent *threads; // thread that this handler belongs to
}; 

struct Agent{
	kind_t kind;
	int id;
	int isActive;
	int nEvents;
	int pc;
	int rbp;
	ent stack;
	ent *eventHandlers;
};

union Entity{
	kind_t kind;
	struct Agent Agent;
	struct EventHandler EventHandler;
	struct Thread Thread;
	struct IntArray IntArray;
	struct IntQue3 IntQue3;	
};

ent intArray_init(void);
ent newStack(const int initVal);
void printStack(ent stack);
void intArray_push(ent a, int value);
void intArray_append(ent a, int value);
int intArray_pop(ent a);
int intArray_last(ent a);

#define IntQue3Size 3

ent newQue3(int nArgs);
void enqueue3(ent eh, int *value);
ent dequeue3(ent thread);

struct EventTable{
	int (*eh)(ent eh); // event handler function
	int (*init)(ent eh); // initialize function
	int nArgs; // number of arguments for the event handler
	char nData; // number of data for the event handler
};

extern struct EventTable *EventTable;
void setEventTable(struct EventTable *tables);

ent newThread(int aPos, int cPos,int ehIndex);

ent newEventHandler(int ehIndex, int nThreads);

ent newAgent(int id, int nEvents);

extern ent *IrCodeList;
extern int nIrCode; // index of getIrCode
ent getIrCode(int index);

struct StdFuncTable{
	int (*stdfunc)(ent stack); // standard function
	int nArgs;
	//return value
};
extern struct StdFuncTable *StdFuncTable;
void setStdFuncTable(struct StdFuncTable *tables);

int printAgent(ent agent);

#endif // ENTITY_H