#ifndef NODE_C
#define NODE_C
#include <stdarg.h>
#include <string.h>
#include "node.h"

//GC
#ifdef MSGC
#define newAtomicObject(TYPE) gc_beAtomic(newNode(TYPE))
#elif defined(MSGCS)
#define newAtomicObject(TYPE) gc_beAtomic(newNode(TYPE))
#else
	#error "GC is not defined, please define MSGC or MSGCS"
#endif

node nil   = 0;
node false = 0;
node true  = 0;
node entryEH = NULL;
node exitEH  = NULL;


int getType(node o)
{
    if ((((intptr_t)o) & TAGMASK) == TAG_INT_OOP) return Integer;
    if ((((intptr_t)o) & TAGMASK) == TAG_FLT_OOP) return Float;
    return o->_type;
}

node _check(node node, enum Type type, char *file, int line)
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

static node _newNode(size_t size, enum Type type)
{
    nobj++;
    node node = calloc(1, size);
    node->_type = type;
    return node;
}

#define newNode(TYPE)	_newNode(sizeof(struct TYPE), TYPE)

node newUndefine()
{
    return newAtomicObject(Undefined);
}

node newInteger(int value)
{
    return (node)(((intptr_t)value << TAGBITS) | TAG_INT_OOP);
}

int Integer_value(node obj)
{
    assert(Integer == getType(obj));
    return (intptr_t)obj >> TAGBITS;
}

node newFloat(char* value)
{
	double d = atof(value);
    union { double d;  intptr_t i; } u = { .d = d };
    return (node)(((intptr_t)u.i & ~TAGMASK) | TAG_FLT_OOP);
}

double Float_value(node obj)
{
    assert(Float == getType(obj));
    union { intptr_t i;  double d; } u = { .i = (intptr_t)obj };
    return u.d;
}

node newString(char *value)
{
	gc_pushRoot((void*)&value);
	GC_PUSH(node, str, newNode(String));
	str->String.value = strdup(value);
	GC_POP(str);
	gc_popRoots(1);
	return str;
}

node newEventObject(node sym, int index)
{
	gc_pushRoot((void*)&sym);
	GC_PUSH(node, eo, newNode(EventObject));
	eo->EventObject.index = index;
	eo->EventObject.funcs = malloc(sizeof(node) * 1); // initial size 1
	sym->Symbol.value = eo;
	GC_POP(eo);
	gc_popRoots(1);
	return eo;
}

node putFuncToEo(node eo, node func, node symbol, int index)
{
	//node->EventObject.funcs[index]->Symbol.name -> symbol
	//node->EventObject.funcs[index]->Symbol.value -> func 
	assert(getType(eo) == EventObject);
	assert(getType(symbol) == Symbol);
	assert(getType(func) == EventH || getType(func) == StdFunc);
	gc_pushRoot((void*)&eo);
	gc_pushRoot((void*)&func);
	gc_pushRoot((void*)&symbol);
#if DEBUG
	for(int i=0; i<index; i++)
		assert(eo->EventObject.funcs[i] != NULL); // ensure no overwrite
#endif
	assert(eo->EventObject.funcs[index] == NULL); // ensure no overwrite
	get(eo,EventObject,funcs) = (node*)realloc(get(eo,EventObject,funcs), sizeof(node) * (index + 2)); // +1 for new func
	get(symbol,Symbol,value) = func;
	get(eo,EventObject,funcs)[index] = symbol;
	get(eo,EventObject,funcs)[index + 1] = NULL;
	//remove
	gc_popRoots(3);
	return eo;
}

node newSymbol(char *name)
{
	gc_pushRoot((void*)&name);
    GC_PUSH(node, sym, newNode(Symbol));
    sym->Symbol.name  = strdup(name);
    sym->Symbol.value = false;
	gc_popRoots(2);
    return sym;
}

static node *symbols = 0;
static int nsymbols = 0;

