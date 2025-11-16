#ifndef DEBUG //for executer
#define DEBUG 0
#endif



/*
GLOBAL VARIABLES: should start with a capital letter
Struct Name: should start with a capital letter
UserFunc Name: should start with a lowwer letter
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdint.h>
#include <unistd.h>


#include "./GC/gc.h"
#include "./Error/error.h"
#include "./Opcode/opcode.h"
#include "./Node/node.h"
#include "./Parser/parser.h"
#include "./Object/object.h"
#include "./Tool/tool.h"

#ifdef WEBSHICA
#include "./Platform/WebShica/Library/library.h"
#else // LINUX
#include "./Platform/Linux/Library/library.h"
#endif

/* GARBAGE? */

// oop *Agents = NULL;
// int nAgents = 0; // number of agents
// oop *codes = NULL; // codes for each agent
// int nCodes = 0; // number of codes

/* end of GARBAGE */

/* ======================= ERROR MSG ==================== */


void error(char *msg, ...){
    va_list ap;
	va_start(ap, msg);
#ifdef WEBSHICA
	fprintf(stdin, "\nError: ");
	vfprintf(stdin, msg, ap);
	fprintf(stdin, "\n");
	fflush(stdin);
#else
	fprintf(stderr, "\nError: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
    va_end(ap);
	return;
}


void rprintf(char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	printf("\x1b[31m");
	printf( msg, ap);
	printf("\x1b[0m");
	va_end(ap);
}



/*======================= COMPILE =======================*/
/* EVENT HANDLER */
int compile_event_init(){
	//lsl event handler
	entryEH = intern("entryEH");
	entryEH->Symbol.value = newEventH(0); // 0 argument
	exitEH = intern("exitEH");
	exitEH->Symbol.value =  newEventH(0); // 0 argument
	//standard event handler
	setEventTable(__EventTable__);
	setStdFuncTable(__StdFuncTable__);
	compile_eh_init();//platform/platformName/library.c

	return 1; // return 1 to indicate success
}
/* STANDARD LIBRARY */
//compile_func_init();//platform/platformName/library.c


/* ==================== EXECUTOR ==================== */
/* EVENT HANDLER */
int executor_event_init()
{
	// Initialize the event handler for the executor
	setEventTable(__EventTable__);
	return 1;
}

/* STANDARD LIBRARY */
int executor_func_init()
{
	// Initialize the function handler for the executor
	setStdFuncTable(__StdFuncTable__);
	setEventObjectFuncTable(__EventObjectFuncTable__);
	return 1;
}



////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


#if DEBUG
#define dprintf(...) printf("line %d: ", __LINE__); printf(__VA_ARGS__); printf("\n");
#else
#define dprintf(...) ;
#endif


#define push(O)	pushStack(stack, O)
#define pop()	popStack(stack)
#define top()	lastStack(stack)
#define pick(I)  stack->Stack.elements[I]


oop execute(oop prog, oop entity, oop agent);
#define TAGBITS 2
enum {
	TAG_PTR = 0b00, // ordinary pointer (lower 2 bits = 00)
	TAG_INT=0b01,
	TAG_FLT=0b10,
	TAG_FLAG=0b11,
};
typedef enum { ERROR_F,NONE_F, HALT_F, EOE_F, EOC_F, CONTINUE_F,TRANSITION_F } retFlag_t;
#define MAKE_FLAG(f) ((oop)(((intptr_t)(f) << TAGBITS) | TAG_FLAG))

oop retFlags[7] = {
	MAKE_FLAG(ERROR_F),
	MAKE_FLAG(NONE_F),
	MAKE_FLAG(HALT_F),
	MAKE_FLAG(EOE_F),
	MAKE_FLAG(EOC_F),
	MAKE_FLAG(CONTINUE_F),
	MAKE_FLAG(TRANSITION_F),
};

oop impleBody(oop code, oop eh, oop agent){
	// printf("\t kind %d\n", code->kind);
	assert(getKind(code) == IntArray);
	assert(getKind(eh) == EventHandler);
	assert(getKind(agent) == Agent);
	oop *threads = getObj(eh, EventHandler, threads);
	oop ret = retFlags[NONE_F];
	for(int i=0;i<getObj(eh, EventHandler, size); ++i){
		oop thread = threads[i];
		if(getObj(thread, Thread, inProgress) == 1){
			ret = execute(code,thread, agent);
		}else if(getObj(thread, Thread, queue)->IntQue3.size > 0){
			getObj(thread, Thread, stack) = dequeue3(thread);
			assert(getObj(thread, Thread, stack) != NULL);
			ret = execute(code, thread, agent);
		}
		if(ret == retFlags[TRANSITION_F]){
			return ret;
		}
		if(ret == retFlags[ERROR_F]){
			return ret;
		}
	}
	return ret; // return 1 to indicate success
}


/* =========================================== */
int runNative(oop code){
	GC_PUSH(oop, agent, newAgent(0,0));//nroos[0] = agent; // set the first root to the agent
	agent = execute(code, agent, agent);
	if(agent == retFlags[ERROR_F]){
		GC_POP(agent);
		return 1; // return 1 to indicate error
	}
	dprintf("agent: %d\n", getObj(agent, Agent, isActive));
	while(1){
		if(getObj(agent, Agent, isActive) == 0) {
			agent = execute(code ,agent , agent);
			if(agent == retFlags[ERROR_F]){
				GC_POP(agent);
				return 1; // return 1 to indicate error
			}
		}else{
			for(int i = 0; i< getObj(agent,Agent,nEvents); ++i){
				// get event data
				oop eh = getObj(agent,Agent,eventHandlers)[i];
				EventTable[getObj(eh, EventHandler, EventH)].eh(eh);
				oop ret = impleBody(code, eh, agent);
				if(ret == retFlags[TRANSITION_F]){
					getObj(agent,Agent,isActive) = 0;
					break;
				}
				if(ret == retFlags[ERROR_F]){
					GC_POP(agent);
					return 1; // return 1 to indicate error
				}
			}
		}
	}
	GC_POP(agent);
	return 0; // return 0 to indicate success
}

oop execute(oop prog,oop entity, oop agent)
{

	printf("Execute Start: entity kind %d\n", getKind(entity));
	int opstep = 20; // number of operations to execute before returning
    int* code = getObj(prog, IntArray, elements);
	int size = getObj(prog, IntArray, size);
	int *pc;
	int *rbp;
	oop stack;
	switch(entity->kind) {
		case Thread:{
			pc = &getObj(entity, Thread, pc);
			rbp = &getObj(entity, Thread, rbp);
			stack = getObj(entity, Thread, stack);
			break;
		}
		case Agent:{
			pc = &getObj(entity, Agent, pc);
			rbp = &getObj(entity, Agent, rbp);
			stack = getObj(entity, Agent, stack);
			opstep = 100; // number of operations to execute before returning
			break;
		}
		default:{
			reportError(DEVELOPER, 0, "execute: unknown entity kind %d", getKind(entity));
			return retFlags[ERROR_F]; // should never reach here
		}
	}

# define fetch()	code[(*pc)++]

    for (;;) {

	if (opstep-- <= 0) {
		return retFlags[CONTINUE_F]; // return CONTINUE_F to indicate that the execution is not finished
	}

	int op = fetch();
	int l = 0, r = 0;
	switch (op) {
		case iMKSPACE:{
			printOP(iMKSPACE);
			int nvars = fetch();
			for (int i = 0;  i < nvars;  ++i) {
				push(0); // reserve space for local variables
			}
			if(getKind(entity) == Agent){
				getObj(entity, Agent, rbp) = nvars;
			}
			continue;
		}
	    case iGT:printOP(iGT);    r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l > r));  continue;
		case iGE:printOP(iGE);    r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l >= r)); continue;
		case iEQ:printOP(iEQ);    r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l == r)); continue;
		case iNE:printOP(iNE);    r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l != r)); continue;
		case iLE:printOP(iLE);    r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l <= r)); continue;
		case iLT:printOP(iLT);    r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l < r));  continue;
	    case iADD:printOP(iADD);  r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l + r));  continue;
	    case iSUB:printOP(iSUB);  r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l - r));  continue;
	    case iMUL:printOP(iMUL);  r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l * r));  continue;
	    case iDIV:printOP(iDIV);  r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l / r));  continue;
	    case iMOD:printOP(iMOD);  r = (intptr_t)pop();  l = (intptr_t)pop();  push(newIntVal(l % r));  continue;
		case iARRAY:{
			int n = fetch();
			oop arr = newArrVal(n);
			oop *ele = getObj(arr, ArrVal, elements);
			for(int i=0; i<n; ++i){
				ele[i] = pop();
			}
			push(arr);
			continue;
		}
		case iSETARRAY:{
			printOP(iSETARRAY);
			oop val   = pop();
			int index = (intptr_t)pop();
			oop arr   = pop();
			assert(getKind(arr) == ArrVal);
			getObj(arr, ArrVal, elements)[index] = val;
			continue;
		}
		case iGETARRAY:{
			printOP(iGETARRAY);
			int index = (intptr_t)pop();
			oop arr = pop();
			assert(getKind(arr) == ArrVal);
			oop *ele = getObj(arr, ArrVal, elements);
			push(ele[index]);
			continue;
		}
	    case iGETVAR:{
			printOP(iGETVAR);
			int symIndex = fetch(); // need to change
			push(stack->Stack.elements[symIndex + *rbp +1]); // get the variable value
			continue;
		}
		case iGETGLOBALVAR:{ /* I: index from global-stack[0] to value */
			printOP(iGETGLOBALVAR);
			int symIndex = fetch(); 
			push(getObj(getObj(agent, Agent, stack), Stack, elements)[symIndex]);
			continue;
		}
		case iGETSTATEVAR:{ /* I: index from state-stack[0 + rbp] to value */
			printOP(iGETSTATEVAR);
			int symIndex = fetch(); 
			continue;
		}
	    case iSETVAR:{ /* I: index from local-stack[0 + rbp] to value, memo: local-stack[0] is init rbp value */
			printOP(iSETVAR);
			int symIndex = fetch();
			stack->Stack.elements[symIndex+*rbp+1] = pop();
			continue;
		}
		case iSETGLOBALVAR:{
			printOP(iSETGLOBALVAR);
			int symIndex = fetch();
			agent->Agent.stack->Stack.elements[symIndex] = pop(); // set the global variable value
			continue;
		}
		case iSETSTATEVAR:{
			printOP(iSETSTATEVAR);
			int index = fetch();
			int symIndex = index + agent->Agent.rbp;
			if(symIndex > agent->Agent.stack->Stack.size -1)//FIXME: every time check the size is not efficient, it should be done at init state.
				for(int i = agent->Agent.stack->Stack.size; i <= symIndex; ++i)
					intArray_push(agent->Agent.stack, 0); // extend the state variable array
			agent->Agent.stack->Stack.elements[symIndex] = pop(); // set the state variable value
			continue;
		}
		case iJUMP:{
			printOP(iJUMP);
			l = fetch();
			if (l >= size) {
				fatal("jump to invalid position %d", l);
			}
			*pc += l; // set program counter to the jump position
			continue;
		}
		case iJUMPIF:{
			printOP(iJUMPIF);
			l = fetch();
			if (l < 0 || l >= size) {
				fatal("jump to invalid position %d", l);
			}
			if (pop()) { // if top of stack is TRUE
				continue;
			} else {
				*pc+=l; // skip the jump
			}
			continue;
		}
		case iRETURN:{
			printOP(iRETURN);
			if (*rbp == 0) {
				fatal("return without call");
			}
			oop retValue = pop(); // get the return value
			stack->Stack.size = *rbp+1; // restore the stack size to the base pointer
			*rbp = (intptr_t)pop(); // restore the base pointer
			*pc = (intptr_t)pop(); // restore the program counter

			push(retValue); // push the return value to the stack
			continue;
		}
		case iPCALL:{
			printOP(iPCALL);
			continue;
		}
		case iSCALL:{
			printOP(iSCALL);
			StdFuncTable[fetch()].stdfunc(stack); // call the standard function
			continue;
		}
		case iUCALL:{
			printOP(iUCALL);
			l = fetch(); // get the function rel position
			r = fetch(); // get the number of arguments
			push(newIntVal(*pc)); // save the current program counter
			*pc += l; // set program counter to the function position

			push(newIntVal(*rbp)); // save the current base pointer
			*rbp = stack->Stack.size-1; // set the base pointer to the current stack size
			dprintf("rbp: %d, pc: %d\n", *rbp, *pc);

			for (int i = 1;  i <= r;  ++i) {
				push(pick((*rbp - i -1)));//pc
			}
			continue;
		}
		case eCALL:{
			printOP(eCALL);
			l = fetch(); // get index of Function for initializing EO.
			push(EventObjectFuncTable[l](stack));
			continue;
		}
		case iPUSH:{
		printOP(iPUSH);
			l = fetch();
			push(newIntVal(l));
			continue;
		}
		case sPUSH:{
			printOP(sPUSH);
			l = fetch();
			char *str = gc_beAtomic(gc_alloc(l+1));
			oop obj = newStrVal(str);
			memcpy(str, &code[(*pc)], l * sizeof(char));
			str[l] = '\0';
			obj->StrVal.value = str;
			push(obj);
			
			if(l%4)*pc += l/4 + 1;
			else *pc += l/4;
			continue;
		}
		case iCLEAN:{
			printOP(iCLEAN);
			l = fetch(); // number of variables to clean
			oop retVal = pop(); // get the return value
			for(int i = 0;  i < l;  ++i) {
				if (stack->Stack.size == 0) {
					fatal("stack is empty");
				}
				pop(); // clean the top element
			}
			push(retVal); // push the return value back to the stack
			// stack->Stack.size -= l; // clean the top l elements from the stack
			continue;
		}
		case aPRINT:{
			printOP(aPRINT);
			oop o = pop();
			if(o->kind == IntVal){
				printf("%d\n", IntVal_value(o));
			}else if(o->kind == FloVal){
				printf("%f\n", FloVal_value(o));
			}else if(o->kind == StrVal){
				printf("%s\n", StrVal_value(o));
			}else{
				printf("%p\n", o);
			}
			continue;
		}
		case iPRINT:{
			printOP(iPRINT);
			int val = IntVal_value(pop());
			printf("%d\n", val);
			continue;
		}
		case sPRINT:{
			printOP(sPRINT);
			oop obj = pop();
			char *str = StrVal_value(obj);
			printf("%s", str);
			continue;
		}
	    case fPRINT:{
			printOP(fPRINT);
			oop obj = pop();
			printf("%f", FloVal_value(obj));
			continue;
		}
		case flashPRINT:{
			printf("\n");
			continue;
		}
		case iTRANSITION:{
			printOP(iTRANSITION);
			int nextStatePos = fetch();
			pushStack(agent->Agent.stack,  newIntVal(nextStatePos+(*pc)));//relative position:
			return retFlags[TRANSITION_F];
		}
		case iSETSTATE:{
			printOP(iSETSTATE);
			assert(entity->kind == Agent);
			int ehSize = fetch(); // get the number of event handlers
			assert(ehSize >= 0);
			entity->Agent.nEvents = ehSize; // set the number of events
			oop *ehs = entity->Agent.eventHandlers = (oop*)gc_beAtomic(malloc(sizeof(oop*) * ehSize)); //initialize the event handlers
			for(int i=0; i<ehSize; ++i){
				op = fetch();
				oop eoEhData = NULL;
				switch(op){
					case iGETGLOBALVAR:{
						l = fetch();
						eoEhData = agent->Agent.stack->Stack.elements[l];
						op = fetch();
						break;
					}
					case iGETSTATEVAR:{
						l = fetch();
						eoEhData = agent->Agent.stack->Stack.elements[agent->Agent.rbp + l];
						op = fetch();
						break;
					}
					default:{
						reportError(DEVELOPER, 0, "iSETSTATE: unknown opcode %d for event handler initialization", op);
						return retFlags[ERROR_F]; // return ERROR_F to indicate error
					}
				}
				assert(op == iSETEVENT);
				printOP(iSETEVENT);
				int eventID = fetch(); // get the event ID
				int nThreads = fetch(); // get the number of threads
				ehs[i] = newEventHandler(eventID, nThreads); // initialize the event handler <------ ERROR: FIX HERE
				if(eoEhData == NULL){
					EventTable[eventID].init(ehs[i]);// initialize the event handler data (std event object)
				}else{
					ehs[i]->EventHandler.data[0]/*event object eh data[0] should be hold instance data*/ = eoEhData;
				}
				for(int j=0; j<nThreads; ++j){
					op = fetch();
					assert(op == iSETPROCESS);
					printOP(iSETPROCESS);
					l = fetch(); // get the aPos
					r = fetch(); // get the cPos
					ehs[i]->EventHandler.threads[j] = newThread(l,r,eventID); // initialize the thread
				}
			}
			op = fetch();
			assert(op == iIMPL);
			printOP(iIMPL);
			entity->Agent.isActive = 1; // set the agent to active
			return entity; // return the entity
		}
		case iSETEVENT:{
			printOP(iSETEVENT);
			fatal("iSETEVENT should not be called here");
			continue;
		}
		case iSETPROCESS:{
			printOP(iSETPROCESS);
			fatal("iSETPROCESS should not be called here");
			continue;
		}
		case iIMPL:{
			printOP(iIMPL);
			continue;
		}
		case iEOC:{
			printOP(iEOC);
			return retFlags[EOC_F]; // return EOC_F to indicate end of code
		}
		case iEOE:{
			printOP(iEOE);
			int nVariables = fetch();
			switch(getKind(entity)){
				case Thread:{
					getObj(entity, Thread, inProgress) = 0; // set the thread to not in progress
					getObj(entity, Thread, pc) += IntVal_value(getObj(stack, Stack, elements)[0]); // restore the program counter
					getObj(entity, Thread, rbp) = 0;
					return retFlags[EOE_F]; // return EOE_F to indicate end of execution
				}
				case Agent:{
					getObj(entity, Agent, isActive) = 0; // set the agent to not active
					for(int i=0; i<nVariables; i++){
						pop();
					}
					continue;
				}
				default:{
					reportError(DEVELOPER, 0, "iEOE: unknown entity kind %d", entity->kind);
					return retFlags[ERROR_F]; // should never reach here
				}
			}
			continue;
		}
		case iEOS:{
			printOP(iEOS);
			assert(entity->kind == Agent);
			int variablesSize = fetch();
			for(int i = 0; i < variablesSize; i++){
				pop(); // clean the top variablesSize elements from the stack
			}
			getObj(entity, Agent, isActive) = 0;
			getObj(entity, Agent, pc) += IntVal_value(pop());
			getObj(entity, Agent, eventHandlers) = NULL;//TODO: don't remove this in the future
			continue;
		}
	    case iHALT:{
			printOP(iHALT);
			pop();//first rbp
			for(int i = 0;  i < getObj(stack, Stack, size);  ++i) {
				printf("%d ", IntVal_value(getObj(stack, Stack, elements)[i]));
			}
			printf("\n");
			return stack; // return the answer
		}
	    default:{
			reportError(DEVELOPER, 0, "execute: unknown opcode %d at pc %d", op, *pc -1);
			return retFlags[ERROR_F]; // return ERROR_F to indicate error
		}
	}
	}
	reportError(DEVELOPER, 0, "execute: reached unreachable code");
	return retFlags[ERROR_F]; // should never reach here
