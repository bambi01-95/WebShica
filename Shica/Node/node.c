#ifndef NODE_C
#define NODE_C
#include <stdarg.h>
#include <string.h>
#include "node.h"

node nil   = 0;
node FALSE = 0;
node TRUE  = 0;
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
    return newAtomic(Undefined);
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
	str->String.value = gc_strdup(value);
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
	getNode(eo,EventObject,funcs) = (node*)realloc(getNode(eo,EventObject,funcs), sizeof(node) * (index + 2)); // +1 for new func
	getNode(symbol,Symbol,value) = func;
	getNode(eo,EventObject,funcs)[index] = symbol;
	getNode(eo,EventObject,funcs)[index + 1] = NULL;
	//remove
	gc_popRoots(3);
	return eo;
}

node newSymbol(char *name)
{
	gc_pushRoot((void*)&name);
    GC_PUSH(node, sym, newNode(Symbol));
    sym->Symbol.name  = strdup(name);
    sym->Symbol.value = FALSE;
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
	int cmp = strcmp(name, getNode(symbols[mid], Symbol,name));
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
    int size  = getNode(array, Array,size);
    int cap   = getNode(array, Array,capacity);
    node *elts = getNode(array, Array,elements);
    while (size >= cap) {
		cap = cap ? cap * 2 : 4;
		elts = realloc(elts, sizeof(*elts) * cap);
    }
    getNode(array, Array,capacity) = cap;
    getNode(array, Array,elements) = elts;
	gc_popRoots(1);
    return array;
}

node Array_append(node array, node element)
{
    int size = getNode(array, Array,size);
    if (size >= getNode(array, Array,capacity)){ 
		gc_pushRoot((void*)&array);
		gc_pushRoot((void*)&element);
		Array_grow(array);
		gc_popRoots(2);
	}
    getNode(array, Array,elements)[size++] = element;
    getNode(array, Array,size) = size;
    return element;
}

node Array_last(node array)
{
    int size = getNode(array, Array,size);
    if (size == 0) fatal("last: array is empty");
    return getNode(array, Array,elements)[size-1];
}

