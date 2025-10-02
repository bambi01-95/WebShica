#ifndef OBJECT_C
#define OBJECT_C
#include <stdarg.h>
#include <string.h>
#include "object.h"

//GC
#ifdef MSGC
#define newAtomicObject(TYPE) gc_beAtomic(newObject(TYPE))
#elif defined(MSGCS)
#define newAtomicObject(TYPE) gc_beAtomic(newObject(TYPE))
#else
	#error "GC is not defined, please define MSGC or MSGCS"
#endif





oop nil   = 0;
oop false = 0;
oop true  = 0;
oop entryEH = NULL;
oop exitEH  = NULL;


int getType(oop o)
{
    if ((((intptr_t)o) & TAGMASK) == TAG_INT_OOP) return Integer;
    if ((((intptr_t)o) & TAGMASK) == TAG_FLT_OOP) return Float;
    return o->_type;
}

oop _check(oop node, enum Type type, char *file, int line)
{
    if (getType(node) != type) {
	fprintf(stderr, "\n%s:%d: expected type %d got type %d\n", file, line, type, getType(node));
	exit(1);
    }
    return node;
}

/*====================== OBJ FUNCTION =====================*/

/*========================================================*/

int nobj = 0;

static oop _newObject(size_t size, enum Type type)
{
    nobj++;
    oop node = calloc(1, size);
    node->_type = type;
    return node;
}

#define newObject(TYPE)	_newObject(sizeof(struct TYPE), TYPE)

oop newUndefine()
{
    return newAtomicObject(Undefined);
}

oop newInteger(int value)
{
    return (oop)(((intptr_t)value << TAGBITS) | TAG_INT_OOP);
}

int Integer_value(oop obj)
{
    assert(Integer == getType(obj));
    return (intptr_t)obj >> TAGBITS;
}

oop newFloat(char* value)
{
	double d = atof(value);
    union { double d;  intptr_t i; } u = { .d = d };
    return (oop)(((intptr_t)u.i & ~TAGMASK) | TAG_FLT_OOP);
}

double Float_value(oop obj)
{
    assert(Float == getType(obj));
    union { intptr_t i;  double d; } u = { .i = (intptr_t)obj };
    return u.d;
}

oop newString(char *value)
{
	gc_pushRoot((void*)&value);
	GC_PUSH(oop, str, newObject(String));
	str->String.value = strdup(value);
	GC_POP(str);
	gc_popRoots(1);
	return str;
}

oop newSymbol(char *name)
{
	gc_pushRoot((void*)&name);
    GC_PUSH(oop, sym, newObject(Symbol));
    sym->Symbol.name  = strdup(name);
    sym->Symbol.value = false;
	gc_popRoots(2);
    return sym;
}

static oop *symbols = 0;
static int nsymbols = 0;

oop intern(char *name)
{
    // binary search for existing symbol
    int lo = 0, hi = nsymbols - 1;
    while (lo <= hi) {
	int mid = (lo + hi) / 2;
	int cmp = strcmp(name, get(symbols[mid], Symbol,name));
	if      (cmp < 0) hi = mid - 1;
	else if (cmp > 0) lo = mid + 1;
	else{
		return symbols[mid];
    }
}
	gc_pushRoot((void*)&name);
    symbols   = realloc(symbols,   sizeof(*symbols)   * (nsymbols + 1));
    memmove(symbols + lo + 1,
	    symbols + lo,
	    sizeof(*symbols) * (nsymbols++ - lo));
	gc_popRoots(1);
    return symbols[lo] = newSymbol(name);
}

void initSymbols(void)
{
    symbols = NULL;
    nsymbols = 0;
}

void collectSymbols(void)
{
    // collect all the symbols
	if(nsymbols != 0){
		gc_markOnly(symbols); // mark the symbols array itself
		for (int i = 0;  i < nsymbols;  ++i)
		{
			oop sym = symbols[i];
			if (sym == NULL) continue; // skip null symbols
			gc_mark(sym); // mark the symbol
		}
	}
}