# undef fetch
# undef push
# undef pop
}



/* ========================== COMPILER =============================== */

/* =================================================================== */


/* ========== OBJECT ============ */

/* ============================== */

/* = VARIABLE === */

/* =============== */
oop compile();



/* ==== STATE ==== */

/* =============== */

// STATESSm
// s1: pos, name;
// s2: pos, name;
// s3: pos, name;
enum { APSTATE = 0, ATRANSITION = 1 };

static node *states = 0;
static int nstates = 0;
static node *transitions = 0; // transitions between states
static int ntransitions = 0;

int initSttTrans()
{
	states = NULL;
	nstates = 0;
	transitions = NULL;
	ntransitions = 0;
	return 0; 
}

int collectSttTrans()
{
	if(nstates != 0){
		gc_markOnly(states); // mark the states array itself
		for(int i = 0;  i < nstates;  ++i)
		{
			node state = states[i];
			if (state == NULL) continue; // skip null states
			gc_mark(state); //PAIR
		}
	}
	if(ntransitions != 0){
		gc_markOnly(transitions); // mark the transitions array itself
		for(int i = 0;  i < ntransitions;  ++i)
		{
			node trans = transitions[i];
			if(trans == NULL) continue; // skip null transitions
			gc_mark(trans);//PAIR
		}
	}
	return 0; 
}


void appendS0T1(node name, int pos,int type)
{
	if(type != 0 && type != 1) {
		printf("type must be 0 or 1, got %d\n", type);
		return;//error
	}
	node *lists = type == APSTATE ? states : transitions;
	int *listSize = type == APSTATE ? &nstates : &ntransitions;
	for (int i = 0;  i < *listSize;  ++i) {
		node stateName = getNode(lists[i], Pair,a);
		if (stateName == name) {
			printf("state %s already exists\n", getNode(name, Symbol,name));
			return;
		}
	}

	gc_pushRoot((void*)&name);
	switch(type){
		case APSTATE:{
			states = realloc(states, sizeof(node) * (nstates + 1));
			states[nstates] = newPair(name, newInteger(pos));
			nstates++;
			break;
		}
		case ATRANSITION:{
			transitions = realloc(transitions, sizeof(node) * (ntransitions + 1));
			transitions[ntransitions] = newPair(name, newInteger(pos));
			ntransitions++;
			break;
		}
	}
	gc_popRoots(1);
	return;
}

void setTransPos(oop prog){
	int *code = prog->IntArray.elements;
	for (int i = 0;  i < ntransitions;  ++i) {
		node trans = transitions[i];
		node transName = getNode(trans, Pair,a);
		int transPos = Integer_value(getNode(trans, Pair,b));//pos of after emitII(prog, iTRANSITION, 0);
		for (int j = 0;  j < nstates;  ++j) {
			node state = states[j];
			node stateName = getNode(state, Pair,a);
			if (stateName == transName) {
				int statePos = Integer_value(getNode(state, Pair,b));
				code[transPos-1] = statePos - transPos;//relative position
			}
		}
	}
}

#if DEBUG
#define printTYPE(OP) printf("emit %s\n", #OP)
#else
#define printTYPE(OP) ;
#endif

int emitOn(oop prog,node vars, node ast, node type);
void emitL (oop array, node object) 	  { intArray_append(array, Integer_value(object)); }
void emitI (oop array, int i     ) 	  { intArray_append(array, i); }
void emitII(oop array, int i, int j)      { emitI(array, i); emitI(array, j); }
void emitIL(oop array, int i, node object) { emitI(array, i); emitL(array, object); }
void emitIII(oop array, int i, int j, int k)
{
	emitI(array, i);
	emitI(array, j);
	emitI(array, k);
}

static int parseType(node vars, node ast)
{
	switch(getType(ast)){
		case Integer:return Integer;
		case String:return String;
		case GetVar:{
			struct RetVarFunc var = searchVariable(vars, getNode(ast, GetVar,id), NULL);
			if(var.index == -1)return -1; // variable not found
			return GET_OOP_FLAG(var.variable->Variable.type);
		}
		default:
			reportError(DEVELOPER, 0, "unknown AST type %d", getType(ast));
			return -1;
	}
	reportError(DEVELOPER, 0, "unknown AST type %d", getType(ast));
	return -1; // should never reach here
}