node intern(char *name)
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
			node sym = symbols[i];
			if (sym == NULL) continue; // skip null symbols
			gc_mark(sym); // mark the symbol
		}
	}
}

node newPair(node a, node b)
{
	gc_pushRoot((void*)&a);
	gc_pushRoot((void*)&b);	
    node obj = newNode(Pair);
    obj->Pair.a = a;
    obj->Pair.b = b;
	gc_popRoots(2);
    return obj;
}

node newArgs(node value, node next)
{
	gc_pushRoot((void*)&value);
	gc_pushRoot((void*)&next);	
	node obj = newNode(Args);
	obj->Args.value = value;
	obj->Args.next  = next;
	gc_popRoots(2);
	return obj;
}

node newEparams(node type, node id, node cond, node next)
{
	gc_pushRoot((void*)&id);	
	gc_pushRoot((void*)&cond);	
	gc_pushRoot((void*)&next);	
	node obj = newNode(Eparams);
	obj->Eparams.type = type;
	obj->Eparams.id   = id;
	obj->Eparams.cond = cond;
	obj->Eparams.next = next;
	gc_popRoots(3);
	return obj;
}

node newParams(node type, node id, node next)
{
	gc_pushRoot((void*)&type);
	gc_pushRoot((void*)&id);	
	gc_pushRoot((void*)&next);	
	node obj = newNode(Params);
	obj->Params.type = type;
	obj->Params.id   = id;
	obj->Params.next = next;
	gc_popRoots(3);
	return obj;
}

node newArray(int size)
{
    GC_PUSH(node, obj, newNode(Array));
    if (size) {
		obj->Array.elements = calloc(size, sizeof(node));
		obj->Array.size     = size;
		obj->Array.capacity = size;
		for (int i = 0;  i < size;  ++i)
			obj->Array.elements[i]= nil;
    }
	GC_POP(obj);
    return obj;
}

node Array_grow(node array)
{
	gc_pushRoot((void*)&array);
    int size  = get(array, Array,size);
    int cap   = get(array, Array,capacity);
    node *elts = get(array, Array,elements);
    while (size >= cap) {
		cap = cap ? cap * 2 : 4;
		elts = realloc(elts, sizeof(*elts) * cap);
    }
    get(array, Array,capacity) = cap;
    get(array, Array,elements) = elts;
	gc_popRoots(1);
    return array;
}

node Array_append(node array, node element)
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

node Array_last(node array)
{
    int size = get(array, Array,size);
    if (size == 0) fatal("last: array is empty");
    return get(array, Array,elements)[size-1];
}

node Array_pop(node array)
{
    int size = get(array, Array,size);
    if (size == 0) fatal("pop: array is empty");
    node element = get(array, Array,elements)[--size];
    get(array, Array,size) = size;
    return element;
}

node newClosure()
{
    node node = gc_beAtomic(newNode(Closure));
    node->Closure.nArgs = 0;
    node->Closure.pos   = 0;
    return node;
}

node newStdFunc(int index)
{
	node node = newNode(StdFunc);
	node->StdFunc.index = index;
	return node;
}

node newUserFunc(node parameters, node body)
{
	gc_pushRoot((void*)&parameters);
	gc_pushRoot((void*)&body);
    node node = newNode(UserFunc);
    node->UserFunc.parameters = parameters;
    node->UserFunc.body       = body;
    node->UserFunc.code       = nil;
	gc_popRoots(2);
    return node;
}

node newBinop(enum binop op, node lhs, node rhs)
{
	gc_pushRoot((void*)&lhs);
	gc_pushRoot((void*)&rhs);
    node node = newNode(Binop);
    node->Binop.op  = op;
    node->Binop.lhs = lhs;
    node->Binop.rhs = rhs;
	gc_popRoots(2);
    return node;
}

node newUnyop(enum unyop op, node rhs)
{
	gc_pushRoot((void*)&rhs);
    node node = newNode(Unyop);
    node->Unyop.op  = op;
    node->Unyop.rhs = rhs;
	gc_popRoots(1);
    return node;
}