oop newPair(oop a, oop b)
{
	gc_pushRoot((void*)&a);
	gc_pushRoot((void*)&b);	
    oop obj = newObject(Pair);
    obj->Pair.a = a;
    obj->Pair.b = b;
	gc_popRoots(2);
    return obj;
}

oop newArgs(oop value, oop next)
{
	gc_pushRoot((void*)&value);
	gc_pushRoot((void*)&next);	
	oop obj = newObject(Args);
	obj->Args.value = value;
	obj->Args.next  = next;
	gc_popRoots(2);
	return obj;
}

oop newParams(oop type, oop id, oop next)
{
	gc_pushRoot((void*)&type);
	gc_pushRoot((void*)&id);	
	gc_pushRoot((void*)&next);	
	oop obj = newObject(Params);
	obj->Params.type = type;
	obj->Params.id   = id;
	obj->Params.next = next;
	gc_popRoots(3);
	return obj;
}

oop newArray(int size)
{
    GC_PUSH(oop, obj, newObject(Array));
    if (size) {
		obj->Array.elements = calloc(size, sizeof(oop));
		obj->Array.size     = size;
		obj->Array.capacity = size;
		for (int i = 0;  i < size;  ++i)
			obj->Array.elements[i]= nil;
    }
	GC_POP(obj);
    return obj;
}

oop Array_grow(oop array)
{
	gc_pushRoot((void*)&array);
    int size  = get(array, Array,size);
    int cap   = get(array, Array,capacity);
    oop *elts = get(array, Array,elements);
    while (size >= cap) {
		cap = cap ? cap * 2 : 4;
		elts = realloc(elts, sizeof(*elts) * cap);
    }
    get(array, Array,capacity) = cap;
    get(array, Array,elements) = elts;
	gc_popRoots(1);
    return array;
}

oop Array_append(oop array, oop element)
{
    int size = get(array, Array,size);
    if (size >= get(array, Array,capacity)){ 
		gc_pushRoot((void*)&array);
		gc_pushRoot((void*)&element);
		Array_grow(array);
		gc_popRoots(2);
	}
    get(array, Array,elements)[size++] = element;
    get(array, Array,size) = size;
    return element;
}

oop Array_last(oop array)
{
    int size = get(array, Array,size);
    if (size == 0) fatal("last: array is empty");
    return get(array, Array,elements)[size-1];
}

oop Array_pop(oop array)
{
    int size = get(array, Array,size);
    if (size == 0) fatal("pop: array is empty");
    oop element = get(array, Array,elements)[--size];
    get(array, Array,size) = size;
    return element;
}

oop newClosure()
{
    oop node = gc_beAtomic(newObject(Closure));
    node->Closure.nArgs = 0;
    node->Closure.pos   = 0;
    return node;
}

oop newStdFunc(int index)
{
	oop node = newObject(StdFunc);
	node->StdFunc.index = index;
	return node;
}

oop newUserFunc(oop parameters, oop body)
{
	gc_pushRoot((void*)&parameters);
	gc_pushRoot((void*)&body);
    oop node = newObject(UserFunc);
    node->UserFunc.parameters = parameters;
    node->UserFunc.body       = body;
    node->UserFunc.code       = nil;
	gc_popRoots(2);
    return node;
}

oop newBinop(enum binop op, oop lhs, oop rhs)
{
	gc_pushRoot((void*)&lhs);
	gc_pushRoot((void*)&rhs);
    oop node = newObject(Binop);
    node->Binop.op  = op;
    node->Binop.lhs = lhs;
    node->Binop.rhs = rhs;
	gc_popRoots(2);
    return node;
}

oop newUnyop(enum unyop op, oop rhs)
{
	gc_pushRoot((void*)&rhs);
    oop node = newObject(Unyop);
    node->Unyop.op  = op;
    node->Unyop.rhs = rhs;
	gc_popRoots(1);
    return node;
}

oop newGetVar(oop id,int line)
{
	gc_pushRoot((void*)&id);
    oop node = newObject(GetVar);
    node->GetVar.id = id;
	node->GetVar.line = line;
	gc_popRoots(1);
    return node;
}