// 0: success
// 1: error
static int parseArray(oop prog, cnode array,cnode index,cnode type, cnode vars){
	if(index == nil)return 0;
	const int size = Integer_value(getNode(index, Pair,a));
	if(size!=getNode(array, Array,size)){
		reportError(DEVELOPER, 0, "array size mismatch: expected %d, got %d", size, getNode(array, Array,size));
		return 1;
	}
	cnode *elements = getNode(array, Array,elements);
	for(int i=0;i<size;i++){
		if(getType(elements[i]) == Array){
			if(parseArray(prog, elements[i], getNode(index, Pair,b), type, vars))return 1;
		}else {
			if(emitOn(prog, vars, elements[i], type))return 1;
		}
	}
	emitII(prog, iARRAY, size);
	return 0;
}

static int isAtomicType(node type)
{
	if(type == TYPES[Integer]) return 1;
	if(type == TYPES[Float]) return 1;
	if(type == TYPES[String]) return 1;
	if(type == TYPES[Undefined]) return 1;
	return 0;
}
static node getElementType(cnode ctx,cnode id)
{
	assert(getType(getNode(ctx, EmitContext, user_types)) == Array);
	cnode *user_types = getNode(ctx, EmitContext, user_types)->Array.elements;
	int nUserTypes = getNode(ctx, EmitContext, user_types)->Array.size;
	for(int i=0;i<nUserTypes;i++){
		cnode user_type = user_types[i]->Pair.a;
		if(user_type == id){
			return user_types[i]->Pair.b;
		}
	}
	return nil;
}

void printCode(oop code);