node newGetVar(node id,int line)
{
	gc_pushRoot((void*)&id);
    node node = newNode(GetVar);
    node->GetVar.id = id;
	node->GetVar.line = line;
	gc_popRoots(1);
    return node;
}


node newSetVar(node type, node id, node rhs, ScopeClass scope, int line)
{
	gc_pushRoot((void*)&id);
    node node = newNode(SetVar);
	node->SetVar.type = type;
    node->SetVar.id  = id;
    node->SetVar.rhs = rhs;
	node->SetVar.scope = scope;
	node->SetVar.line = line;
	gc_popRoots(1);
    return node;
}

node newGetArray(node array, node index)
{
	gc_pushRoot((void*)&array);
	gc_pushRoot((void*)&index);
    node node = newNode(GetArray);
    node->GetArray.array = array;
    node->GetArray.index = index;
	gc_popRoots(2);
    return node;
}

node newSetArray(node type, node array, node index, node value, ScopeClass scope)
{
	gc_pushRoot((void*)&array);
	gc_pushRoot((void*)&index);
	gc_pushRoot((void*)&value);
    node node = newNode(SetArray);
	node->SetArray.type = type;
    node->SetArray.array = array;
    node->SetArray.index = index;
    node->SetArray.value = value;
	node->SetArray.scope = scope; // use _type to store scope
	gc_popRoots(3);
    return node;
}

node newGetField(node id, node field, int line)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&field);
	node node = newNode(GetField);
	node->GetField.id = id;
	node->GetField.field = field;
	node->GetField.line = line;
	gc_popRoots(2);
	return node;
}

node newCall(node arguments, node function, int line)
{
	gc_pushRoot((void*)&arguments);
	gc_pushRoot((void*)&function);
    node node = newNode(Call);
    node->Call.arguments = arguments;
    node->Call.function  = function;
	node->Call.line      = line;
	gc_popRoots(2);
    return node;
}

node newReturn(node value)
{
	gc_pushRoot((void*)&value);
    node node = newNode(Return);
    node->Return.value = value;
	gc_popRoots(1);
    return node;
}

node newBreak(void)
{
    return newNode(Break);
}

node newContinue(void)
{
    return newNode(Continue);
}
node newTransition(node id)
{
	gc_pushRoot((void*)&id);
	node node = newNode(Transition);
	node->Transition.id = id;
	gc_popRoots(1);
	return node;
}

node newPrint(node arguments, int line)
{
	gc_pushRoot((void*)&arguments);
    node node = newNode(Print);
    node->Print.arguments = arguments;
	node->Print.line      = line;
	gc_popRoots(1);
    return node;
}

node newIf(node condition, node s1, node s2)
{
	gc_pushRoot((void*)&condition);
	gc_pushRoot((void*)&s1);
	gc_pushRoot((void*)&s2);
    node node = newNode(If);
    node->If.condition = condition;
    node->If.statement1 = s1;
    node->If.statement2 = s2;
	gc_popRoots(3);
    return node;
}

node newLoop(node init,node cond,node iterate, node s)
{
	gc_pushRoot((void*)&init);
	gc_pushRoot((void*)&cond);
	gc_pushRoot((void*)&iterate);
	gc_pushRoot((void*)&s);
    node node = newNode(Loop);
	node->Loop.initialization = init;
    node->Loop.condition = cond;
	node->Loop.iteration = iterate;
    node->Loop.statement = s;
	gc_popRoots(4);
    return node;
}

node newBlock(void)
{
    node node = newNode(Block);
    node->Block.statements = 0;
    node->Block.size = 0;
    return node;
}