oop newSetVar(oop type, oop id, oop rhs, ScopeClass scope, int line)
{
	gc_pushRoot((void*)&id);
    oop node = newObject(SetVar);
	node->SetVar.type = type;
    node->SetVar.id  = id;
    node->SetVar.rhs = rhs;
	node->SetVar.scope = scope;
	node->SetVar.line = line;
	gc_popRoots(1);
    return node;
}

oop newGetArray(oop array, oop index)
{
	gc_pushRoot((void*)&array);
	gc_pushRoot((void*)&index);
    oop node = newObject(GetArray);
    node->GetArray.array = array;
    node->GetArray.index = index;
	gc_popRoots(2);
    return node;
}

oop newSetArray(oop type, oop array, oop index, oop value, ScopeClass scope)
{
	gc_pushRoot((void*)&array);
	gc_pushRoot((void*)&index);
	gc_pushRoot((void*)&value);
    oop node = newObject(SetArray);
	node->SetArray.type = type;
    node->SetArray.array = array;
    node->SetArray.index = index;
    node->SetArray.value = value;
	node->SetArray.scope = scope; // use _type to store scope
	gc_popRoots(3);
    return node;
}

oop newCall(oop arguments, oop function, int line)
{
	gc_pushRoot((void*)&arguments);
	gc_pushRoot((void*)&function);
    oop node = newObject(Call);
    node->Call.arguments = arguments;
    node->Call.function  = function;
	node->Call.line      = line;
	gc_popRoots(2);
    return node;
}

oop newReturn(oop value)
{
	gc_pushRoot((void*)&value);
    oop node = newObject(Return);
    node->Return.value = value;
	gc_popRoots(1);
    return node;
}

oop newBreak(void)
{
    return newObject(Break);
}

oop newContinue(void)
{
    return newObject(Continue);
}
oop newTransition(oop id)
{
	gc_pushRoot((void*)&id);
	oop node = newObject(Transition);
	node->Transition.id = id;
	gc_popRoots(1);
	return node;
}

oop newPrint(oop arguments, int line)
{
	gc_pushRoot((void*)&arguments);
    oop node = newObject(Print);
    node->Print.arguments = arguments;
	node->Print.line      = line;
	gc_popRoots(1);
    return node;
}

oop newIf(oop condition, oop s1, oop s2)
{
	gc_pushRoot((void*)&condition);
	gc_pushRoot((void*)&s1);
	gc_pushRoot((void*)&s2);
    oop node = newObject(If);
    node->If.condition = condition;
    node->If.statement1 = s1;
    node->If.statement2 = s2;
	gc_popRoots(3);
    return node;
}

oop newLoop(oop init,oop cond,oop iterate, oop s)
{
	gc_pushRoot((void*)&init);
	gc_pushRoot((void*)&cond);
	gc_pushRoot((void*)&iterate);
	gc_pushRoot((void*)&s);
    oop node = newObject(Loop);
	node->Loop.initialization = init;
    node->Loop.condition = cond;
	node->Loop.iteration = iterate;
    node->Loop.statement = s;
	gc_popRoots(4);
    return node;
}

oop newBlock(void)
{
    oop node = newObject(Block);
    node->Block.statements = 0;
    node->Block.size = 0;
    return node;
}


void Block_append(oop b, oop s)
{
	gc_pushRoot((void*)&b);
	gc_pushRoot((void*)&s);
    oop *ss = get(b, Block,statements);
    int  sz = get(b, Block,size);
    ss = realloc(ss, sizeof(oop) * (sz + 1));
    ss[sz++] = s;
    get(b, Block,statements) = ss;
    get(b, Block,size) = sz;
	gc_popRoots(2);
}