int emitOn(oop prog,node vars, node ast, node type)
{
	assert(getType(vars) == EmitContext);
	int ret = 0; /* ret 0 indicates success */
    switch (getType(ast)) {
		case Undefined:
		case Integer:{
			printTYPE(_Integer_);
			if(type != TYPES[Integer]){
				reportError(ERROR, 0, "type mismatch: expected %d, got Integer", GET_OOP_FLAG(type));
				return 1;
			}
			emitIL(prog, iPUSH, ast);
			return 0;
		}
		case String:{
			printTYPE(_String_);
			if(type != TYPES[String]){
				reportError(ERROR, 0, "type mismatch: expected %d, got String", GET_OOP_FLAG(type));
				return 1;
			}
			char* str = getNode(ast, String,value);
			int sLen = strlen(str);
			emitII(prog, sPUSH, sLen); // push the length of the string
			for(int i = 0; i < sLen; i++){
				int data = 0;
				for(int j = 0; j < 4; j++, i++){
					if(i < sLen){
						data |= (str[i] & 0xFF) << (j * 8);
					}else{
						data |= (0 & 0xFF) << (j * 8);
					}
				}
				intArray_append(prog, data);
				--i;
			}
			return 0;
		}
		case Array:{
			printTYPE(_Array_);
			if(isAtomicType(type))
			{
				node *elements = getNode(ast, Array,elements);
				int size = getNode(ast, Array,size);
				emitII(prog, iPUSH, size); // push the size of the array
				for (int i = 0;  i < size;  ++i) {
					if(emitOn(prog, vars, elements[i], type))return 1; // compile each element
				}
			}else{//User-defined type
				assert(getType(type) == Array);
				int arrSize = getNode(ast, Array,size);
				node *arrs = getNode(ast, Array,elements);
				int typeSize= getNode(type, Array,size);
				node *types = getNode(type, Array,elements);//Variable
				if(arrSize != typeSize){
					reportError(ERROR, 0, "element size mismatch: expected %d, got %d", typeSize, arrSize);
					return 1;
				}
				for(int i=0;i<arrSize;i++){
					node t = getNode(types[i], Variable,type);
					if(!isAtomicType(t))stop();//removeme
					if(emitOn(prog, vars, arrs[i], t))return 1;
				}
				emitII(prog, iARRAY, arrSize);//user defined type (that deal as array)
			}
			return 0; 
		}
		case Binop:{
			printTYPE(_Binop_);
			if(emitOn(prog, vars, getNode(ast, Binop,lhs), type)) return 1;
			if(emitOn(prog, vars, getNode(ast, Binop,rhs), type)) return 1;
			switch (getNode(ast, Binop,op)) {
				case NE: emitI(prog, iNE);  return 0; // emit not equal
				case EQ: emitI(prog, iEQ);  return 0;
				case LT: emitI(prog, iLT);  return 0;
				case LE: emitI(prog, iLE);  return 0;
				case GE: emitI(prog, iGE);  return 0;
				case GT: emitI(prog, iGT);  return 0;
				case ADD: emitI(prog, iADD);  return 0;
				case SUB: emitI(prog, iSUB);  return 0;
				case MUL: emitI(prog, iMUL);  return 0;
				case DIV: emitI(prog, iDIV);  return 0;
				case MOD: emitI(prog, iMOD);  return 0;
				default:break;
			}
			fatal("file %s line %d unknown Binop operator %d", __FILE__, __LINE__, getNode(ast, Binop,op));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Unyop:{
			printTYPE(_Unyop_);
			node rhs = getNode(ast, Unyop,rhs);
			switch (getNode(ast, Unyop,op)) {
				case NEG: emitII(prog,iPUSH, 0);emitOn(prog, vars, rhs, type); emitI(prog, iSUB); return 0;
				case NOT: emitII(prog,iPUSH, 0);emitOn(prog, vars, rhs, type); emitI(prog, iEQ);  return 0;
				default: break;
			}
			struct RetVarFunc var = searchVariable(vars, getNode(rhs, GetVar,id), type);
			if(var.index == -1)return 1; // variable not found
			switch(getNode(ast, Unyop,op)) {
					case BINC: if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iADD);emitII(prog, iSETVAR, var.index);if(emitOn(prog, vars, rhs, type))return 1; return 0;
					case BDEC: if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iSUB);emitII(prog, iSETVAR, var.index); if(emitOn(prog, vars, rhs, type))return 1;return 0;
					case AINC: if(emitOn(prog, vars, rhs, type))return 1;if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iADD);emitII(prog, iSETVAR, var.index); return 0;
					case ADEC: if(emitOn(prog, vars, rhs, type))return 1;if(emitOn(prog, vars, rhs, type))return 1;emitII(prog, iPUSH, 1); emitI(prog, iSUB);emitII(prog, iSETVAR, var.index); return 0;
				default: break;
			}
			fatal("file %s line %d unknown Unyop operator %d", __FILE__, __LINE__, getNode(ast, Unyop,op));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case GetVar: {
			printTYPE(_GetVar_);
			node sym = getNode(ast, GetVar,id);
			struct RetVarFunc var = searchVariable(vars, sym, type);
			if(var.index == -1)return 1; // variable found
			emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the global variable value
			return 0;
		}
		case SetVar: {
			printTYPE(_SetVar_);
			node sym = getNode(ast, SetVar,id);
			node rhs = getNode(ast, SetVar,rhs);
			node declaredType = getNode(ast, SetVar,type);
			switch(getType(rhs)){
				case UserFunc:{
					printTYPE(_Function_);
					if(sym->Symbol.value != FALSE) {
						reportError(ERROR, getNode(ast,SetVar,line), "variable %s is already defined as a function", getNode(sym, Symbol,name));
						return 1;
					}
					node params = getNode(rhs, UserFunc,parameters);
					node body = getNode(rhs, UserFunc,body);
					getNode(vars, EmitContext, local_vars) = newArray(0);
					GC_PUSH(node, closure, newClosure());
					GC_PUSH(int*, argTypes, NULL);
					while(params != nil){
						node sym = getNode(params, Params, id);
						node type = getNode(params, Params, type);
						struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, local_vars), sym, type, NULL); // insert parameter into local variables
						if(var.index == -1){
							GC_POP(argTypes);
							GC_POP(closure);
							return 1; // type error
						}
						appendNewInt(argTypes, ++(closure->Closure.nArgs), GET_OOP_FLAG(type)); // append parameter type to argument types
						params = getNode(params, Pair,b);
					}
					closure->Closure.argTypes = argTypes; // set the argument types
					closure->Closure.retType = GET_OOP_FLAG(declaredType); // set the return type
					emitII(prog, iJUMP, 0); //jump to the end of the function // TODO: once call jump 
					int jump4EndPos = prog->IntArray.size - 1; // remember the position of the jump // TODO: once call jump
					closure->Closure.pos = prog->IntArray.size; // remember the position of the closure
					sym->Symbol.value = closure; // set the closure as the value of the variable
					emitII(prog, iMKSPACE, 0); // reserve space for local variables
					int codePos = prog->IntArray.size - 1; // remember the position of the function code
					if(emitOn(prog, vars, body, declaredType)) {//declated type is the return type
						GC_POP(closure);
						return 1; // compile function body
					}
					prog->IntArray.elements[codePos] = getNode(getNode(vars, EmitContext, local_vars), Array, size); // set the size of local variables
					prog->IntArray.elements[jump4EndPos] = (prog->IntArray.size - 1) - jump4EndPos; // set the jump position to the end of the function // TODO: once call jump
					GC_POP(closure);
					getNode(vars, EmitContext, local_vars) = NULL; // clear local variables
					return 0;
				}
				case Call:{
					printTYPE(_Call_);

					node func = getNode(rhs, Call, function)->GetVar.id->Symbol.value;

					if(func ==  FALSE){
						reportError(ERROR, 
							getNode(ast,SetVar,line), "variable %s is not defined", 
							getNode(rhs, Call,function)->GetVar.id->Symbol.name);
						return 1;
					}
					switch(getType(func)){
						case EventObject:{//var t = timer();
							printTYPE(__EventObject__);
							node args = getNode(rhs, Call, arguments);
							int index = getNode(func, EventObject, index); // get the index of the event object
							struct EventObjectTable eo = EventObjectTable[index];
							node* funcs = getNode(func, EventObject, funcs);
#ifndef NDEBUG
							for(int i = 0; i < eo.nFuncs; i++){
								if(funcs[i] == NULL){
									reportError(DEVELOPER, getNode(ast,SetVar,line), "event object %s function %d is not defined", getNode(sym, Symbol,name), i);
									return 1;
								}
							}
#endif
							int argsCount = 0;
							while(args != nil){
								if(argsCount >= eo.nArgs){
									reportError(ERROR, 
										getNode(ast,SetVar,line), "event object %s expects %d arguments, but got more", 
										getNode(sym, Symbol,name), eo.nArgs);
									printf("index %d eo.nArgs %d argsCount %d\n", index, eo.nArgs, argsCount);
									return 1;
								}
								node arg = getNode(args, Args, value);
								if(emitOn(prog, vars, arg, TYPES[eo.argTypes[argsCount]]))return 1; // compile argument
								args = getNode(args, Args, next);
								argsCount++;
							}
							if(eo.nArgs != argsCount){
								reportError(ERROR, getNode(ast,Call,line), "event object %s expects %d arguments, but got %d", getNode(sym, Symbol,name), eo.nArgs, argsCount);
								return 1;
							}
							//initialize the event handler opcode
							emitII(prog, eCALL, index); // call the event object and push it to top of the stack
							switch(getNode(ast, SetVar,scope)) {
								case SCOPE_LOCAL:{
									reportError(ERROR, getNode(ast,SetVar,line), "cannot define event object %s as local variable", getNode(sym, Symbol,name));
									return 1;
								}
								case SCOPE_STATE_LOCAL:{
									struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, state_vars),sym, nil/*Undefined*/, func);
									emitII(prog, iSETSTATEVAR, var.index);
									return 0;
								}
								case SCOPE_GLOBAL:{
									struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, global_vars),sym, nil/*Undefined*/, func);
									emitII(prog, iSETGLOBALVAR, var.index);
									return 0;
								}
							}
							reportError(DEVELOPER,getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
							return 1;
						}
						default:{
							fatal("file %s line %d emitOn: unknown Call type %d", __FILE__, __LINE__, getType(func));
							reportError(DEVELOPER, getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
							return 1;
						}
					}
				}
				case Array:{//UserType
					if(isAtomicType(declaredType)){
						reportError(DEVELOPER, getNode(ast,SetVar,line), "variable %s declared type cannot be atomic type", getNode(sym, Symbol,name));
						return 1;
					}
					assert(getType(declaredType) == Symbol);
					node userTypes = vars->EmitContext.user_types;
					assert(getType(userTypes) == Array);
					int size = userTypes->Array.size;
					node *ele = userTypes->Array.elements;
					for(int i=0;i<size;i++){//search for user type
						if(declaredType == getNode(ele[i], Pair, a)){
							declaredType = getNode(ele[i], Pair, b);
							break;
						}
						if(i==size-1){
							reportError(DEVELOPER, getNode(ast,SetVar,line), "unknown user type %s", getNode(declaredType, Symbol,name));
							return 1;
						}
					}
				}
				case Integer:
				case String:{
					if(declaredType == nil){
						declaredType = TYPES[parseType(vars, rhs)];
					}
				}
				case GetArray:
				case GetField:
				case GetVar:
				case Unyop:
				case Binop:{
					int scope = getNode(ast, SetVar,scope);
					switch(scope) {
						case SCOPE_LOCAL:{
							printf("defining local variable %s\n", getNode(sym, Symbol,name));
							if(emitOn(prog,vars, rhs, declaredType)) return 1;
							struct RetVarFunc var = insertVariable(vars, sym, declaredType);
							if(var.index == -1)return 1; //type error
							emitII(prog, iSETGLOBALVAR + var.scope, var.index);
							return 0;
						}
						case SCOPE_STATE_LOCAL:{
							printf("defining state variable %s\n", getNode(sym, Symbol,name));
							if(emitOn(prog,vars, rhs, declaredType)) return 1;
							struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, state_vars),sym, declaredType, NULL);
							emitII(prog, iSETSTATEVAR, var.index);
							return 0;
						}
						case SCOPE_GLOBAL:{
							printf("defining global variable %s\n", getNode(sym, Symbol,name));
							if(emitOn(prog,vars, rhs, declaredType)) return 1;
							struct RetVarFunc var = appendVariable(getNode(vars, EmitContext, global_vars),sym, declaredType, NULL);
							emitII(prog, iSETGLOBALVAR, var.index);
							return 0;
						}
					}
					fatal("file %s line %d emitOn: unknown SetVar scope %d", __FILE__, __LINE__, scope);
					reportError(DEVELOPER,getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
				default:{
					fatal("file %s line %d emitOn: unknown SetVar type %d", __FILE__, __LINE__, getType(rhs));
					reportError(DEVELOPER, getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d emitOn: unknown SetVar type %d", __FILE__, __LINE__, getType(rhs));
			reportError(DEVELOPER, getNode(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
			return 0;
		}
		case GetArray:{
			printTYPE(_GetArray_);
			node array = getNode(ast, GetArray,array);//Array
			node index = getNode(ast, GetArray,index);//Pair
			struct RetVarFunc var = searchVariable(vars, array->GetVar.id, type);
			if(var.index == -1)return 1; // variable not found
			emitII(prog, iGETGLOBALVAR + var.scope, var.index); 
			while(index != nil){
				node idx = getNode(index, Pair, a);
				if(emitOn(prog, vars, idx, TYPES[Integer]))return 1;					
				index = getNode(index, Pair, b);
				emitI(prog, iGETARRAY);
			}
			printCode(prog);
			stop();
			return 0;
		}
		case SetArray:{
			printTYPE(_SetArray_);
			node setType = getNode(ast, SetArray,type);
			node array = getNode(ast, SetArray,array);//Array
			node index = getNode(ast, SetArray,index);//Pair
			node value = getNode(ast, SetArray,value);//Id
#ifdef DEBUG
			assert(getType(index) == Pair);
			assert(getType(value) == GetVar);
#endif
			int scope = getNode(ast, SetArray,scope);
			if(scope == SCOPE_GLOBAL && setType == nil){
				reportError(ERROR, getNode(ast,SetArray,line), "global variable %s must have a declared type", getNode(value, GetVar,id)->Symbol.name);
				return 1;
			}
			if(setType==nil && getType(value)==Array){
				reportError(ERROR, getNode(ast,SetArray,line), "array variable %s must have a declared type", getNode(value, GetVar,id)->Symbol.name);
				return 1;
			}
			if(setType==nil){
				setType = TYPES[parseType(vars, value)];
				struct RetVarFunc var = searchVariable(vars, value->GetVar.id, setType);
				if(var.index == -1)return 1; // variable not found
				emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the global variable value
				while(index != nil){
					node idx = getNode(index, Pair, a);
					if(emitOn(prog, vars, idx, TYPES[Integer]))return 1;					
					index = getNode(index, Pair, b);
					if(index==nil){
						if(emitOn(prog, vars, array, setType))return 1;
						emitI(prog, iSETARRAY);
						return 0;
					}else{
						emitI(prog, iGETARRAY);
					}
				}
				reportError(DEVELOPER, getNode(ast,SetArray,line), "please contact %s", DEVELOPER_EMAIL);
				return 1;
			}else{// setType is given
				if(parseArray(prog, array, index, setType, vars))return 1;
				struct RetVarFunc var;
				switch(scope) {
					case SCOPE_LOCAL:{
						var = insertVariable(vars, value->GetVar.id, setType);
						if(var.index == -1)return 1; //type error
						emitII(prog, iSETGLOBALVAR, var.index);
						break;
					}
					case SCOPE_STATE_LOCAL:{
						var = appendVariable(getNode(vars, EmitContext, state_vars), value->GetVar.id, setType, NULL);
						emitII(prog, iSETSTATEVAR, var.index);
						break;
					}
					case SCOPE_GLOBAL:{
						var = appendVariable(getNode(vars, EmitContext, global_vars), value->GetVar.id, setType, NULL);
						emitII(prog, iSETGLOBALVAR, var.index);
						break;
					}
				}
				node variable = var.variable;
				int dimIndex = 0;
				while(index != nil){
					node idx = getNode(index, Pair, a);
					variable->Variable.dim[dimIndex++] = Integer_value(idx);
					index = getNode(index, Pair, b);
				}
				return 0;
			}
			reportError(DEVELOPER, getNode(ast,SetArray,line), "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case GetField:{
			printTYPE(_GetField_);
			// id->field
			node id = getNode(ast, GetField, id);
			node field = getNode(ast, GetField, field);
	
			struct RetVarFunc var = searchVariable(vars, id, NULL);//if error happens, `type` is set to NULL to skip type checking
			if(var.index == -1)return 1; // variable not found
			 node varType = getNode(var.variable,Variable, type);
			if(isAtomicType(varType)){
				reportError(ERROR, getNode(ast,GetField,line), "variable %s is of atomic type and has no fields", getNode(id, Symbol,name));
				return 1;
			}
			switch(getType(varType))
			{
				case Undefined:{//Event Object
					node value = var.variable->Variable.value;
					int eoIndex = getNode(value, EventObject,index);
					const node * const funcs = getNode(value, EventObject,funcs);
					//search field in eo.funcs
					int fieldIndex = 0;
					char* funcName = getNode(getNode(field, Call, function), GetVar, id)->Symbol.name;//Stopied at this line
					while(funcs[fieldIndex] != nil){
						if(strcmp(funcs[fieldIndex]->Symbol.name, funcName) == 0){
							emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the event object
							//chat.send(msg);
							field->Call.function = funcs[fieldIndex];
							emitOn(prog, vars, field, type);
							return 0;
						}
						fieldIndex++;
					}
					reportError(ERROR, getNode(ast,GetField,line), "event object %s has no field %s", getNode(id, Symbol,name), getNode(field, Symbol,name));
					return 1;
				}
				case Array:{ //User-defined type
					int fieldSize = getNode(varType, Array,size);
					node *fields = getNode(varType, Array,elements);
					int isGetField = getType(field) == GetField ? 1 : 0;
					node fieldId = isGetField ? getNode(field, GetField, id) : getNode(field, GetVar, id);
					emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the user-defined type variable
					for(int i=0; i<fieldSize; i++){
						if(fieldId == getNode(fields[i], Variable, id)){
							emitII(prog, iPUSH, i); // push the field index
							emitI(prog, iGETARRAY);
							if(isGetField){//agent.pos.x
								i = -1;
								id = getNode(field, GetField, id);
								field = getNode(field, GetField, field);
								var = searchVariable(vars, id, NULL);//if error happens, `type` is set to NULL to skip type checking
								if(var.index == -1)return 1; // variable not found
			 					varType = getNode(var.variable,Variable, type);
								fieldSize = getNode(varType, Array,size);
								fields = getNode(varType, Array,elements);
								i = -1; //restart searching
								continue;
							}else{
								return 0;
							}
						}
						if(i==fieldSize-1){
							reportError(ERROR, getNode(ast,GetField,line), "user-defined type %s has no field %s", getNode(id, Symbol,name), getNode(fieldId, Symbol,name));
							return 1;
						}
					}
					return 0;
				}
				default:{
					fatal("file %s line %d emitOn: GetField is not supported for type %d yet", __FILE__, __LINE__, getType(varType));
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d emitOn: GetField is not supported yet", __FILE__, __LINE__);
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case SetType:{
			printTYPE(_SetType_);
			node id = getNode(ast, SetType,id);
			node rhs = getNode(ast, SetType,rhs);
			assert(getType(rhs) == Array);// variable array typesa
			addUserType(vars, newPair(id, rhs)); 
			return 0;
		}
		case Call:{
			//Standard library functions / user defined functions
			printTYPE(_Call__);
			node id   = getNode(ast, Call, function);
			if(getType(id) == GetVar)id = id->GetVar.id;
			node args = getNode(ast, Call,arguments);

			node func = getNode(id, Symbol, value); // get the function from the variable;

			switch(getType(func)){
				case EventObject:{
					reportError(DEVELOPER, getNode(ast,Call,line), "event object call is not supported yet");
					return -1;
				}
				case EventH:{// event handler
					printTYPE(_EventH_);
					int index = getNode(func, EventH,index);// get the index of the event handler
					struct EventTable eh = EventTable[index];
					int nArgs = eh.nArgs;
					char *argTypes = eh.argTypes;

					int argsCount = 0;
					while(args != nil){
						node arg = getNode(args, Eparams, id);
						if(emitOn(prog, vars, arg, TYPES[argTypes[argsCount]]))return 1; // compile argument
						args = getNode(args, Eparams, next);
						argsCount++;
					}
					if(nArgs != argsCount){
						reportError(ERROR, getNode(ast,Call,line), "event %s expects %d arguments, but got %d", getNode(func, Symbol,name), nArgs, argsCount);
						return 1;
					}
					emitIII(prog, iPCALL,  index, nArgs); // call the event handler
					return 0; 
				}
				case StdFunc:{// standard function
					printTYPE(_StdFunc_);
					int funcIndex = getNode(func, StdFunc, index); // get the index of the standard function
					int argsCount = 0;
					struct StdFuncTable func = StdFuncTable[funcIndex];
					while(args != nil){
						node arg = getNode(args, Args, value);
						if(emitOn(prog, vars, arg, TYPES[func.argTypes[argsCount]]))return 1; // compile argument
						args = getNode(args, Args, next);
						argsCount++;
					}
					if(func.nArgs != argsCount){
						reportError(ERROR, getNode(ast,Call,line), "standard function %s expects %d arguments, but got %d", getNode(id, Symbol,name), func.nArgs, argsCount);
						return 1;
					}

					emitII(prog, iSCALL, funcIndex); // call the standard function
					//NOTE: we don't need to clean the stack after the call, because the standard function will do it
					//      BUT we need to check the return value (not implemented yet)
					//emitII(prog, iCLEAN, nReturns); // clean the stack after the call
					return 0; 
				}
				case Closure:{ // user defined function
					printTYPE(_Closure_);
					int nArgs = getNode(func, Closure,nArgs);
					int *argTypes = getNode(func, Closure, argTypes);
					int pos   = getNode(func, Closure,pos);
					int argsCount = 0;
					while(args != nil){
						node arg = getNode(args, Args, value);
						if(emitOn(prog, vars, arg, TYPES[argTypes[argsCount]]))return 1; // compile argument
						args = getNode(args, Args, next);
						argsCount++;
					}
					if(nArgs != argsCount){
						reportError(ERROR, getNode(ast, Call, line), "function %s expects %d arguments, but got %d", getNode(id, Symbol,name), nArgs, argsCount);
						return 1; 
					}
					emitIII(prog, iUCALL, 0, nArgs); // call the function
					int iCallPos = prog->IntArray.size - 2; // remember the position of the call
					prog->IntArray.elements[iCallPos] = pos - (prog->IntArray.size) ; // set the jump position to the function code
					emitII(prog, iCLEAN, nArgs); // clean the stack after the call
					return 0; 
				}
				default:{
					fatal("file %s line %d call function with type %d",__FILE__,__LINE__,getType(func));
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d unknown Call type %d", __FILE__, __LINE__, getType(id));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Pair:{
			printTYPE(_Pair_);
			node a = getNode(ast, Pair,a);
			node b = getNode(ast, Pair,b);
			fatal("file %s line %d emitOn: Pair is not supported yet", __FILE__, __LINE__);
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Print:{
			printTYPE(_Print_);
			node args = getNode(ast, Print,arguments);
			node local_vars = getNode(vars, EmitContext, local_vars);
			int nArgs = 0;
			while (args != nil) {
				node value = getNode(args, Args, value);
				int argType = parseType(vars, value);
				if(argType == -1) return 1; // type error
				if(emitOn(prog, vars, value, TYPES[argType])) return 1; // compile argument
				switch(argType){
					case Integer: emitI(prog, iPRINT); break;
					case Float:   emitI(prog, fPRINT); break;
					case String:  emitI(prog, sPRINT); break;
					default:
						reportError(ERROR, getNode(ast, Print, line), "print statement does not support type %d", argType);
						return 1;
				}
				args = getNode(args, Args, next);
				nArgs++;
			}
			emitI(prog, flashPRINT); // print the result
			return 0;
		}
		case If:{
			printTYPE(_If_);
			//NEXT-TODO
			int variablesSize = getNode(vars, EmitContext, local_vars) ? getNode(getNode(vars, EmitContext, local_vars), Array, size) : 0; // remember the size of variables
			node condition = getNode(ast, If,condition);
			node statement1 = getNode(ast, If,statement1);
			node statement2 = getNode(ast, If,statement2);

			if(emitOn(prog, vars, condition,TYPES[Integer])) return 1; // compile condition

			emitII(prog, iJUMPIF, 0); // emit jump if condition is TRUE
			int jumpPos = prog->IntArray.size-1; // remember position for jump

			if(emitOn(prog, vars, statement1,type)) return 1; // compile first statement
			if (statement2 !=  FALSE) {
				emitII(prog, iJUMP, 0);
				int jumpPos2 = prog->IntArray.size-1; // remember position for second jump
				if(emitOn(prog, vars, statement2,type)) return 1; // compile second statement
				prog->IntArray.elements[jumpPos] = jumpPos2 - jumpPos; // set jump position for first jump
				prog->IntArray.elements[jumpPos2] = (prog->IntArray.size - 1) - jumpPos2; // set jump position
			} else {
				prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position for first jump	
			}
			//NEXT-TODO
			discardVariables(getNode(vars, EmitContext, local_vars), variablesSize); // discard variables
			return 0;
		}
		case Loop:{
			printTYPE(_Looop_);
			node initialization = getNode(ast, Loop,initialization);
			node condition      = getNode(ast, Loop,condition);
			node iteration      = getNode(ast, Loop,iteration);
			node statement      = getNode(ast, Loop,statement);
			//NEXT-TODO
			int variablesSize = getNode(vars, EmitContext, local_vars) ? getNode(getNode(vars, EmitContext, local_vars), Array, size) : 0; // remember the size of variables

			if (initialization !=  FALSE) {
				if(emitOn(prog, vars, initialization, type)) return 1; // compile initialization
			}
			int stLoopPos = prog->IntArray.size; // remember the position of the loop start
			int jumpPos = 0; // position for the jump if condition is TRUE
			if(condition !=  FALSE) {
				if(emitOn(prog, vars, condition, TYPES[Integer])) return 1; // compile condition
				emitII(prog, iJUMPIF, 0); // emit jump if condition is TRUE
				jumpPos = prog->IntArray.size - 1; // remember position for jump
			}

			if(emitOn(prog, vars, statement, type)) return 1; // compile statement

			if (iteration !=  FALSE) {
				if(emitOn(prog, vars, iteration, type)) return 1; // compile iteration
			}
			emitII(prog, iJUMP, 0); // jump to the beginning of the loop
			prog->IntArray.elements[prog->IntArray.size - 1] = stLoopPos - prog->IntArray.size; // set jump position to the start of the loop
			if(condition !=  FALSE) {
				prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position for condition
			}
			//NEXT-TODO
			discardVariables(getNode(vars, EmitContext, local_vars), variablesSize); // discard variables
			return 0;
		}
		case Return:{
			printTYPE(_Return_);
			node value = getNode(ast, Return,value);
			if (value !=  FALSE) {
				if(emitOn(prog, vars, value, type)) return 1; // compile return value
			} else {
				emitII(prog, iPUSH, 0); // return 0 if no value is specified
			}
			emitI(prog, iRETURN); // emit return instruction
			return 0;
		}
		case Block:{
			printTYPE(_Block_);
			node *statements = ast->Block.statements;
			int nStatements = ast->Block.size;
			for (int i = 0;  i < nStatements;  ++i) {
				if(emitOn(prog, vars, statements[i], type)) return 1; // compile each statement
			}
			return 0;
		}
		case Transition:{
			printTYPE(_Transition_);
			node id = getNode(ast, Transition,id);
			emitII(prog, iTRANSITION, 0);
			/* WARN: 実際に値を入れるときは、-1して値を挿入する */
			appendS0T1(id, prog->IntArray.size, ATRANSITION); // append state to states
			return 0;
		}
		case State:{
			printTYPE(_State_);
			pushUserTypeIndex(vars);
			node id = getNode(ast, State,id);
			node params = getNode(ast, State,parameters);
			node events = getNode(ast, State,events);
			dprintf("State: %s\n", getNode(id, Symbol,name));

			// compile events  
			node *eventList = events->Block.statements;
			int nElements = 0;
			int elements[events->Block.size]; // collect elements: 0: empty, other: number of event handlers block
			int nEventHandlers = 0;
			node preid =  FALSE;
			emitII(prog, iJUMP, 0);
			int jumpPos = prog->IntArray.size - 1; // remember the position of the jump
			getNode(vars, EmitContext, state_vars) = newArray(0); // set state variables for the state

			for (int i = 0;  i < getNode(events, Block, size);  ++i) {
				if(eventList[i] == NULL){
					fatal("%s %d ERROR: eventList[%d] is NULL\n", __FILE__, __LINE__, i);
					reportError(DEVELOPER, 0,"please contact %s", DEVELOPER_EMAIL);
					popUserTypeIndex(vars);
					return 1;
				}
				if((getNode(eventList[i], Event,id) == entryEH) || (getNode(eventList[i], Event,id) == exitEH)){
					dprintf("entryEH or exitEH\n");
					elements[nElements++] = 0; // collect empty events
					continue;
				}
				if(getType(eventList[i])!=Event){
					fatal("file %s line %d emitOn: eventList[%d] is not an Event", __FILE__, __LINE__, i);
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					popUserTypeIndex(vars);
					return 1;
				}
				else if(getNode(eventList[i], Event,id) != preid) {
					dprintf("new event\n");
					nEventHandlers++;
					preid = getNode(eventList[i], Event,id);
					elements[nElements]=1; // collect unique events
				}else if(getNode(eventList[i], Event,id) == preid){
					dprintf("same event\n");
					elements[nElements]++;
				}
				dprintf("eventList[%d]: %s\n", i, getNode(eventList[i], Event,id)->Symbol.name);
				if(emitOn(prog, vars, eventList[i], type)) {
					popUserTypeIndex(vars);
					return 1; // compile each event
				}
			}
			dprintf("Finished collecting events\n");

			prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position to the end of the events
			// state initialization
			appendS0T1(id, prog->IntArray.size, APSTATE); // append state to states
			//entryEH
			if(getNode(eventList[0], Event,id) == entryEH){
				if(emitOn(prog, vars, eventList[0], type)) {
					popUserTypeIndex(vars);
					return 1; // compile entryEH
				}
			}
			emitII(prog, iSETSTATE, nEventHandlers); // set the number of events and position
			for(int i =0 ,  ehi =0;  i < getNode(events, Block, size);  ++i) {
				if(elements[i] == 0){ ehi++;continue;} // skip empty events
				node id = getNode(eventList[i], Event,id);

				if(getType(id) == GetField)//chat.received()
				{
					struct RetVarFunc var = searchVariable(vars, getNode(id, GetField, id), type);
					if(var.index == -1){
						popUserTypeIndex(vars);
						return 1; // variable not found
					}
					emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the event object variable value
					node *funcs = getNode(var.variable->Variable.value, EventObject, funcs);
					char * fieldName = getNode(getNode(id, GetField, field),Symbol,name);
					int _i = 0;
					while(funcs[_i] != NULL){
						if(strcmp(getNode(funcs[_i], Symbol,name), fieldName) == 0){
							id = funcs[_i];
							break;
						}
						_i++;
					}
				}

				node eh = getNode(id,Symbol,value);

				int eventID = getNode(eh, EventH, index); // get event ID
				emitIII(prog, iSETEVENT, eventID, elements[ehi]); // emit SETEVENT instruction
				for(int j = 0; j < elements[ehi]; ++j) {
					node event = eventList[i++];
					node posPair = getNode(event, Event,block);
					emitIII(prog, iSETPROCESS ,Integer_value(getNode(posPair,Pair,a))/*action*/,Integer_value(getNode(posPair,Pair,b))/*condition*/); // set the position of the event handler)
				}
				ehi++; // increment event handler index
			}
			emitI(prog, iIMPL);
			// exitEH
			for(int i = 0; i < getNode(events, Block, size); i++){
				if(getNode(eventList[i], Event,id) == exitEH){ 
					if(emitOn(prog, vars, eventList[i],type)) {
						popUserTypeIndex(vars);
						return 1; // compile exitEH
					}
				}
				if(i == 1)break;//0: entryEH, 1: exitEH ...
			}
			//NEXT-TODO
			// discardVariables(vars->EmitContext.state_vars, 0); // discard variables
			getNode(vars, EmitContext, state_vars) = NULL; // clear state variables
			emitII(prog, iEOS, 0); // emit EOS instruction to mark the end of the state
			popUserTypeIndex(vars);
			return 0;
		}
		case Event:{
			printTYPE(_Event_);
			node id = getNode(ast, Event,id);
			pushUserTypeIndex(vars); // push user type index stack
			if(getType(id) == GetField)
			{
				node parent = getNode(id, GetField, id); // get the variable from the GetField
				struct RetVarFunc var = searchVariable(vars, parent, type);
				if(var.index == -1){
					reportError(ERROR, getNode(ast,Event,line), "variable %s is not defined", getNode(parent, Symbol,name));
					popUserTypeIndex(vars);
					return 1;
				}
				int parentIndex = getNode(var.variable->Variable.value, EventObject, index);
				node* funcs = getNode(var.variable->Variable.value, EventObject, funcs);
				struct EventObjectTable eo = EventObjectTable[parentIndex];
				char* fieldName = getNode(getNode(id, GetField, field),Symbol,name);
				id = NULL;
				for(int i = 0; i < eo.nFuncs; ++i){
					if(strcmp(getNode(funcs[i], Symbol,name), fieldName) == 0){
						id = funcs[i];
						break;
					}
				}
				if(id == NULL){
					reportError(ERROR, getNode(ast,Event,line), "event object %s has no event %s", getNode(parent, Symbol,name), fieldName);
					popUserTypeIndex(vars);
					return 1;
				}
				emitII(prog, iGETGLOBALVAR + var.scope, var.index); // get the event object variable value
			}
			node params = getNode(ast, Event, parameters);
			node block = getNode(ast, Event, block);
			node eh = getNode(id,Symbol,value);
			if(getType(eh) != EventH) {
				fatal("file %s line %d emitOn: event %s() is not an EventH", __FILE__, __LINE__, getNode(id, Symbol,name));
				reportError(DEVELOPER, getNode(ast,Event,line), "please contact %s", DEVELOPER_EMAIL);
				popUserTypeIndex(vars);
				return 1;
			}
			getNode(vars, EmitContext, local_vars) = newArray(0); // set local variables for the event
			int nArgs = EventTable[eh->EventH.index].nArgs;
			int paramSize =0;
			int cPos      = 0;

			while(params != nil) {//a:id-b:cond
				printf("\t appendVarialbe\n");
				struct RetVarFunc var =  appendVariable(
					getNode(vars, EmitContext, local_vars), 
					getNode(params, Eparams, id), 
					getNode(params, Eparams, type),
					NULL); // add parameter to local variables
				if(var.index == -1){
					popUserTypeIndex(vars);
					return 1; // type error
				}
				printf("\t finished appendVarialbe\n");
				if(getNode(params, Eparams, cond) !=  FALSE){
					if(cPos==0)cPos = prog->IntArray.size; // remember the position of the condition
					if(emitOn(prog, vars, getNode(params, Eparams, cond), getNode(params, Eparams, type))) return 1; // compile condition if exists
					emitI(prog,iJUDGE); // emit JUDGE instruction
				}
				params = getNode(params, Eparams, next);
				paramSize++;
			}
			if(cPos != 0){
				emitI(prog, iEOC); // emit EOC instruction if condition exists
			}
			if(paramSize != nArgs) {
				reportError(ERROR, getNode(ast,Event,line), "event %s has %d parameters, but expected %d", getNode(id, Symbol,name), paramSize, nArgs);
				getNode(vars, EmitContext, local_vars) = NULL; // clear local variables
				popUserTypeIndex(vars);
				return 1;
			}
			int aPos = prog->IntArray.size; // remember the position of the event handler
			emitII(prog, iMKSPACE, 0); // reserve space for local variables
			int mkspacepos = prog->IntArray.size - 1; // remember the position of MKSPACE

			if(emitOn(prog, vars, block, type))return 1; // compile block
			emitII(prog, iEOE, getNode(getNode(vars, EmitContext, local_vars), Array, size)); // emit EOE instruction to mark the end of the event handler

			prog->IntArray.elements[mkspacepos] = getNode(getNode(vars, EmitContext, local_vars), Array, size); // store number of local variables
			getNode(vars, EmitContext, local_vars) = NULL; // clear local variables
			GC_PUSH(node,apos, newInteger(aPos));
			GC_PUSH(node,cpos,newInteger(cPos));
/* BE CAREFUL */
			ast->Event.block = newPair(apos,cpos); // set the position of the event handler
			GC_POP(cpos);
			GC_POP(apos);
			popUserTypeIndex(vars); // pop user type index stack
			return 0;
		}
		default:break;
	}
    fatal("file %s line %d: emitOn: unimplemented emitter for type %d", __FILE__, __LINE__, getType(ast));
	reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
	popUserTypeIndex(vars);
	return 1;
}

void printCode(oop code){
	for (int i = 0; i < code->IntArray.size; ++i) {
		int op = code->IntArray.elements[i];
		const char *inst = "UNKNOWN";
		switch (op) {
			case iHALT:
				inst = "HALT";
				printf("%03d: %-10s\n", i, inst);
				break;
			case iPUSH:
				inst = "LOAD";
				int value = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, value);
				break;
			case sPUSH:{
				inst = "LOADS";
				int sLen = code->IntArray.elements[++i];
				char sValue[sLen+1];
				memcpy(sValue, &code->IntArray.elements[++i], sLen);
				sValue[sLen] = '\0';
				printf("%03d: %-10s \"%s\"\n", i-2, inst, sValue);
				if(sLen % sizeof(int) == 0)
					i += sLen/sizeof(int) - 1;
				else
					i += sLen/sizeof(int);
				break;
			}
			case iARRAY: {
				inst = "ARRAY";
				int nElements = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nElements);
				break;
			}
			case iSETARRAY: {
				inst = "SETARRAY";
				printf("%03d: %-10s\n", i, inst);
				break;
			}
			case iGETARRAY: {
				inst = "GETARRAY";
				printf("%03d: %-10s\n", i, inst);
				break;
			}
			case iGT:   inst = "GT"; goto simple;
			case iGE:   inst = "GE"; goto simple;
			case iEQ:   inst = "EQ"; goto simple;
			case iNE:   inst = "NE"; goto simple;
			case iLE:   inst = "LE"; goto simple;
			case iLT:   inst = "LT"; goto simple;
			case iADD:  inst = "ADD"; goto simple;
			case iSUB:  inst = "SUB"; goto simple;
			case iMUL:  inst = "MUL"; goto simple;
			case iDIV:  inst = "DIV"; goto simple;
			case iMOD:  inst = "MOD"; goto simple;
			case iJUDGE:inst = "JUDGE"; goto simple;
			case iEOC:  inst = "EOC"; goto simple;
			case iRETURN: inst = "RETURN"; goto simple;
simple:
				printf("%03d: %-10s\n", i, inst);
				break;
			case iEOE:{
				inst = "EOE";
				int nVariables = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nVariables);
				break;
			}
			case iEOS:{
				inst = "EOS";
				int nVariables = code->IntArray.elements[++i];// FIXME: this is not correct instruction
				printf("%03d: %-10s %03d\n", i-1, inst, nVariables);
				break;
			}
			case iGETVAR:
				inst = "GETVAR";
				int varIndex = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, varIndex);
				break;
			case iSETVAR:
				inst = "SETVAR";
				int varIndexSet = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, varIndexSet);
				break;
			case iGETSTATEVAR:
				inst = "GETSTATEVAR";
				int sVarIndex = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, sVarIndex);
				break;
			case iSETSTATEVAR:
				inst = "SETSTATEVAR";
				int sVarIndexSet = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, sVarIndexSet);
				break;
			case iGETGLOBALVAR:
				inst = "GETGLOBALVAR";
				int gVarIndex = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, gVarIndex);
				break;
			case iSETGLOBALVAR:
				inst = "SETGLOBALVAR";
				int gVarIndexSet = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, gVarIndexSet);
				break;
			case iMKSPACE:
				inst = "MKSPACE";
				int space = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, space);
				break;
			case aPRINT: {
				inst = "aPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case iPRINT: {
				inst = "PRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case fPRINT: {
				inst = "fPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case sPRINT: {
				inst = "sPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case flashPRINT: {
				inst = "flashPRINT";
				printf("%03d: %-10s\n", i-1, inst);
				break;
			}
			case iJUMP: {
				inst = "JUMP";
				int offset = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d)\n", i-1, inst, offset, offset + (i+1));
				break;
			}
			case iJUMPIF: {
				inst = "JUMPIF";
				int offset = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d)\n", i-1, inst, offset, offset + (i+1));
				break;
			}
			case iCLEAN: {
				inst = "CLEAN";
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nArgs);
				break;
			}
			case iPCALL: {
				inst = "PCALL";
				int index = code->IntArray.elements[++i];
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d) %03d\n", i-2, inst, index, index + (i+1), nArgs);
				break;
			}
			case iSCALL: {
				inst = "SCALL";
				int index = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, index);
				break;
			}
			case iUCALL: {
				inst = "CALL";
				int pos = code->IntArray.elements[++i];
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%3d) %03d\n", i-2, inst, pos, (i+1)+pos, nArgs);
				break;
			}
			case eCALL:{
				inst = "eCall";
				int index = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, index);
				break;
			}
			case iTRANSITION: {
				inst = "TRANSITION";
				int pos = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d(%d)\n", i-1, inst, pos,(i+1)+pos);
				break;
			}
			case iSETSTATE: {
				inst = "SETSTATE";
				int nEvents = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nEvents);
				break;
			}
			case iSETEVENT: {
				inst = "SETEVENT";
				int eventID = code->IntArray.elements[++i];
				int nHandlers = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d %3d\n", i-2, inst, eventID, nHandlers);
				break;
			}
			case iSETPROCESS: {
				inst = "SETPROCESS";
				int apos = code->IntArray.elements[++i];
				int cpos = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d %3d\n", i-2, inst, apos, cpos);
				break;
			}
			case iIMPL:
				inst = "IMPL";
				printf("%03d: %-10s\n", i, inst);
				break;
			default:
				printf("%03d: %-10s %03d\n", i, inst, op);
		}
	}
	printf("\n");
}