node Array_pop(node array)
{
    int size = getNode(array, Array,size);
    if (size == 0) fatal("pop: array is empty");
    node element = getNode(array, Array,elements)[--size];
    getNode(array, Array,size) = size;
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

node newGetArray(node array, node index, int line)
{
	gc_pushRoot((void*)&array);
	gc_pushRoot((void*)&index);
    node node = newNode(GetArray);
    node->GetArray.array = array;
    node->GetArray.index = index;
	gc_popRoots(2);
    return node;
}

node newSetArray(node type, node value, node index, node array, ScopeClass scope, int line)
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

node newSetType(node id, node rhs, int line)
{
	gc_pushRoot((void*)&id);
	gc_pushRoot((void*)&rhs);
	node node = newNode(SetType);
	node->SetType.id = id;
	node->SetType.rhs = rhs;
	node->SetType.line = line;
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
    node *ss = getNode(b, Block,statements);
    int  sz = getNode(b, Block,size);
    ss = realloc(ss, sizeof(node) * (sz + 1));
    ss[sz++] = s;
    getNode(b, Block,statements) = ss;
    getNode(b, Block,size) = sz;
	gc_popRoots(2);
}

//b:block, e:event
void Event_Block_append(node b, node e)
{
/*
Assignment 
Entry event handler
Common event handler
Exit event handler
*/
	gc_pushRoot((void*)&b);
	gc_pushRoot((void*)&e);

    node *ss = getNode(b, Block,statements);
    int  sz = getNode(b, Block,size);
    ss = realloc(ss, sizeof(node) * (sz + 1));

	switch(getType(e)){
		case SetVar:{
			int pos = 0;
			for(int i=0; i<sz; i++){
				node ele = ss[i];
				switch(getType(ele)){
					case SetVar:{
						continue;
					}
					case Event:{
						pos = i;
						break;
					}
					default:{
						reportError(DEVELOPER, 0, "Unexpected node type %d in Event_Block_append", getType(ele));
						break;
					}
				}
				if(pos != 0)break;
			}
			for(int i=sz-1; i>=pos; i--){
				ss[i+1] = ss[i];
			}
			dprintf("Inserting SetVar at position %d\n", pos);
			ss[pos] = e; // insert e at the beginning
			break;
		}
		case Event:{
			int isInserted = 0;
			int pos = sz; // default to end
			node id = getNode(e, Event, id);
			if(id == entryEH){
				for(int i=0; i<sz; i++){
					node ele = ss[i];
					switch(getType(ele)){
						case SetVar:break;
						case Event:{
							if(getNode(ele, Event, id) != entryEH){
								reportError(DEVELOPER, 0, "Multiple entry event handlers found");
								return;
							}
							pos = i;
							break;
						}
						default:{
							reportError(DEVELOPER, 0, "Unexpected node type %d in Event_Block_append", getType(ele));
							break;
						}
					}
					if(pos != sz)break;
				}
			}else if(id == exitEH){
				pos = sz;; // insert at the end
			}else{
				pos = sz; // default to end
				for(int i=0; i<sz; i++){
					node ele = ss[i];
					printf("Checking Event at index %d\n", i);
					switch(getType(ele)){
						case Event:{
							if(getNode(ele, Event, id)==id){
								pos = i + 1; // insert after this
							}
							break;
						}
						default:{
							break;
						}
					}
					if(pos != sz) break; // exit the loop if inserted
				}
			}
			for(int i=sz-1; i>=pos; i--){
				ss[i+1] = ss[i];
			}
			dprintf("Inserting Event at position %d\n", pos);
			ss[pos] = e; // insert e at the position
			break;
		}
		default:{
			fatal("Type %d is not supported yet", getType(e));
			exit(1);
			return;
		}
	}
	getNode(b, Block,size) = sz + 1;
    getNode(b, Block,statements) = ss;
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
		int nvariables = *arr ? getNode(*arr, Array, size) : 0;
		node *variables = *arr ? getNode(*arr, Array, elements) : 0;
		for (int i = 0;  i < nvariables;  ++i){
			if ((variables[i]->Variable.id)== sym){
				if(variables[i]->Variable.type != type){
					printf("type %d %d\n", GET_OOP_FLAG(variables[i]->Variable.type), GET_OOP_FLAG(type));
					reportError(ERROR, 0, "variable %s type mismatch", getNode(sym, Symbol,name)); 
					return (struct RetVarFunc){0, -1}; // error
				}
				return (struct RetVarFunc){(scope==0)?SCOPE_GLOBAL:(scope==1)?SCOPE_STATE_LOCAL:SCOPE_LOCAL, i, variables[i]};
			}	
		}
	}
	node arr = ctx->EmitContext.local_vars; // local variables
	if(arr == NULL){
		gc_pushRoot((void*)&ctx);
		arr = newArray(1);
		ctx->EmitContext.local_vars = arr;
		gc_popRoots(1);
	}else if(arr->Array.size + 1 > arr->Array.capacity){
		gc_pushRoot((void*)&arr);
		// reallocate 1 more element
		arr->Array.elements = realloc(arr->Array.elements, sizeof(*arr->Array.elements) * (arr->Array.size + 1));
		arr->Array.capacity = arr->Array.size + 1;
		gc_popRoots(1);
	}
	gc_pushRoot((void*)&sym);
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
			reportError(ERROR, 0, "variable %s already exists", getNode(var, Symbol,name));
			return (struct RetVarFunc){0, -1, NULL}; // error
		}
	}
	if(arr->Array.size + 1 > arr->Array.capacity){
		gc_pushRoot((void*)&arr);
		gc_pushRoot((void*)&var);
		gc_pushRoot((void*)&value);
		// reallocate 1 more element
		arr->Array.elements = realloc(arr->Array.elements, sizeof(*arr->Array.elements) * (arr->Array.size + 1));
		arr->Array.capacity = arr->Array.size + 1;
	}
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

		int nvariables = *arr ? getNode(*arr, Array, size) : 0;
		node *variables = *arr ? getNode(*arr, Array, elements) : 0;
		for (int i = 0;  i < nvariables;  ++i){
			if ((variables[i]->Variable.id)== sym){
				if(type!=NULL && type!=TYPES[Undefined] && variables[i]->Variable.type != type){
					reportError(ERROR, 0, "variable %s type mismatch", getNode(sym, Symbol,name));
					return (struct RetVarFunc){0, -1}; // error
				}
				return (struct RetVarFunc){(scope==0)?SCOPE_GLOBAL:(scope==1)?SCOPE_STATE_LOCAL:SCOPE_LOCAL, i, variables[i]};
			}	
		}
	}
	reportError(ERROR, 0, "variable %s not found", getNode(sym, Symbol,name));
	return (struct RetVarFunc){0, -1, NULL}; // not found
}