//b:block, e:event
void Event_Block_append(oop b, oop e)
{
	gc_pushRoot((void*)&b);
	gc_pushRoot((void*)&e);

    oop *ss = get(b, Block,statements);
    int  sz = get(b, Block,size);
    ss = realloc(ss, sizeof(oop) * (sz + 1));

	switch(getType(e)){
		case Event:{
			int isInserted = 0;
			if(get(e, Event, id) == entryEH){
				for(int i=sz-1; i>=0; i--){
					ss[i+1] = ss[i];
				}
				ss[0] = e;
				isInserted = 1;
			}else if(get(e, Event, id) == exitEH){
				if(sz == 0){
					ss[0] = e;
				}else if(get(ss[sz-1], Event, id) == entryEH){
					for(int i=sz-1; i>=1; i--){
						ss[i+1] = ss[i];
					}
					ss[1] = e;
				}else{
					for(int i=sz-1; i>=0; i--){
						ss[i+1] = ss[i];
					}
					ss[0] = e;
				}
				isInserted = 1;
			}else{
				for(int i=sz-1; i>=0; i--){
					oop ele = ss[i];
					switch(getType(ele)){
						case Event:{
							if(get(ele, Event, id)==get(e, Event, id)){
								for(int j=i+1; j < sz; j++){
									ss[j + 1] = ss[j];
								}
								ss[i + 1] = e; // insert e at position i + 1
								isInserted = 1;
							}
							break;
						}
						default:{
							break;
						}
					}
					if(isInserted) break; // exit the loop if inserted
				}
			}
			if(isInserted == 0){
				ss[sz] = e; // insert e at the end
			}
			break;
		}
		default:{
			fatal("Type %d is not supported yet", getType(e));
			return;
		}
	}
	get(b, Block,size) = sz + 1;
    get(b, Block,statements) = ss;
#if DEBUG
	for(int i=0; i<sz+1; i++){
		if(ss[i] == NULL){
			fatal("line %d ERROR: EVENT_BLOCK_APPEND()\n- elements include NULL pointer\n- contact the developer\n", __LINE__);
			exit(1);
		}
	}
#endif


	gc_popRoots(2);
}

oop newState(oop id, oop parameters, oop events, int line)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&parameters);
	gc_pushRoot((void*)&events);
	oop node = newObject(State);
	node->State.id         = id;
	node->State.parameters = parameters;
	node->State.events     = events;
	node->State.line       = line;
	gc_popRoots(3);
	return node;
}

oop newEvent(oop id, oop parameters, oop block,int line)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&parameters);
	gc_pushRoot((void*)&block);
	oop node = newObject(Event);
	node->Event.id         = id;
	node->Event.parameters = parameters;
	node->Event.block      = block;
	node->Event.line       = line;
	gc_popRoots(3);
	return node;
}
oop newEventH(int id, int nArgs)
{
	oop node = newObject(EventH);
	node->EventH.id = id;
	node->EventH.nArgs = nArgs;
	return node;
}

oop newVariable(oop type, oop id)
{
	gc_pushRoot((void*)&id);
	oop node = newObject(Variable);
	node->Variable.id = id;
	node->Variable.type = type;
	gc_popRoots(1);
	return node;
}



struct RetVarFunc insertVariable(oop ctx, oop sym, oop type)
{
	for(int scope = 0; scope < 3; scope++)
	{
		oop arr = ctx->EmitContext.global_vars + sizeof(oop) * scope;
		if(arr == NULL) continue;
		// linear search for existing variable
		int nvariables = arr ? arr->Array.size : 0;
		oop *variables = arr ? arr->Array.elements : 0;
		for (int i = 0;  i < nvariables;  ++i){
			if ((variables[i]->Variable.id)== sym){
				if(variables[i]->Variable.type != sym){
					reportError("variable %s type mismatch", get(sym, Symbol,name));
					return (struct RetVarFunc){0, -1}; // error
				}
				return (struct RetVarFunc){(scope==0)?SCOPE_GLOBAL:(scope==1)?SCOPE_STATE_LOCAL:SCOPE_LOCAL, i};
			}	
		}
	}
	oop arr = ctx->EmitContext.local_vars; // local variables
	gc_pushRoot((void*)&sym);
	arr->Array.elements = realloc(arr->Array.elements, sizeof(*arr->Array.elements) * (arr->Array.size + 1));
	arr->Array.elements[arr->Array.size] = newVariable(type, sym);
	gc_popRoots(1);
	return (struct RetVarFunc){SCOPE_LOCAL, arr->Array.size++};
}

