#ifndef OBJECT_H
#define OBJECT_H
#include "opcode.h"
#include "error.h"
#include "gc.h"
#include <stdint.h>
/*
int32_t: 4 bytes -> int: 4 bytes
int64_t: 8 bytes -> long long: 8 bytes
*/
typedef union Object Object;
typedef Object *oop;

typedef enum kind{
	Undeclar = 0,
	IntVal,
	FloVal,
	StrVal,
	ArrVal,
	IntArray=5,
	Stack,
	Queue,
	Thread,
	EventHandler,
	Agent,
	Instance,
	Any,
	RunCtx,
	RETFLAG,
	EXTRA_KIND, // for future extension
} kind_t;

kind_t getKind(oop o);
oop _checkObject(oop obj, kind_t type, char *file, int line);
#define getObj(PTR, KIND, FIELD)	(_checkObject((PTR), KIND, __FILE__, __LINE__)->KIND.FIELD)




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
struct ArrVal{
	kind_t kind; // kind of the array
	int size, capacity;
	oop *elements;
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

struct Queue{
	kind_t kind; // kind of the queue
	char tail, head,size,nArgs,capacity;
	oop *que;
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
	int size;//number of threads
	int nData;//number of data
	int EventH;//index of event handler
	oop *data; // event handler data (stack / instance)
	oop *threads; // thread that this handler belongs to
}; 

struct Agent{
	kind_t kind;
	int id;
	int isActive;
	int nEvents;//FIXME: rename nEH
	int pc;
	int rbp;
	int base;// base index of state variables in the stack
	oop stack;
	oop *eventHandlers;
};

struct Instance{
	kind_t kind;
	char nFields;
	oop *fields;
};

struct Any{
	kind_t kind;
	int8_t markbit;
	int8_t nData;
	void **data;
};

struct RunCtx{
	kind_t kind;
	oop agent;
	oop code;
};

struct RETFLAG{
	kind_t kind;
};

union Object{
	kind_t kind;
	struct Agent Agent;
	struct EventHandler EventHandler;
	struct Thread Thread;
	struct IntArray IntArray;
	struct Stack Stack;
	struct Queue Queue;	
	struct IntVal IntVal;
	struct FloVal FloVal;
	struct StrVal StrVal;
	struct ArrVal ArrVal;
	struct Instance Instance;
	struct Any Any;
	struct RunCtx RunCtx;
	struct RETFLAG RETFLAG;
};

oop newIntVal(int value);
int IntVal_value(oop obj);

oop newFloVal(double value);
double FloVal_value(oop obj);

oop newStrVal(const char *value);
char* StrVal_value(oop obj);

oop newArrVal(int size);

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

#define QueueSize 4

oop newQueue(int nArgs);
void enqueue(oop exec, oop eh, oop value);
oop dequeue(oop thread);



oop newThread(int aPos, int cPos,int ehIndex);

oop newEventHandler(int ehIndex, int nThreads);

oop newAgent(int id, int nEvents);



oop newInstance(int nFeilds);
#define getInstanceField(obj, index) (((obj)->Instance.fields)[index])

oop newAny(int markbit, int nData);
#define getAnyData(T, obj, index) ((T)((obj)->Any.data[index]))

oop newRunCtx(oop agent, oop code);

oop newRETFLAG(void);

extern oop *IrCodeList;
extern int nIrCode; // index of getIrCode
oop getIrCode(int index);

int printAgent(oop agent);
void printObj(oop obj, int indent);
//----------------------------------------
// Library API
//----------------------------------------
typedef oop (*eo_func_t)(oop stack);

struct ExecEventTable {
	int (*eh)(oop exec, oop eh); // event handler function
	int (*init)(oop eh); // initialize function
	int nArgs; // number of arguments
	int nData; // number of data
};
struct ExecStdFuncTable {
	int (*func)(oop stack); //function pointer
};
struct ExecEventObjectTable {
	oop (*eo)(oop stack); // event object function
};

extern struct ExecEventTable  *ExecEventTable;
extern struct ExecStdFuncTable *ExecStdFuncTable;
extern struct ExecEventObjectTable  *ExecEventObjectTable;
void reinitializeEventObject(oop eh);
//----------------------------------------
// GC
//----------------------------------------
void markExecutors(oop ptr);
#endif // OBJECT_H