void Block_append(node b, node s)
{
	gc_pushRoot((void*)&b);
	gc_pushRoot((void*)&s);
    node *ss = get(b, Block,statements);
    int  sz = get(b, Block,size);
    ss = realloc(ss, sizeof(node) * (sz + 1));
    ss[sz++] = s;
    get(b, Block,statements) = ss;
    get(b, Block,size) = sz;
	gc_popRoots(2);
}

//b:block, e:event
void Event_Block_append(node b, node e)
{
	gc_pushRoot((void*)&b);
	gc_pushRoot((void*)&e);

    node *ss = get(b, Block,statements);
    int  sz = get(b, Block,size);
    ss = realloc(ss, sizeof(node) * (sz + 1));

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
					node ele = ss[i];
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

node newState(node id, node parameters, node events, int line)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&parameters);
	gc_pushRoot((void*)&events);
	node node = newNode(State);
	node->State.id         = id;
	node->State.parameters = parameters;
	node->State.events     = events;
	node->State.line       = line;
	gc_popRoots(3);
	return node;
}

node newEvent(node id, node parameters, node block,int line)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&parameters);
	gc_pushRoot((void*)&block);
	node node = newNode(Event);
	node->Event.id         = id;
	node->Event.parameters = parameters;
	node->Event.block      = block;
	node->Event.line       = line;
	gc_popRoots(3);
	return node;
}
node newEventH(int index)
{
	node node = newNode(EventH);
	node->EventH.index = index;
	return node;
}

node newVariable(node type, node id)
{
	gc_pushRoot((void*)&id);
	node node = newNode(Variable);
	node->Variable.id = id;
	node->Variable.type = type;
	node->Variable.value = NULL;
	gc_popRoots(1);
	return node;
}

node newVariableWithValue(node type, node id, node value)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&value);
	node node = newNode(Variable);
	node->Variable.id = id;
	node->Variable.type = type;
	node->Variable.value = value;
	gc_popRoots(2);
	return node;
}


//FIXME: report error line number
struct RetVarFunc insertVariable(node ctx, node sym, node type)
{
	for(int scope = 0; scope < 3; scope++)
	{
		node *arr = &ctx->EmitContext.global_vars + scope;
		if(*arr == NULL) continue;
		// linear search for existing variable
		int nvariables = *arr ? get(*arr, Array, size) : 0;
		node *variables = *arr ? get(*arr, Array, elements) : 0;
		for (int i = 0;  i < nvariables;  ++i){
			if ((variables[i]->Variable.id)== sym){
				if(variables[i]->Variable.type != sym){
					reportError(ERROR, 0, "variable %s type mismatch", get(sym, Symbol,name)); 
					return (struct RetVarFunc){0, -1}; // error
				}
				return (struct RetVarFunc){(scope==0)?SCOPE_GLOBAL:(scope==1)?SCOPE_STATE_LOCAL:SCOPE_LOCAL, i, variables[i]};
			}	
		}
	}
	node arr = ctx->EmitContext.local_vars; // local variables
	gc_pushRoot((void*)&sym);
	arr->Array.elements = realloc(arr->Array.elements, sizeof(*arr->Array.elements) * (arr->Array.size + 1));
	arr->Array.elements[arr->Array.size] = newVariable(type, sym);
	gc_popRoots(1);
	return (struct RetVarFunc){SCOPE_LOCAL, arr->Array.size++, arr->Array.elements[arr->Array.size-1]};
}

/*
IF variable already exists, report error and return NULL
ELSE append variable to the array and return the appended variable
*/
//FIXME: report error line number
struct RetVarFunc appendVariable(node arr, node var, node type, node value)
{
	assert(arr != NULL);
	// linear search for existing variable
	int nvariables = arr ? arr->Array.size : 0;
	node *variables = arr ? arr->Array.elements : 0;
	for (int i = 0;  i < nvariables;  ++i){
		if ((variables[i]->Variable.id)== var){
			reportError(ERROR, 0, "variable %s already exists", get(var, Symbol,name));
			return (struct RetVarFunc){0, -1, NULL}; // error
		}
	}
	gc_pushRoot((void*)&arr);
	gc_pushRoot((void*)&var);
	gc_pushRoot((void*)&value);
	arr->Array.elements = realloc(arr->Array.elements, sizeof(*arr->Array.elements) * (arr->Array.size + 1));
	arr->Array.elements[arr->Array.size] = value ? newVariableWithValue(type, var, value) : newVariable(type, var);
	gc_popRoots(3);
	return (struct RetVarFunc){0, arr->Array.size++, arr->Array.elements[arr->Array.size-1]};
}