/*
IF variable already exists, report error and return NULL
ELSE append variable to the array and return the appended variable
*/
struct RetVarFunc appendVariable(oop arr, oop var, oop type)
{
	// linear search for existing variable
	int nvariables = arr ? arr->Array.size : 0;
	oop *variables = arr ? arr->Array.elements : 0;
	for (int i = 0;  i < nvariables;  ++i){
		if ((variables[i]->Variable.id)== var){
			reportError("variable %s already exists", get(var, Symbol,name));
			return (struct RetVarFunc){0, -1}; // error
		}
	}
	gc_pushRoot((void*)&arr);
	gc_pushRoot((void*)&var);
	arr->Array.elements = realloc(arr->Array.elements, sizeof(*arr->Array.elements) * (arr->Array.size + 1));
	arr->Array.elements[arr->Array.size] = newVariable(type, var);
	gc_popRoots(2);
	return (struct RetVarFunc){0, arr->Array.size++};
}

struct RetVarFunc searchVariable(oop ctx, oop sym, oop type)
{
	assert(ctx != NULL);
	for(int scope = 0; scope < 3; scope++)
	{
		oop arr = ctx->EmitContext.global_vars + sizeof(oop) * scope;
		if(arr == NULL) continue;
		int nvariables = arr ? arr->Array.size : 0;
		oop *variables = arr ? arr->Array.elements : 0;
		for (int i = 0;  i < nvariables;  ++i){
			if ((variables[i]->Variable.id)== sym){
				if(variables[i]->Variable.type != type){
					reportError("variable %s type mismatch", get(sym, Symbol,name));
					return (struct RetVarFunc){0, -1}; // error
				}
				return (struct RetVarFunc){(scope==0)?SCOPE_GLOBAL:(scope==1)?SCOPE_STATE_LOCAL:SCOPE_LOCAL, i};
			}	
		}
	}
	reportError("variable %s not found", get(sym, Symbol,name));
	return (struct RetVarFunc){0, -1}; // not found
}

oop newEmitContext()
{
	GC_PUSH(oop, context, newObject(EmitContext));
	context->EmitContext.global_vars = newArray(0);
	context->EmitContext.state_vars  = NULL;
	context->EmitContext.local_vars  = NULL;
	GC_POP(context);
	return context;
}