node newEmitContext()
{
	GC_PUSH(node, context, newNode(EmitContext));
	context->EmitContext.global_vars = newArray(0);
	context->EmitContext.state_vars  = NULL;
	context->EmitContext.local_vars  = NULL;
	context->EmitContext.user_types  = newArray(0);
	context->EmitContext.isVoid = 1;
	GC_POP(context);
	return context;
}

// manage user type index stack
static unsigned int userTypeIndexs[32] = {0};
static int userTypeIndexCount = 0;
int addUserType(node ctx, node ut)
{
	gc_pushRoot((void*)&ctx);
	gc_pushRoot((void*)&ut);
	Array_append(ctx->EmitContext.user_types, ut);
	gc_popRoots(2);
	return 0;
}
int pushUserTypeIndex(node ctx)
{
	if(userTypeIndexCount >= 32){
		reportError(ERROR, 0, "exceeded maximum user type nesting level");
		return 1;
	}
	userTypeIndexs[userTypeIndexCount++] = ctx->EmitContext.user_types->Array.size;
	return 0;
}
int popUserTypeIndex(node ctx)
{
	if(userTypeIndexCount <= 0){
		reportError(ERROR, 0, "user type nesting level underflow");
		return 1;
	}
	int startIndex = userTypeIndexs[--userTypeIndexCount];
	getNode(getNode(ctx, EmitContext, user_types), Array, size) = startIndex;
	return 0;
}