//FIXME: report error line number
struct RetVarFunc searchVariable(node ctx, node sym, node type)//TYPE 
{
	assert(ctx != NULL);

	for(int scope = 0; scope < 3; scope++)
	{
		node* arr = &ctx->EmitContext.global_vars + scope;
		if(*arr == NULL) continue;
		assert(getType(*arr) == Array);

		int nvariables = *arr ? get(*arr, Array, size) : 0;
		node *variables = *arr ? get(*arr, Array, elements) : 0;
		for (int i = 0;  i < nvariables;  ++i){
			if ((variables[i]->Variable.id)== sym){
				if(type!=NULL && variables[i]->Variable.type != type){
					reportError(ERROR, 0, "variable %s type mismatch", get(sym, Symbol,name));
					return (struct RetVarFunc){0, -1}; // error
				}
				return (struct RetVarFunc){(scope==0)?SCOPE_GLOBAL:(scope==1)?SCOPE_STATE_LOCAL:SCOPE_LOCAL, i, variables[i]};
			}	
		}
	}
	reportError(ERROR, 0, "variable %s not found", get(sym, Symbol,name));
	return (struct RetVarFunc){0, -1, NULL}; // not found
}

node newEmitContext()
{
	GC_PUSH(node, context, newNode(EmitContext));
	context->EmitContext.global_vars = newArray(0);
	context->EmitContext.state_vars  = NULL;
	context->EmitContext.local_vars  = NULL;
	GC_POP(context);
	return context;
}



void printlnObject(node node, int indent)
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
	case GetField: {
	    printf("GetField %s\n", get(get(node, GetField,field), Symbol,name));
	    printlnObject(get(node, GetField,id), indent+1);
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
	    printf("EventH %d\n", get(node, EventH,index));
	    break;
	}
	default:
	    assert(!"this cannot happen");
    }
}

void println(node obj)	{ printlnObject(obj, 0); }

void print(node node)
{
    switch (getType(node)) {
	case Undefined:	printf("nil");				break;
	case Integer:	printf("%d", Integer_value(node));	break;
	case Float:	printf("%f", Float_value(node));	break;
	case Symbol:	printf("%s", get(node, Symbol,name));	break;
	default:	println(node);				break;
    }
}

char* appendNewChar(char *arr, int size, char value)
{
	gc_pushRoot((void*)&arr);
	arr = realloc(arr, sizeof(*arr) * (size + 1));
	arr[size] = value;
	gc_popRoots(1);
	return arr;
}

int* appendNewInt(int *arr, int size, int value)
{
	gc_pushRoot((void*)&arr);
	arr = realloc(arr, sizeof(*arr) * (size + 1));
	arr[size] = value;
	gc_popRoots(1);
	return arr;
}

node TYPES[5] = {
	MAKE_OOP_FLAG(Undefined),// var | fn
	MAKE_OOP_FLAG(Integer),// int
	MAKE_OOP_FLAG(Float),// float
	MAKE_OOP_FLAG(String),// string
	MAKE_OOP_FLAG(EventObject),// event object
};

#ifdef MSGC
#undef newAtomicObject
#elif defined(MSGCS)
#undef newAtomicObject
#else
   #error "GC is not defined, please define MSGC or MSGCS"
#endif

#undef newNode
#undef TAGINT		
#undef TAGFLT	
#undef TAGBITS		
#undef TAGMASK

#endif // NODE_C