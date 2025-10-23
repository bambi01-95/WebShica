#ifndef OBJECT_H
#define OBJECT_H
#include "../Opcode/opcode.h"
#include "../Error/error.h"
#include "../GC/gc.h"
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
	IntArray,
	Stack,
	IntQue3,
	Thread,
	EventHandler,
	Agent,
	Instance,
	Any,
} kind_t;

kind_t getKind(oop o);
oop _checkObject(oop obj, kind_t type, char *file, int line);
#define getObj(PTR, KIND, FIELD)	(_checkObject((PTR), KIND, __FILE__, __LINE__)->KIND.FIELD)


struct Instance{
	kind_t kind;
	oop *fields;
};

struct Any{
	kind_t kind;
	int8_t markbit;
	int8_t nData;
	void **data;
};

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
	struct Instance Instance;
	struct Any Any;
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
	char *argTypes; // types of arguments for the event handler
	char nData; // number of data for the event handler
};

extern struct EventTable *EventTable;
void setEventTable(struct EventTable *tables);

oop newThread(int aPos, int cPos,int ehIndex);

oop newEventHandler(int ehIndex, int nThreads);

oop newAgent(int id, int nEvents);

typedef oop (*eo_func_t)(oop stack);
extern eo_func_t *EventObjectFuncTable;
void setEventObjectFuncTable(eo_func_t *tables);

oop newInstance(int nFeilds);
#define getInstanceField(obj, index) (((obj)->Instance.fields)[index])

oop newAny(int markbit, int nData);
#define getAnyData(T, obj, index) ((T)((obj)->Any.data[index]))

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


// Event Object Table
struct EventObjectTable{
	char nArgs; // number of arguments
	char nFuncs; // number of functions
	int *argTypes; // types of arguments
};
extern struct EventObjectTable *EventObjectTable;
void setEventObjectTable(struct EventObjectTable *tables);

int printAgent(oop agent);
void printObj(oop obj, int indent);

#endif // OBJECT_H