#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdint.h>
#include "../GC/gc.h"
#include "../Error/error.h"

union Object;
typedef union Object Object;
typedef Object *oop; // ordinary object pointer

extern oop nil;
extern oop false;
extern oop true;
extern oop entryEH;
extern oop exitEH;

typedef enum Type {
    /*  0 */ Undefined = 0,
    /*  1 */ Integer, Float, Symbol, Pair, Array, Closure, StdFunc, UserFunc,
    /*  8 */ Binop, Unyop, GetVar, SetVar, GetArray, SetArray,
    /* 14 */ Call, Return, Break, Continue,
    /* 18 */ Print, If, Loop, Block,
	/* 21 */ Transition, State, Event,EventH,
	/* AF LEG */
	/* 25 */ Variables,
	/* LEG OBJ */
}type_t;

enum binop { ADD, SUB, MUL, DIV, MOD, LT, LE, GE, GT, EQ, NE, };
enum unyop { NEG, NOT, AINC, BINC, ADEC, BDEC,};

/* before leg */
struct Undefined { type_t _type; };
struct Integer 	 { type_t _type;           int _value;  };
struct Symbol  	 { type_t _type;           char *name;  oop value; };
struct Pair  	 { type_t _type;           oop a, b; };
struct Tensor    { type_t _type;           int *shape;  int ndim;  oop *elements; };
struct Array  	 { type_t _type;           oop *elements;  int size, capacity; };
struct Closure 	 { type_t _type;           int nArgs; int pos; };
struct StdFunc   { type_t _type;           int index; };
struct UserFunc	 { type_t _type;           oop parameters, body, code; };
struct Binop   	 { type_t _type;           enum binop op;  oop lhs, rhs; };
struct Unyop   	 { type_t _type;           enum unyop op;  oop rhs; };
struct GetVar  	 { type_t _type; int line; oop id; };
struct SetVar  	 { type_t _type; int line; oop id; oop rhs;int scope;  };
struct GetArray	 { type_t _type;           oop array, index; };
struct SetArray	 { type_t _type;           oop array, index, value; };
struct Call 	 { type_t _type; int line; oop function, arguments; };
struct Return 	 { type_t _type;           oop value; };
struct Break 	 { type_t _type;           };
struct Continue	 { type_t _type;           };
struct Transition{ type_t _type;           oop id; };
struct Print   	 { type_t _type;           oop arguments; };
struct If      	 { type_t _type;           oop condition, statement1, statement2; };
struct Loop   	 { type_t _type;           oop initialization,condition,iteration, statement; };
struct Block   	 { type_t _type;           oop *statements;  int size; };
struct State   	 { type_t _type; int line; oop id, parameters, events; };
struct Event   	 { type_t _type; int line; oop id, parameters, block; };
struct EventH    { type_t _type;           int id; int nArgs; };

/* after leg */
struct Variables{
	type_t _type;
	int size;
	int capacity;
	oop *elements;
};

union Object {
    enum   Type     _type;
	struct Undefined Undefined;
    struct Integer  Integer;
    struct Symbol   Symbol;
    struct Pair     Pair;
    struct Array    Array;
    struct Closure  Closure;
	struct StdFunc  StdFunc;
    struct UserFunc UserFunc;
    struct Binop    Binop;
    struct Unyop    Unyop;
    struct GetVar   GetVar;
    struct SetVar   SetVar;
    struct GetArray GetArray;
    struct SetArray SetArray;
    struct Call     Call;
    struct Return   Return;
    struct Break    Break;
    struct Continue Continue;
	struct Transition Transition;
    struct Print    Print;
    struct If       If;
    struct Loop    Loop;
    struct Block    Block;
	struct State    State;
	struct Event    Event;
	struct EventH   EventH;

	/* AF LEG */
	struct Variables Variables;
};

int getType(oop o);

oop _check(oop node, enum Type type, char *file, int line);
#define get(PTR, TYPE, FIELD)	(_check((PTR), TYPE, __FILE__, __LINE__)->TYPE.FIELD)

extern int nobj;
// It should not be called in outside, only from inside
// oop _newObject(size_t size, enum Type type);

oop newUndefine();


/* Integer */
oop newInteger(int value);
int Integer_value(oop obj);

/* Float */
oop newFloat(double value);
double Float_value(oop obj);

/* Symbol: not string */
oop newSymbol(char *name);
oop intern(char *name);
void initSymbols();
void collectSymbols();

oop newPair(oop a, oop b);

oop newArray(int size);
oop Array_grow(oop array);
oop Array_append(oop array, oop element);
oop Array_last(oop array);
oop Array_pop(oop array);

oop newClosure();
oop newStdFunc(int index);
oop newUserFunc(oop parameters, oop body);

oop newBinop(enum binop op, oop lhs, oop rhs);
oop newUnyop(enum unyop op, oop rhs);

oop newGetVar(oop id,int line);
oop newSetVar(oop id, oop rhs,int scope, int line);

oop newGetArray(oop array, oop index);
oop newSetArray(oop array, oop index, oop value);

oop newCall(oop arguments, oop function, int line);

oop newReturn(oop value);
oop newBreak(void);
oop newContinue(void);

oop newTransition(oop id);

oop newPrint(oop arguments);
oop newIf(oop condition, oop s1, oop s2);
oop newLoop(oop init,oop cond,oop iterate, oop s);

oop newBlock(void);
void Block_append(oop b, oop s);
void Event_Block_append(oop b, oop e);

oop newState(oop id, oop parameters, oop events, int line);

oop newEvent(oop id, oop parameters, oop block,int line);

oop newEventH(int id, int nArgs);

oop newVariables();
oop insertVariable(oop list, oop sym);
oop searchVariable(oop list, oop sym);
#define discardVarialbes(V,X) V->Variables.size = X

void printlnObject(oop node, int indent);
void println(oop obj);
void print(oop node);

#endif // OBJECT_H

/*
make:
void symbol_collect();
*/