oop compile()
{
	printf("compiling...\n");
    GC_PUSH(oop, prog, intArray_init()); // create a new program code
	emitII(prog, iMKSPACE, 0); // reserve space for local variables
#if DEBUG
int roots = gc_ctx.nroots;
#endif 
	GC_PUSH(node, vars, newEmitContext()); // If something goes wrong in the web parser, move to before changing GC roots
	// compile the AST into the program code
	int line = 1;
	assert(getType(vars) == EmitContext);
	while(yyparse()){
		if (ISTAG_FLAG(result)) {
			break;
		}
		printf("compiling statement %d\n", line);
		if(emitOn(prog, vars, result, NULL))return NULL;
		result = parserRetFlags[PARSER_READY];
		printf("compiled statement %d [size %4d]\n",++line, (prog->IntArray.size)*4);
	}
	if(result == parserRetFlags[PARSER_FINISH]){
		printf("compilation finished\n");
		emitI (prog, iHALT); // end of program
		printf("total variable size %d\n", getNode(vars, EmitContext, global_vars)->Array.size);
		prog->IntArray.elements[1] = getNode(vars, EmitContext, global_vars)->Array.size; // store number of variables
		setTransPos(prog); // set transition positions
	}else if(result == parserRetFlags[PARSER_ERROR]){
		printf("compilation error\n");
		GC_POP(vars); // pop context variables from GC roots
		return NULL;
	}
	GC_POP(vars); // pop context variables from GC roots
#if DEBUG
	if(roots != gc_ctx.nroots){
		fatal("file %s line %d: memory leak: roots before compile %d, after compile %d", __FILE__, __LINE__, roots, gc_ctx.nroots);
		reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
		return NULL;
	}
#endif
	GC_POP(prog); // pop program code from GC roots

	printf("\ncompile finished, %d statements, code size %d bytes\n\n", line, 4 * prog->IntArray.size);
    return prog;
}