void printlnObject(oop node, int indent)
{
    printf("%*s", indent*2, "");
    switch (getType(node)) {
	case Undefined:	printf("nil\n");				break;
	case Integer:	printf("%d\n", Integer_value(node));		break;

	case Symbol:	printf("%s\n", get(node, Symbol,name));		break;
	case Pair: {
	    printf("Pair\n");
	    printlnObject(get(node, Pair,a), indent+1);
	    printlnObject(get(node, Pair,b), indent+1);
	    break;
	}
	case Array: {
	    printf("Array\n");
	    for (int i = 0;  i < get(node, Array,size);  ++i)
		printlnObject(get(node, Array,elements)[i], indent+1);
	    break;
	}
	case Closure: {
	    printf("Closure\n");
		printf("nArgs: %d, pos: %d\n", get(node, Closure,nArgs), get(node, Closure,pos));
	    break;
	}
	case StdFunc: {
	    printf("StdFunc %d\n", get(node, StdFunc,index));
	    break;
	}
	case UserFunc: {
	    printf("function()\n");
	    printlnObject(get(node, UserFunc,parameters), indent+2);
	    printlnObject(get(node, UserFunc,body), indent+1);
	    break;
	}
	case Binop: {
	    switch (get(node, Binop,op)) {
		case NE:  printf("NE\n"); break;
		case EQ:  printf("EQ\n"); break;
		case LT:  printf("LT\n"); break;
		case LE:  printf("LE\n"); break;
		case GE:  printf("GE\n"); break;
		case GT:  printf("GT\n"); break;
		case ADD: printf("ADD\n"); break;
		case SUB: printf("SUB\n"); break;
		case MUL: printf("MUL\n"); break;
		case DIV: printf("DIV\n"); break;
		case MOD: printf("MOD\n"); break;
		default:  assert(!"this cannot happen");
	    }
	    printlnObject(get(node, Binop,lhs), indent+1);
	    printlnObject(get(node, Binop,rhs), indent+1);
	    break;
	}
	case Unyop: {
	    switch (get(node, Unyop,op)) {
		case NEG: printf("NEG\n"); break;
		case NOT: printf("NOT\n"); break;
		default:  assert(!"this cannot happen");
	    }
	    printlnObject(get(node, Unyop,rhs), indent+1);
	    break;
	}
	case GetVar: {
	    printf("GetVar %s\n", get(get(node, GetVar,id), Symbol,name));
	    break;
	}
	case SetVar: {
	    printf("SetVar %s\n", get(get(node, SetVar,id), Symbol,name));
	    printlnObject(get(node, SetVar,rhs), indent+1);
	    break;
	}
	case GetArray: {
	    printf("GetArray\n");
	    printlnObject(get(node, GetArray,array), indent+1);
	    printlnObject(get(node, GetArray,index), indent+1);
	    break;
	}
	case SetArray: {
	    printf("SetArray\n");
	    printlnObject(get(node, SetArray,array), indent+1);
	    printlnObject(get(node, SetArray,index), indent+1);
	    printlnObject(get(node, SetArray,value), indent+1);
	    break;
	}
	case Call: {
	    printf("Call\n");
	    printlnObject(get(node, Call,function), indent+1);
	    printlnObject(get(node, Call,arguments), indent+1);
	    break;
	}
	case Return: {
	    printf("Return\n");
	    printlnObject(get(node, Return,value), indent+1);
	    break;
	}
	case Break: {
	    printf("Break\n");
	    break;
	}
	case Continue: {
	    printf("Continue\n");
	    break;
	}
	case Transition: {
		printf("Transition %s\n", get(get(node, Transition,id), Symbol,name));
		break;
	}
	case Print: {
	    printf("Print\n");
	    printlnObject(get(node, Print,arguments), indent+1);
	    break;
	}
	case If: {
	    printf("If\n");
	    printlnObject(get(node, If,condition), indent+1);
	    printlnObject(get(node, If,statement1), indent+1);
	    printlnObject(get(node, If,statement2), indent+1);
	    break;
	}
	case Loop: {
	    printf("Loop\n");
	    printlnObject(get(node, Loop,condition), indent+1);
		printlnObject(get(node, Loop,initialization), indent+1);
		printlnObject(get(node, Loop,iteration), indent+1);
	    printlnObject(get(node, Loop,statement), indent+2);
	    break;
	}
	case Block: {
	    printf("Block...\n");
	    break;
	}
	case State: {
	    printf("State %s\n", get(get(node, State,id), Symbol,name));
	    printlnObject(get(node, State,parameters), indent+1);
	    printlnObject(get(node, State,events), indent+1);
	    break;
	}
	case Event: {
	    printf("Event %s\n", get(get(node, Event,id), Symbol,name));
	    printlnObject(get(node, Event,parameters), indent+1);
	    printlnObject(get(node, Event,block), indent+1);
	    break;
	}
	case EventH: {
	    printf("EventH %d, nArgs: %d\n", get(node, EventH,id), get(node, EventH,nArgs));
	    break;
	}
	default:
	    assert(!"this cannot happen");
    }
}

void println(oop obj)	{ printlnObject(obj, 0); }

void print(oop node)
{
    switch (getType(node)) {
	case Undefined:	printf("nil");				break;
	case Integer:	printf("%d", Integer_value(node));	break;
	case Float:	printf("%f", Float_value(node));	break;
	case Symbol:	printf("%s", get(node, Symbol,name));	break;
	default:	println(node);				break;
    }
}

#ifdef MSGC
#undef newAtomicObject
#elif defined(MSGCS)
#undef newAtomicObject
#else
   #error "GC is not defined, please define MSGC or MSGCS"
#endif

#undef newObject
#undef TAGINT		
#undef TAGFLT	
#undef TAGBITS		
#undef TAGMASK

#endif // OBJECT_C