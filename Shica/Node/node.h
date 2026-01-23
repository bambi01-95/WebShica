#ifndef NODE_H
#define NODE_H

#include <stddef.h>
#include <stdint.h>
#include "GC/gc.h"
#include "Error/error.h"

union Node;
typedef union Node Node;
typedef Node *node;                 // ordinary object pointer 
typedef const node cnode;           // constant object pointer

extern node nil;
extern node FALSE;
extern node TRUE;
extern node entryEH;
extern node exitEH;

typedef enum Type {
    /*  0 */ Undefined = 0,
    /*  1 */ Integer, Float, String, EventObject, //Shica Atomic Type
    Intruction = 5,
    Symbol, Pair,Args, Eparams, Params, Array, Closure, StdFunc, UserFunc,
    /* 14 */ Binop, Unyop, GetVar, SetVar, GetArray, SetArray, GetField, SetType,
    /* 22 */ Call, Return, Break, Continue,
    /* 26 */ Print, If, Loop, Block,
	/* 30 */ Transition, State, Event,EventH,
	/* AF LEG */
	/* 34 */ Variable,EmitContext,
	/* LEG OBJ */
}type_t;

enum binop { ADD, SUB, MUL, DIV, MOD, LT, LE, GE, GT, EQ, NE, };
enum unyop { NEG, NOT, AINC, BINC, ADEC, BDEC,};

/* before leg */
struct Undefined { type_t _type; };
struct Intruction{ type_t _type;            int size, capacity; int *elements; };
struct Integer 	 { type_t _type;           int _value;  };
struct Float   	 { type_t _type;           double _value; };
struct String  	 { type_t _type;           char *value; };
struct EventObject{ type_t _type;           int index; node *funcs;};
struct Symbol  	 { type_t _type;           char *name;  node value; };
struct Pair  	 { type_t _type;           node a, b; };
struct Args  	 { type_t _type;           node value, next; };
struct Eparams   { type_t _type;           node type, id, cond, next; };
struct Params     { type_t _type;          node type, id; node next; };
struct Tensor    { type_t _type;           int *shape;  int ndim;  node *elements; };
struct Array  	 { type_t _type;           node *elements;  int size, capacity; };
struct Closure 	 { type_t _type;           int nArgs; int pos,retType; int *argTypes; };//store user defined function
struct StdFunc   { type_t _type;           int index;  };/*Table*/
struct UserFunc	 { type_t _type;           node parameters, body, code; };
struct Binop   	 { type_t _type;           enum binop op;  node lhs, rhs; };
struct Unyop   	 { type_t _type;           enum unyop op;  node rhs; };
struct GetVar  	 { type_t _type; int line; node id; };
struct SetVar  	 { type_t _type; int line; node type; node id; node rhs;int scope;  };
struct GetArray	 { type_t _type; int line; node array, index; };
struct SetArray	 { type_t _type; int line; node type, array, index, value;int scope; };
struct GetField  { type_t _type; int line; node id; node field; };
struct SetType   { type_t _type; int line; node id; node rhs;};
struct Call 	 { type_t _type; int line; node function, arguments; };
struct Return 	 { type_t _type;           node value; };
struct Break 	 { type_t _type;           };
struct Continue	 { type_t _type;           };
struct Transition{ type_t _type;           node id; };
struct Print   	 { type_t _type;  int line; node arguments; };
struct If      	 { type_t _type;           node condition, statement1, statement2; };
struct Loop   	 { type_t _type;           node initialization,condition,iteration, statement; };
struct Block   	 { type_t _type;           node *statements;  int size; };
struct State   	 { type_t _type; int line; node id, parameters, events; };
struct Event   	 { type_t _type; int line; node id, parameters, block; };
struct EventH    { type_t _type;           int index; };/*Table*/

/* after leg */
struct EmitContext{
    type_t _type;
	node global_vars; // global variables
	node state_vars; // state variables to emit code for
	node local_vars; // local variables to emit code for
    node user_types; // user defined types
    char isVoid; // 1: in void context, 0: not in void context
};
struct Variable{
    type_t _type;
    node type; // type of variable (for future use)
    node id; // symbol
    node value;// EventObject or Types
    unsigned char dim[3]; //maximum 3 dimension for array and 256 size each
};