/*====================== MSGC =====================*/

/* ================================================*/

/*=============== DEFAULT MARKER ==============*/

/* ==================================*/
#ifdef WEBSHICA
void collectWeb(void);
#endif

void markEmpty(void* ptr){ return;}
void collectEmpty(void){ 
#ifdef WEBSHICA
	collectWeb();
#endif
	return;
}

/*===============COMPILE MARKER=============*/

/* ==================================*/



void markObject(node obj){
	switch(getType(obj)){
case Integer :{return;}
case String :{
	if (obj->String.value) {
		gc_markOnly(obj->String.value);
	}
	return;
}
case Symbol  :
{
	if (obj->Symbol.value) {
		gc_mark(obj->Symbol.value);
	}
	return;
}
case Pair    :
{
	if (obj->Pair.a) {
		gc_mark(obj->Pair.a);
	}
	if (obj->Pair.b) {
		gc_mark(obj->Pair.b);
	}
	return;
}
case Array   :
{
	if (obj->Array.elements) {
		for (int i = 0;  i < obj->Array.size;  ++i) {
			if (obj->Array.elements[i]) {
				gc_mark(obj->Array.elements[i]);
			}
		}
	}
	if (obj->Array.elements) {
		gc_markOnly(obj->Array.elements);
	}
	return;
}
case Closure :{return;}
case StdFunc:{
	return;
}
case UserFunc:
{
	if (obj->UserFunc.parameters) {
		gc_mark(obj->UserFunc.parameters);
	}
	if (obj->UserFunc.body) {
		gc_mark(obj->UserFunc.body);
	}
	if (obj->UserFunc.code) {
		gc_mark(obj->UserFunc.code);
	}
	return;
}
case Binop   :
{
	if (obj->Binop.lhs) {
		gc_mark(obj->Binop.lhs);
	}
	if (obj->Binop.rhs) {
		gc_mark(obj->Binop.rhs);
	}
	return;
}
case Unyop   :
{
	if (obj->Unyop.rhs) {
		gc_mark(obj->Unyop.rhs);
	}
	return;
}
case GetVar  :
{
	if (obj->GetVar.id) {
		gc_mark(obj->GetVar.id);
	}
	return;
}
case SetVar  :
{
	if (obj->SetVar.id) {
		gc_mark(obj->SetVar.id);
	}
	if (obj->SetVar.rhs) {
		gc_mark(obj->SetVar.rhs);
	}
	return;
}
case GetArray:
{
	if (obj->GetArray.array) {
		gc_mark(obj->GetArray.array);
	}
	if (obj->GetArray.index) {
		gc_mark(obj->GetArray.index);
	}
	return;
}
case SetArray:
{
	if (obj->SetArray.array) {
		gc_mark(obj->SetArray.array);
	}
	if (obj->SetArray.index) {
		gc_mark(obj->SetArray.index);
	}
	if (obj->SetArray.value) {
		gc_mark(obj->SetArray.value);
	}
	return;
}
case GetField:
{
	if (obj->GetField.id) {
		gc_mark(obj->GetField.id);
	}
	if (obj->GetField.field) {
		gc_mark(obj->GetField.field);
	}
	return;
}
case Call    :
{
	if (obj->Call.function) {
		gc_mark(obj->Call.function);
	}
	if (obj->Call.arguments) {
		gc_mark(obj->Call.arguments);
	}
	return;
}
case Return  :
{
	if (obj->Return.value) {
		gc_mark(obj->Return.value);
	}
	return;
}
case Break   :
{
	return;
}
case Continue:
{
	return;
}
case Print   :
{
	if (obj->Print.arguments) {
		gc_mark(obj->Print.arguments);
	}
	return;
}
case If      :
{
	if (obj->If.condition) {
		gc_mark(obj->If.condition);
	}
	if (obj->If.statement1) {
		gc_mark(obj->If.statement1);
	}
	if (obj->If.statement2) {
		gc_mark(obj->If.statement2);
	}
	return;
}
case Loop    :
{
	if (obj->Loop.initialization) {
		gc_mark(obj->Loop.initialization);
	}
	if (obj->Loop.condition) {
		gc_mark(obj->Loop.condition);
	}
	if (obj->Loop.iteration) {
		gc_mark(obj->Loop.iteration);
	}
	if (obj->Loop.statement) {
		gc_mark(obj->Loop.statement);
	}
	return;
}
case Block   :
{
	if (obj->Block.statements) {
		for (int i = 0;  i < obj->Block.size;  ++i) {
			if (obj->Block.statements[i]) {
				gc_mark(obj->Block.statements[i]);
			}
		}
	}
	if (obj->Block.statements) {
		gc_markOnly(obj->Block.statements);
	}
	return;
}
case Transition:
{
	if(obj->Transition.id){
		gc_mark(obj->Transition.id);
	}
	return;
}
case State   :
{
	if (obj->State.id) {
		gc_mark(obj->State.id);
	}
	if (obj->State.parameters) {
		gc_mark(obj->State.parameters);
	}
	if (obj->State.events) {
		gc_mark(obj->State.events);
	}
	return;
}
case Event   :
{
	if (obj->Event.id) {
		gc_mark(obj->Event.id);
	}
	if (obj->Event.parameters) {
		gc_mark(obj->Event.parameters);
	}
	if (obj->Event.block) {
		gc_mark(obj->Event.block);
	}
	return;
}
case EventH :
{
	return;
}
case Variable:
{
	if (obj->Variable.id) {
		gc_mark(obj->Variable.id);
	}
	return;
}
case EmitContext:
{
	if (obj->EmitContext.local_vars) {
		gc_mark(obj->EmitContext.local_vars);
	}
	if (obj->EmitContext.state_vars) {
		gc_mark(obj->EmitContext.state_vars);
	}
	if (obj->EmitContext.global_vars) {
		gc_mark(obj->EmitContext.global_vars);
	}
	return;
}
default:
	fprintf(stderr, "markObject: unknown type %d\n", getType(obj));
	return; //error
	}
}