void printNode(node n, int indent)
{
    printf("%*s", indent*2, "");
    switch (getType(n)) {
	case Undefined:	printf("nil\n");				break;
	case Integer:	printf("%d\n", Integer_value(n));		break;
	case Symbol:	printf("%s\n", getNode(n, Symbol,name));		break;
	case Pair: {
	    printf("Pair\n");
	    printNode(getNode(n, Pair,a), indent+1);
	    printNode(getNode(n, Pair,b), indent+1);
	    break;
	}
	case Array: {
	    printf("Array\n");
	    for (int i = 0;  i < getNode(n, Array,size);  ++i)
		printNode(getNode(n, Array,elements)[i], indent+1);
	    break;
	}
	case Closure: {
	    printf("Closure\n");
		printf("nArgs: %d, pos: %d\n", getNode(n, Closure,nArgs), getNode(n, Closure,pos));
	    break;
	}
	case StdFunc: {
	    printf("StdFunc %d\n", getNode(n, StdFunc,index));
	    break;
	}
	case UserFunc: {
	    printf("function()\n");
	    printNode(getNode(n, UserFunc,parameters), indent+2);
	    printNode(getNode(n, UserFunc,body), indent+1);
	    break;
	}
	case Binop: {
	    switch (getNode(n, Binop,op)) {
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
	    printNode(getNode(n, Binop,lhs), indent+1);
	    printNode(getNode(n, Binop,rhs), indent+1);
	    break;
	}
	case Unyop: {
	    switch (getNode(n, Unyop,op)) {
		case NEG: printf("NEG\n"); break;
		case NOT: printf("NOT\n"); break;
		default:  assert(!"this cannot happen");
	    }
	    printNode(getNode(n, Unyop,rhs), indent+1);
	    break;
	}
	case GetVar: {
	    printf("GetVar %s\n", getNode(getNode(n, GetVar,id), Symbol,name));
	    break;
	}
	case SetVar: {
	    printf("SetVar %s\n", getNode(getNode(n, SetVar,id), Symbol,name));
	    printNode(getNode(n, SetVar,rhs), indent+1);
	    break;
	}
	case GetArray: {
	    printf("GetArray\n");
	    printNode(getNode(n, GetArray,array), indent+1);
	    printNode(getNode(n, GetArray,index), indent+1);
	    break;
	}
	case SetArray: {
	    printf("SetArray\n");
	    printNode(getNode(n, SetArray,array), indent+1);
	    printNode(getNode(n, SetArray,index), indent+1);
	    printNode(getNode(n, SetArray,value), indent+1);
	    break;
	}
	case GetField: {
	    printf("GetField %s\n", getNode(getNode(n, GetField,field), Symbol,name));
	    printNode(getNode(n, GetField,id), indent+1);
	    break;
	}
	case SetType: {
	    printf("SetType\n");
	    printNode(getNode(n, SetType,id), indent+1);
	    printNode(getNode(n, SetType,rhs), indent+1);
	    break;
	}
	case Call: {
	    printf("Call\n");
	    printNode(getNode(n, Call,function), indent+1);
	    printNode(getNode(n, Call,arguments), indent+1);
	    break;
	}
	case Return: {
	    printf("Return\n");
	    printNode(getNode(n, Return,value), indent+1);
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
		printf("Transition %s\n", getNode(getNode(n, Transition,id), Symbol,name));
		break;
	}
	case Print: {
	    printf("Print\n");
	    printNode(getNode(n, Print,arguments), indent+1);
	    break;
	}
	case If: {
	    printf("If\n");
	    printNode(getNode(n, If,condition), indent+1);
	    printNode(getNode(n, If,statement1), indent+1);
	    printNode(getNode(n, If,statement2), indent+1);
	    break;
	}
	case Loop: {
	    printf("Loop\n");
	    printNode(getNode(n, Loop,condition), indent+1);
		printNode(getNode(n, Loop,initialization), indent+1);
		printNode(getNode(n, Loop,iteration), indent+1);
	    printNode(getNode(n, Loop,statement), indent+2);
	    break;
	}
	case Block: {
		printf("Block\n");
		int size = getNode(n, Block,size);
		for(int i=0; i<size; i++){
			printNode(getNode(n, Block,statements)[i], indent+1);
		}
	    break;
	}
	case State: {
	    printf("State %s\n", getNode(getNode(n, State,id), Symbol,name));
	    // printNode(getNode(n, State,parameters), indent+1);
	    printNode(getNode(n, State,events), indent+1);
	    break;
	}
	case Event: {
		node id = getNode(n, Event,id);
		if(id == entryEH){
			printf("Event ENTRY\n");
		}else if(id == exitEH){
			printf("Event EXIT\n");
		}else{
			if(getType(id) != Symbol){
				printf("Event Object\n");
			}else{
				printf("Event %s\n", getNode(id, Symbol,name));
				printNode(getNode(n, Event,parameters), indent+1);
				printNode(getNode(n, Event,block), indent+1);
			}
		}
	    break;
	}
	case EventH: {
	    printf("EventH %d\n", getNode(n, EventH,index));
	    break;
	}
	case Variable:{
	    printf("Variable %s\n", getNode(getNode(n, Variable,id), Symbol,name));
		
	    break;
	}
	default:
	    assert(!"this cannot happen");
    }
}

void println(node obj)	{ printNode(obj, 0); }

void print(node node)
{
    switch (getType(node)) {
	case Undefined:	printf("nil");				break;
	case Integer:	printf("%d", Integer_value(node));	break;
	case Float:	printf("%f", Float_value(node));	break;
	case Symbol:	printf("%s", getNode(node, Symbol,name));	break;
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

node TYPES[5] = {
	MAKE_OOP_FLAG(Undefined),// var | fn
	MAKE_OOP_FLAG(Integer),// int
	MAKE_OOP_FLAG(Float),// float
	MAKE_OOP_FLAG(String),// string
	MAKE_OOP_FLAG(EventObject),// event object
};
void printShicaType(node type)
{
	printf("	Type: ");
	if(type == TYPES[Undefined]){
		printf("undefined\n");
	}else if(type == TYPES[Integer]){
		printf("int\n");
	}else if(type == TYPES[Float]){
		printf("float\n");
	}else if(type == TYPES[String]){
		printf("string\n");
	}else if(type == TYPES[EventObject]){
		printf("event object\n");
	}else{
		printf("unknown type\n");
	}
}

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