union Node {
    enum   Type     _type;
	struct Undefined Undefined;
    struct Intruction Intruction;
    struct Integer  Integer;
    struct Float    Float;
    struct String   String;
    struct EventObject EventObject;
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
    struct GetField GetField;
    struct SetType  SetType;
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
// typedef struct RetNode{
//     node value;
//     int isSuccess; // 0: success, 1: error
//     int line;
// } RetNode;

int getType(node o);

node _check(node node, enum Type type, char *file, int line);
#define getNode(PTR, TYPE, FIELD)	(_check((PTR), TYPE, __FILE__, __LINE__)->TYPE.FIELD)

extern int nobj;
// It should not be called in outside, only from inside
// node _newObject(size_t size, enum Type type);

node newUndefine();

node newInstruction();
node Instruction_append(node instr, int value);
/* Integer */
node newInteger(int value);
int Integer_value(node obj);

/* Float */
node newFloat(char* value);
double Float_value(node obj);

/* String */
node newString(char *value);
#define String_value(obj) (getNode(obj, String, value))

node newEventObject(node sym, int index);
node putFuncToEo(node eo, node func, node symbol, int index);

/* Symbol: not string */
node newSymbol(char *name);
node intern(char *name);
void initSymbols();
void collectSymbols();

node newPair(node a, node b);
node newArgs(node value, node next);
node newEparams(node type, node id, node cond, node next);
node newParams(node type, node id, node next);

node newArray(int size);
node Array_grow(node array);
node Array_append(node array, node element);
node Array_last(node array);
node Array_pop(node array);

node newClosure();
node newStdFunc(int index);
node newUserFunc(node parameters, node body);

node newBinop(enum binop op, node lhs, node rhs);
node newUnyop(enum unyop op, node rhs);

typedef enum {
  SCOPE_GLOBAL,
  SCOPE_STATE_LOCAL,
  SCOPE_LOCAL,
  SCOPE_UPVALUE,
  SCOPE_CONST
} ScopeClass;

node newGetVar(node id,int line);
node newSetVar(node type, node id, node rhs,ScopeClass scope, int line);

node newGetArray(node array, node index, int line);
node newSetArray(node type, node array, node index, node value, ScopeClass scope, int line);

node newGetField(node id, node field, int line);

node newSetType(node id, node rhs, int line);

node newCall(node arguments, node function, int line);

node newReturn(node value);
node newBreak(void);
node newContinue(void);

node newTransition(node id);

node newPrint(node arguments,int line);
node newIf(node condition, node s1, node s2);
node newLoop(node init,node cond,node iterate, node s);

node newBlock(void);
void Block_append(node b, node s);
void Event_Block_append(node b, node e);

node newState(node id, node parameters, node events, int line);

node newEvent(node id, node parameters, node block,int line);

node newEventH(int index);

node newVariable(node type, node id);
node newVariableWithValue(node type, node id, node value);

struct RetVarFunc{
	ScopeClass scope; // G: global, S: state, L: local
	int index; // index of the variable
    node variable;// variable node
};
struct RetVarFunc insertVariable(node ctx, node sym, node type);
struct RetVarFunc appendVariable(node ctx, node sym, node type, node value);
struct RetVarFunc searchVariable(node ctx, node sym, node type);
#define discardVariables(V,X) V->Array.size = X

node newEmitContext();
#define getVoidContext() (vars->EmitContext.isVoid)
#define setVoidContext() (vars->EmitContext.isVoid = 1)
#define clearVoidContext() (vars->EmitContext.isVoid = 0)
int addUserType(node ctx, node ut); // ret: index of user type
// nested user type index stack
int pushUserTypeIndex(node ctx); // ret: 0: success, 1: error
int popUserTypeIndex(node ctx);  // ret: 0: success, 1: error

void printNode(node node, int indent);
void println(node obj);
void print(node node);

enum {
  TAG_PTR_OOP  = 0b00,   // 普通のポインタ（下位2bit=00）
  TAG_INT_OOP  = 0b01,   // 小さい整数（fixnum）
  TAG_FLT_OOP  = 0b10,   // 浮動小数点数（fixnum）
  TAG_FLAG_OOP = 0b11,   // 列挙フラグ等
};



#define MAKE_OOP_FLAG(f) ((node)(((intptr_t)(f) << TAGBITS) | TAG_FLAG_OOP))
#define MAKE_OOP_FLAG_INT(f) (((intptr_t)(f) << TAGBITS) | TAG_INT_OOP)
#define GET_OOP_FLAG(o) ((int)(((intptr_t)(o)) >> TAGBITS))
#define TAGBITS 2			// how many bits to use for tag bits
#define TAGMASK ((1 << TAGBITS) - 1)	// mask to extract just the tag bits
#define ISTAG_FLAG(o) ((((intptr_t)(o)) & TAGMASK) == TAG_FLAG_OOP) 
#define ISTAG_INT(o)  ((((intptr_t)(o)) & TAGMASK) == TAG_INT_OOP)
#define ISTAG_PTR(o)  ((((intptr_t)(o)) & TAGMASK) == TAG_PTR_OOP)

char* appendNewChar(char* arr, int size, char value);

extern node TYPES[5]; // 0: Undefined, 1: Integer, 2: Float, 3: String, 4: EventObject
void printShicaType(node type);
//--------------------------------------------------------
// Code Generation
//--------------------------------------------------------

//--------------------------------------------------------
// Library
//--------------------------------------------------------
struct CompEventTable {
	int nArgs; // number of arguments for the event handler
	char *argTypes; // types of arguments for the event handler
	int nData; // number of data for the event handler
};

struct CompStdFuncTable {
	int nArgs; // number of arguments
	int *argTypes; // types of arguments
	int retType; // return type
};

struct CompEventObjectTable {
	int nArgs; // number of arguments
	int nFuncs; // number of functions
	int *argTypes; // types of arguments
};
extern struct CompEventTable *CompEventTable;
extern struct CompStdFuncTable *CompStdFuncTable;
extern struct CompEventObjectTable *CompEventObjectTable;
void initCompLibrary();
//--------------------------------------------------------
// GC
//--------------------------------------------------------
void markObject(node obj);

#endif // NODE_H

/*
make:
void symbol_collect();
*/