void collectObjects(void)	// pre-collection funciton to mark all the symbols
{
	collectSymbols();
	collectSttTrans();
	// collect ir code
	if(nIrCode != 0){
		gc_markOnly(IrCodeList); // mark the ir code array itself
		for (int i = 0;  i < nIrCode;  ++i)
		{
			oop code = IrCodeList[i];
			if (code == NULL) continue; // skip null codes
			gc_markOnly(code); // mark the code itself
			// mark the code elements
			if (code->IntArray.elements == NULL) continue; // skip null elements
			// mark the code elements
			gc_markOnly(code->IntArray.elements); // mark the code elements
		}
	}
	#ifdef WEBSHICA
	collectWeb(); // collect web code
	#endif
    collectYYContext(); // collect yycontext
}


/*=============== EXECUTOR MARKER ==============*/

/* ==================================*/


void markExecutors(oop ptr)
{
	// collect ir code
	switch(ptr->kind){
		case IntVal:break; // nothing to do
		case FloVal:break; // nothing to do
		case StrVal:{
			dprintf("markExecutors StrVal\n");
			if(ptr->StrVal.value != NULL){
				gc_markOnly(ptr->StrVal.value); // mark the string value
			}
			dprintf("markExecutors StrVal done\n");
			return;
		}
		case IntArray:{
			dprintf("markExecutors IntArray\n");
			if(ptr->IntArray.elements != NULL){
				gc_markOnly(ptr->IntArray.elements); // mark the int array elements
			}
			dprintf("markExecutors IntArray done\n");
			return;
		}
		case Stack:{
			dprintf("markExecutors Stack\n");
			int size = ptr->Stack.size;
			for(int i = 0; i < size; i++){
				if(ptr->Stack.elements[i] != NULL){	
					gc_mark(ptr->Stack.elements[i]); 
				}
			}
			gc_markOnly(ptr->Stack.elements); // mark the stack elements
			dprintf("markExecutors Stack done\n");
			return;
		}
		case IntQue3:{
			dprintf("markExecutors IntQue3\n");
			int pos = ptr->IntQue3.head;
			for(int i = 0; i < ptr->IntQue3.size; i++){
				if(ptr->IntQue3.que[pos] != NULL){
					gc_mark(ptr->IntQue3.que[pos]); 
				}
				pos = (pos + 1) % IntQue3Size;
			}
			//que[3] is fixed size
			dprintf("markExecutors IntQue3 done\n");
			return;
		}
		case Thread:{
			dprintf("markExecutors Thread\n");
			if(ptr->Thread.stack != NULL){
				gc_mark(ptr->Thread.stack); // mark the thread stack
			}
			if(ptr->Thread.queue != NULL){
				gc_mark(ptr->Thread.queue); // mark the thread queue
			}
			dprintf("markExecutors Thread done\n");
			return;
		}
		case EventHandler:{
			dprintf("markExecutors EventHandler\n");
			if(ptr->EventHandler.data){
				gc_markOnly(ptr->EventHandler.data); // mark the event handler data
				for(int i = 0; i<ptr->EventHandler.nData; i++){
					if(ptr->EventHandler.data[i]!=NULL){
						gc_mark(ptr->EventHandler.data[i]); // mark the event handler data
					}
				}
			}
			if(ptr->EventHandler.threads != NULL){
				for(int i = 0; i<ptr->EventHandler.size; i++){
					if(ptr->EventHandler.threads[i]!=NULL){
						gc_mark(ptr->EventHandler.threads[i]); // mark the event handler
					}
				}
				gc_markOnly(ptr->EventHandler.threads); // mark the event handler queue
			}
			dprintf("markExecutors EventHandler done\n");
			return;
		}
		case Agent:{
			dprintf("markExecutors Agent\n");
			if(ptr->Agent.stack){
				gc_mark(ptr->Agent.stack);
			}
			if(ptr->Agent.eventHandlers != NULL){
				gc_markOnly(ptr->Agent.eventHandlers); 
				for(int i = 0; i < ptr->Agent.nEvents; i++){
					if(ptr->Agent.eventHandlers[i]){
						gc_mark(ptr->Agent.eventHandlers[i]);
					}
				}
			}
			dprintf("markExecutors Agent done\n");
			return;
		}
		case Instance:{
			dprintf("markExecutors Instance\n");
			if(ptr->Instance.fields){
				int nFields = ptr->Instance.nFields;
				printf("markExecutors Instance nFields=%d\n", nFields);
				gc_markOnly(ptr->Instance.fields); // mark the instance fields array
				for(int i = 0; i < nFields; i++)
				{
					if(ptr->Instance.fields[i]){
						gc_mark(ptr->Instance.fields[i]); // mark the instance fields
					}
				}
			}
			dprintf("markExecutors Instance done\n");
			return;
		}
		case Any:{
			printf("Any type is not supported in markExecutors\n");
			return ;
		}
		default:{
			dprintf("markExecutors ERROR: unknown type %d\n", ptr->kind);
			fprintf(stderr, "markExecutors ERROR: unknown type %d\n", ptr->kind);
			//error
			return ;
		}
	}
	dprintf("markExecutors ERROR\n");
	return;
}

void collectExecutors(void)
{

	dprintf("collectExecutors\n");
	//FIXME:
#ifdef WEBSHICA

#else // Linux
	
#endif 
	dprintf("collectExecutors done\n");
}

/*====================== WEBCONNECTION =====================*/

/* =========================================================*/

#ifdef WEBSHICA
// #include <emscripten.h>
// #include <emscripten/bind.h>
// #include <emscripten/val.h>
// WEB RUN 
int compile_init(int index);
int compile_finalize();
oop webcode  = NULL;
oop webagent = NULL;
gc_context *comctx = NULL; // context for the garbage collector
gc_context *exectx = NULL; // context for the garbage collector

