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
    /*  1 */ Integer, Float, String, Symbol, Pair,Args, Eparams, Params, Array, Closure, StdFunc, UserFunc,
    /* 12 */ Binop, Unyop, GetVar, SetVar, GetArray, SetArray,
    /* 18 */ Call, Return, Break, Continue,
    /* 22 */ Print, If, Loop, Block,
	/* 24 */ Transition, State, Event,EventH,
	/* AF LEG */
	/* 25 */ Variable,EmitContext,
	/* LEG OBJ */
}type_t;

enum binop { ADD, SUB, MUL, DIV, MOD, LT, LE, GE, GT, EQ, NE, };
enum unyop { NEG, NOT, AINC, BINC, ADEC, BDEC,};

/* before leg */
struct Undefined { type_t _type; };
struct Integer 	 { type_t _type;           int _value;  };
struct Float   	 { type_t _type;           double _value; };
struct String  	 { type_t _type;           char *value; };
struct Symbol  	 { type_t _type;           char *name;  oop value; };
struct Pair  	 { type_t _type;           oop a, b; };
struct Args  	 { type_t _type;           oop value, next; };
struct Eparams   { type_t _type;           oop type, id, cond, next; };
struct Params     { type_t _type;           oop type, id; oop next; };
struct Tensor    { type_t _type;           int *shape;  int ndim;  oop *elements; };
struct Array  	 { type_t _type;           oop *elements;  int size, capacity; };
struct Closure 	 { type_t _type;           int nArgs; int pos,retType; int *argTypes; };//store user defined function
struct StdFunc   { type_t _type;           int index;  };
struct UserFunc	 { type_t _type;           oop parameters, body, code; };
struct Binop   	 { type_t _type;           enum binop op;  oop lhs, rhs; };
struct Unyop   	 { type_t _type;           enum unyop op;  oop rhs; };
struct GetVar  	 { type_t _type; int line; oop id; };
struct SetVar  	 { type_t _type; int line; oop type; oop id; oop rhs;int scope;  };
struct GetArray	 { type_t _type;           oop array, index; };
struct SetArray	 { type_t _type;           oop type, array, index, value;int scope; };
struct Call 	 { type_t _type; int line; oop function, arguments; };
struct Return 	 { type_t _type;           oop value; };
struct Break 	 { type_t _type;           };
struct Continue	 { type_t _type;           };
struct Transition{ type_t _type;           oop id; };
struct Print   	 { type_t _type;  int line; oop arguments; };
struct If      	 { type_t _type;           oop condition, statement1, statement2; };
struct Loop   	 { type_t _type;           oop initialization,condition,iteration, statement; };
struct Block   	 { type_t _type;           oop *statements;  int size; };
struct State   	 { type_t _type; int line; oop id, parameters, events; };
struct Event   	 { type_t _type; int line; oop id, parameters, block; };
struct EventH    { type_t _type;           int id; int nArgs; int *argTypes; };

/* after leg */
struct EmitContext{
    type_t _type;
	oop global_vars; // global variables
	oop state_vars; // state variables to emit code for
	oop local_vars; // local variables to emit code for
};
struct Variable{
    type_t _type;
    oop type; // type of variable (for future use)
    oop id; // symbol
};

union Object {
    enum   Type     _type;
	struct Undefined Undefined;
    struct Integer  Integer;
    struct Float    Float;
    struct String   String;
    struct Symbol   Symbol;
    struct Pair     Pair;
    struct Args     Args;
    struct Params   Params;
    struct Eparams  Eparams;
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
    struct Variable Variable;
    struct EmitContext EmitContext; // for emit code
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
oop newFloat(char* value);
double Float_value(oop obj);

/* String */
oop newString(char *value);
#define String_value(obj) (get(obj, String, value))

/* Symbol: not string */
oop newSymbol(char *name);
oop intern(char *name);
void initSymbols();
void collectSymbols();

oop newPair(oop a, oop b);
oop newArgs(oop value, oop next);
oop newEparams(oop type, oop id, oop cond, oop next);
oop newParams(oop type, oop id, oop next);

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

typedef enum {
  SCOPE_GLOBAL,
  SCOPE_STATE_LOCAL,
  SCOPE_LOCAL,
  SCOPE_UPVALUE,
  SCOPE_CONST
} ScopeClass;

oop newGetVar(oop id,int line);
oop newSetVar(oop type, oop id, oop rhs,ScopeClass scope, int line);

oop newGetArray(oop array, oop index);
oop newSetArray(oop type, oop array, oop index, oop value, ScopeClass scope);

oop newCall(oop arguments, oop function, int line);

oop newReturn(oop value);
oop newBreak(void);
oop newContinue(void);

oop newTransition(oop id);

oop newPrint(oop arguments,int line);
oop newIf(oop condition, oop s1, oop s2);
oop newLoop(oop init,oop cond,oop iterate, oop s);

oop newBlock(void);
void Block_append(oop b, oop s);
void Event_Block_append(oop b, oop e);

oop newState(oop id, oop parameters, oop events, int line);

oop newEvent(oop id, oop parameters, oop block,int line);

oop newEventH(int id, int nArgs);

oop newVariable(oop type, oop id);

struct RetVarFunc{
	ScopeClass scope; // G: global, S: state, L: local
	int index; // index of the variable
};
struct RetVarFunc insertVariable(oop ctx, oop sym, oop type);
struct RetVarFunc appendVariable(oop ctx, oop sym, oop type);
struct RetVarFunc searchVariable(oop ctx, oop sym, oop type);
#define discardVariables(V,X) V->Array.size = X

oop newEmitContext();

void printlnObject(oop node, int indent);
void println(oop obj);
void print(oop node);

enum {
  TAG_PTR_OOP  = 0b00,   // 普通のポインタ（下位2bit=00）
  TAG_INT_OOP  = 0b01,   // 小さい整数（fixnum）
  TAG_FLT_OOP  = 0b10,   // 浮動小数点数（fixnum）
  TAG_FLAG_OOP = 0b11,   // 列挙フラグ等
};



#define MAKE_OOP_FLAG(f) ((oop)(((intptr_t)(f) << TAGBITS) | TAG_FLAG_OOP))
#define MAKE_OOP_FLAG_INT(f) (((intptr_t)(f) << TAGBITS) | TAG_INT_OOP)
#define GET_OOP_FLAG(o) ((int)(((intptr_t)(o)) >> TAGBITS))
#define TAGBITS 2			// how many bits to use for tag bits
#define TAGMASK ((1 << TAGBITS) - 1)	// mask to extract just the tag bits
#define ISTAG_FLAG(o) ((((intptr_t)(o)) & TAGMASK) == TAG_FLAG_OOP) 
#define ISTAG_INT(o)  ((((intptr_t)(o)) & TAGMASK) == TAG_INT_OOP)
#define ISTAG_PTR(o)  ((((intptr_t)(o)) & TAGMASK) == TAG_PTR_OOP)

char* appendNewChar(char* arr, int size, char value);
int* appendNewInt(int* arr, int size, int value);

oop TYPES[4]; // 0: Undefined, 1: Integer, 2: Float, 3: String
#endif // OBJECT_H

/*
make:
void symbol_collect();
*/