//once called, initialize the memory for the garbage collector
int memory_init(void)
{
#ifdef MSGC
	gc_init(1024 * 1024); // initialize the garbage collector with 1MB of memory
#elif defined(MSGCS)
	gc_init(1024 * 1024 * 4); // initialize the garbage collector with 4MB of memory
	exectx = &gc_ctx;
	comctx = ctx  = newGCContext(1024 * 1024); // initialize the garbage collector with 1MB of memory
#else
	#error "MSGCS or MSGC must be defined"
#endif

	return 0; 
}

unsigned int maxNumWebCodes = 1; // maximum number of web codes
oop *webCodes = NULL; // web codes
int nWebCodes = 0; // number of web codes

unsigned int maxNumWebAgents = 1; // maximum number of web agents
oop *webAgents = NULL; // web agents
int nWebAgents = 0; // number of web agents



void collectWeb(void)
{
	assert(ctx == comctx); // check if the context is equal to the compilation context
	gc_markOnly(webCodes); // mark the web codes
	printf("collectWeb: nWebCodes=%d, size %lu\n", nWebCodes, sizeof(oop) * maxNumWebCodes);
	if(nWebCodes > 0) {
		for(int i = 0; i < nWebCodes; ++i){
			if(webCodes[i] != NULL){//manualy mark web codes cause they are called from both side.
				printf("\tmarking web code %d, size %lu\n", i, sizeof(int) * (webCodes[i]->IntArray.size));
				gc_markOnly(webCodes[i]); // mark the web code
				gc_markOnly(webCodes[i]->IntArray.elements); // mark the web code elements
			}
		}
	}
	gc_markOnly(webAgents); // mark the web agents
	if(nWebAgents > 0) {
		for(int i = 0; i < nWebAgents; ++i){
			if(webAgents[i] != NULL){
				gc_markOnly(webAgents[i]); // mark the web agent
				gc_markOnly(webAgents[i]->IntArray.elements); // mark the web agent elements
			}
		}
	}
}

//CCALL
//WARNING: which ctx you use is important -> comctx
int initWebCodes(int num)
{
	ctx = comctx; // use the context for the garbage collector
	webCodes = (oop *)gc_beAtomic(malloc(sizeof(oop) * num)); // initialize web codes
	maxNumWebCodes = num; // set the maximum number of web codes
	return 0; 
}

int addWebCode(void)
{
	assert(ctx == comctx); // check if the context is equal to the compilation context
	if(nWebCodes >= maxNumWebCodes){
		printf("%s %d contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	webCodes[nWebCodes++] = NULL; // add a new web code
	return 0; 
}

int compile_init(int index)
{
	printf("compile_init\n");
	webCodes[index] = NULL; // initialize the web code
	ctx = comctx; // use the context for the garbage collector
	ctx->nroots = 0; // reset the number of roots
	gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now

	// reset global variables
	initSymbols();
	initSttTrans();
	initLine();

	gc_collectWithCleanup(); // collect garbage
	//gc_collect(); // collect garbage

	gc_markFunction = (gc_markFunction_t)markObject; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectObjects; // set the collect function for the garbage collector

    nil   = newUndefine();gc_pushRoot(nil);
    FALSE = newInteger(0);gc_pushRoot( FALSE);
    TRUE  = newInteger(1);gc_pushRoot(TRUE);

	printf("compile_event_init\n");
	compile_event_init(); // initialize the event system
	printf("compile_func_init\n");
	compile_func_init(); // initialize the standard functions
	printf("compile_eo_init\n");
	compile_eo_init(); // initialize the eo system
	printf("compile_init done\n");
	return 1;
}

int compile_finalize()
{
	// garbage collection
	rprintf("Running garbage collector...\n");
    gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now
	gc_collect(); // collect garbage
	return 1; // return 1 to indicate success
}

//CCALL
int compileWebCode(const int index, const char *code)
{
	printf("nroot: %d\n", ctx->nroots);
	if(index < 0 || index >= maxNumWebCodes){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	compile_init(index); // initialize the compiler
	store(code); // store the code to the memory
	initYYContext();
	webCodes[index] = compile();

	if(webCodes[index] == NULL){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		compile_finalize(); // finalize the compilation
		return 1; // return 1 to indicate failure
	}
	// print bytecode 
	printf("Printing bytecode for web code %d:\n", index);
	printCode(webCodes[index]);
	compile_finalize(); // finalize the compilation
	return 0; 
}

int deleteWebCode(const int index)
{
	if(index < 0 || index >= maxNumWebCodes){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	for(int i = index; i < nWebCodes; ++i)
	{
		webCodes[i] = webCodes[i + 1]; // shift the web codes
	}
	nWebCodes--; // decrease the number of web codes
	return 0; 
}


//WARNING: which ctx you use is important -> comctx
int initWebAgents(int num)
{
	executor_event_init(); // initialize the event system for the executor
	executor_func_init(); // initialize the standard functions for the executor
	ctx = comctx; // use the context for the garbage collector
	if(num <= 0){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	webAgents = (oop *)gc_beAtomic(malloc(sizeof(oop) * num)); // initialize web agents
	
	printf("Initializing web %d agents...\n", num);
	gc_markFunction = (gc_markFunction_t)markExecutors; // set the mark function for the garbage collector
    gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector
	ctx = exectx; // use the context for execution
	memset(ctx->memory, 0, (char*)ctx->memend - (char*)ctx->memory); // clear the memory
	gc_separateContext(num,0); // separate context for the garbage collector
	assert(num == ctx->nroots); // check if the number of roots is equal to the number of web agents
	gc_context **ctxs = (gc_context **)ctx->roots; // initialize web agents
	for(int i = 0; i<num; ++i)
	{
		ctx = ctxs[i];
		#ifdef DEBUG
		printf("context size %ld: %p -> %p\n", (char*)ctx->memend - (char*)ctx->memory, ctx->memory, ctx->memend);
		#endif
		GC_PUSH(oop,agent,newAgent(0,0)); // create a new agent
		#ifdef DEBUG
		printf("type of agent: %d\n", getKind(agent));
		#endif
		assert(getKind(agent) == Agent); // check if the agent is of type Agent
		assert(ctx->nroots == 1); // check if the number of roots is equal to 1
		assert(getKind(((oop)*ctx->roots[0])) == Agent); // check if the root is of type Agent
	}
	maxNumWebAgents = num; // set the maximum number of web agents
	printf("Initialized %d web agents.\n", num);
	nWebAgents = num; // set the number of web agents
	ctx = exectx; // reset the context to the execution context
	return 0; 
}

//WARNING: which ctx you use is important -> exectx (initWebAgents)
int executeWebCodes(void)
{
	gc_context **ctxs = (gc_context **)ctx->roots; // initialize web agents
	printf("nWebAgents: %d\n", nWebAgents);
	for(int i = 0; i<nWebAgents ; i++){
		setActiveAgent(i); // set the agent as active
		ctx = ctxs[i]; // set the context to the current web agent
		oop agent = (oop)*ctx->roots[0]; // get the agent from the context memory
		if(agent==retFlags[ERROR_F])continue;
		if(getObj(agent, Agent, isActive) == 0){
			agent = execute(webCodes[i] ,agent , agent);
		}else{
			printf("nEvents: %d\n", getObj(agent, Agent, nEvents));
			if(getObj(agent, Agent, nEvents) == 0){
				printAgent(agent); // print the agent if it has no events
				continue; // skip to the next agent
			}
			for(int j = 0; j < getObj(agent, Agent, nEvents); ++j){
				// get event data
				oop eh = getObj(agent, Agent, eventHandlers)[j];
				EventTable[getObj(eh, EventHandler, EventH)].eh(eh);
				if(impleBody(webCodes[i], eh, agent)==retFlags[TRANSITION_F]){
					getObj(agent, Agent, isActive) = 0;
					getObj(agent, Agent, pc) = intArray_pop(getObj(agent, Agent, stack));
					getObj(agent, Agent, eventHandlers) = NULL;
					break;
				}
			}
		}
		// printCode(webCodes[i]); // print the bytecode of the web code
	}
	ctx = exectx; // reset the context to the execution context
	return 0; 
}
int stopWebCodes(void)
{
	ctx = comctx; // use the context for the garbage collector
	return 0; 
}

int getCompiledWebCode(int index)
{
	if(index < 0 || index >= maxNumWebCodes){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	FILE *f = fopen("/ints.bin", "wb");
  	if (!f){
		reportError(DEVELOPER, 0, "cannot open file for writing");
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		return 1; // return 1 to indicate failure
  	}
	int *code = webCodes[index]->IntArray.elements; // get the int array elements of the web code
	int size = webCodes[index]->IntArray.size; // get the size of the int array

  	size_t n = sizeof(int) * size; // calculate the size of the int array in bytes
  	int n2 = fwrite(code, sizeof(int), size, f); // write the int array to the file
  	if(n2 != size){
		reportError(DEVELOPER, 0, "cannot write all data to file");
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		fclose(f);
		return 1; // return 1 to indicate failure
  	}
  	fclose(f); // close the file
  	return 0; 
}

#endif // WEBSHICA

/*====================== MAIN =====================*/

/* ================================================*/

#ifndef WEBSHICA
int main(int argc, char **argv)
{
	int opt_c = 0;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-c") == 0){
			opt_c = 1;
		}
	}
#if TAGFLT
    assert(sizeof(double) == sizeof(intptr_t));
#endif
	gc_init(1024 * 1024);// initialize the garbage collector with 1MB of memory 

    nil   = newUndefine();	gc_pushRoot(nil);
	FALSE = newInteger(0);	gc_pushRoot( FALSE);
    TRUE  = newInteger(1);	gc_pushRoot(TRUE);
	
	gc_markFunction = (gc_markFunction_t)markObject; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectObjects; // set the collect function for the garbage collector
	rprintf("Compiling code:\n");
	compile_event_init(); // initialize the event system
	compile_func_init(); // initialize the standard functions
	compile_eo_init(); // initialize the eo functions

	// compile code
	oop code = compile();

	if(code == NULL){
		rprintf("Compilation failed.\n");
		printErrorList(); // print the error list
		return 1; // return 1 to indicate failure
	}

	// print bytecode 
	rprintf("Print IR code:\n");
	printCode(code);

	// garbage collection
	rprintf("Running garbage collector...\n");
    gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector
	initYYContext();// initialize the yycontext
#ifdef MSGC
	nroots = 0; // reset the number of roots
#elif defined(MSGCS)
	gc_ctx.nroots = 0; // reset the number of roots
#else
	#error "MSGCS or MSGC must be defined"
#endif
	gc_markOnly(code); // mark the code itself
	gc_markOnly(code->IntArray.elements); // mark the code elements
	gc_collect();


	if(opt_c == 1){
		rprintf("end of compilation\n");
		return 0;
	}
	printCode(code); // print the bytecode



	gc_markFunction = (gc_markFunction_t)markExecutors; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector

	// execute code
	rprintf("Executing code...\n");
	executor_event_init(); // initialize the event system for the executor
	executor_func_init(); // initialize the standard functions for the executor
/*
 * CHECK: if assertion is happening, please check the initialization of above functions
*/
#ifdef MSGC
	assert(nroots == 0); // check if the number of roots is equal to 0
#elif defined(MSGCS)
	assert(gc_ctx.nroots == 0); // check if the number of roots is equal to 0
#else
	#error "MSGCS or MSGC must be defined"
#endif
	gc_pushRoot(code);
	printf("\tExecuting...\n");
	int ret = runNative(code);
	printf("\tExecution done.\n");

	if(ret){
		rprintf("Execution error.\n");
		printErrorList(); // print the error list
		return 1; // return 1 to indicate failure
	}
	
	rprintf("Execution finished.\n");
	gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now
	// free memory
	gc_collect(); 
}
#endif