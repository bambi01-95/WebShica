

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
#include "./Object/object.h"
#include "./Parser/parser.h"
#include "./Entity/entity.h"

#ifdef WEBSHICA
#include "./Platform/WebShica/Library/library.h"
#else // LINUX
#include "./Platform/Linux/Library/library.h"
#endif

/* GARBAGE? */

// ent *Agents = NULL;
// int nAgents = 0; // number of agents
// ent *codes = NULL; // codes for each agent
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
#define stop() printf("%s line %d\n", __FILE__, __LINE__); fflush(stdout);



/*======================= COMPILE =======================*/
/* EVENT HANDLER */
int compile_event_init(){
	//lsl event handler
	entryEH = intern("entryEH");
	entryEH->Symbol.value = newEventH(0,0); // 0 argument
	exitEH = intern("exitEH");
	exitEH->Symbol.value =  newEventH(0,0); // 0 argument
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


#define push(O)	intArray_push(stack, O)
#define pop()	intArray_pop(stack)
#define top()	intArray_last(stack)
#define pick(I)  stack->IntArray.elements[I]


ent execute(ent prog, ent entity, ent agent);
#define TAGBITS 2
enum {
	TAG_PTR = 0b00, // ordinary pointer (lower 2 bits = 00)
	TAG_INT=0b01,
	TAG_FLT=0b10,
	TAG_FLAG=0b11,
};
typedef enum { ERROR_F,NONE_F, HALT_F, EOE_F, EOC_F, CONTINUE_F,TRANSITION_F } retFlag_t;
#define MAKE_FLAG(f) ((ent)(((intptr_t)(f) << TAGBITS) | TAG_FLAG))

ent retFlags[7] = {
	MAKE_FLAG(ERROR_F),
	MAKE_FLAG(NONE_F),
	MAKE_FLAG(HALT_F),
	MAKE_FLAG(EOE_F),
	MAKE_FLAG(EOC_F),
	MAKE_FLAG(CONTINUE_F),
	MAKE_FLAG(TRANSITION_F),
};

ent impleBody(ent code, ent eh, ent agent){
	assert(code->kind == IntArray);
	assert(eh->kind == EventHandler);
	assert(agent->kind == Agent);
	ent *threads = eh->EventHandler.threads;
	ent ret = retFlags[ERROR_F];
	for(int i=0;i<eh->EventHandler.size; ++i){
		ent thread = threads[i];
		if(thread->Thread.inProgress == 1){
			ret = execute(code,thread, agent);
		}else if(thread->Thread.queue->IntQue3.size > 0){
			thread->Thread.stack = dequeue3(thread);
			assert(thread->Thread.stack != NULL);
			ret = execute(code, thread, agent);
		}
		if(ret == retFlags[TRANSITION_F]){
			return ret;
		}
	}
	return ret; // return 1 to indicate success
}


/* =========================================== */
int runNative(ent code){
	GC_PUSH(ent, agent, newAgent(0,0));//nroos[0] = agent; // set the first root to the agent
	agent = execute(code, agent, agent);
	dprintf("agent: %d\n", agent->Agent.isActive);
	while(1){
		if(agent->Agent.isActive == 0) {
			agent = execute(code ,agent , agent);
		}else{
			for(int i = 0; i< agent->Agent.nEvents; ++i){
				// get event data
				ent eh = agent->Agent.eventHandlers[i];
				EventTable[eh->EventHandler.EventH].eh(eh);
				if(impleBody(code, eh, agent)==retFlags[TRANSITION_F]){
					agent->Agent.isActive = 0;
					break;
				}
			}
		}
	}
	GC_POP(agent);
	return 1; // return 1 to indicate success
}

ent execute(ent prog,ent entity, ent agent)
{

	int opstep = 20; // number of operations to execute before returning
    int* code = prog->IntArray.elements;
	int size = prog->IntArray.size;
	int *pc;
	int *rbp;
	ent stack;
	switch(entity->kind) {
		case Thread:{
			pc = &entity->Thread.pc; // program counter
			rbp = &entity->Thread.rbp; // base pointer
			stack = entity->Thread.stack; // stack pointer
			break;
		}
		case Agent:{
			pc = &entity->Agent.pc; // program counter
			rbp = &entity->Agent.rbp; // base pointer
			stack = entity->Agent.stack; // stack pointer
			opstep = 100; // number of operations to execute before returning
			break;
		}
		default:{
			fatal("execute: unknown entity kind %d", entity->kind);
			return NULL; // should never reach here
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
			if(entity->kind == Agent){
				entity->Agent.rbp = nvars;
			}
			continue;
		}
	    case iGT:printOP(iGT);  r = pop();  l = pop();  push(l > r);  continue;
		case iGE:printOP(iGE);  r = pop();  l = pop();  push(l >= r); continue;
		case iEQ:printOP(iEQ);  r = pop();  l = pop();  push(l == r); continue;
		case iNE:printOP(iNE);  r = pop();  l = pop();  push(l != r); continue;
		case iLE:printOP(iLE);  r = pop();  l = pop();  push(l <= r); continue;
		case iLT:printOP(iLT);  r = pop();  l = pop();  push(l < r);  continue;
	    case iADD:printOP(iADD);  r = pop();  l = pop();  push(l + r);  continue;
	    case iSUB:printOP(iSUB);  r = pop();  l = pop();  push(l - r);  continue;
	    case iMUL:printOP(iMUL);  r = pop();  l = pop();  push(l * r);  continue;
	    case iDIV:printOP(iDIV);  r = pop();  l = pop();  push(l / r);  continue;
	    case iMOD:printOP(iMOD);  r = pop();  l = pop();  push(l % r);  continue;
	    case iGETVAR:{
			printOP(iGETVAR);
			int symIndex = fetch(); // need to change
			push(stack->IntArray.elements[symIndex + *rbp+1]); // get the variable value
			dprintf("%d => %d\n", symIndex, stack->IntArray.elements[symIndex + *rbp+1]);
			continue;
		}
		case iGETGLOBALVAR:{ /* I: index from global-stack[0] to value */
			printOP(iGETGLOBALVAR);
			int symIndex = fetch(); 
			push(agent->Agent.stack->IntArray.elements[symIndex]);
			dprintf("%d => %d\n", symIndex, agent->Agent.stack->IntArray.elements[symIndex]);
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
			stack->IntArray.elements[symIndex+*rbp+1] = pop();
			continue;
		}
		case iSETGLOBALVAR:{
			printOP(iSETGLOBALVAR);
			int symIndex = fetch();
			agent->Agent.stack->IntArray.elements[symIndex] = pop(); // set the global variable value
			continue;
		}
		case iSETSTATEVAR:{
			printOP(iSETSTATEVAR);
			int index = fetch();
			int symIndex = index + agent->Agent.rbp;
			if(symIndex > agent->Agent.stack->IntArray.size -1)//FIXME: every time check the size is not efficient, it should be done at init state.
				for(int i = agent->Agent.stack->IntArray.size; i <= symIndex; ++i)
					intArray_push(agent->Agent.stack, 0); // extend the state variable array
			agent->Agent.stack->IntArray.elements[symIndex] = pop(); // set the state variable value
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
			if (pop()) { // if top of stack is true
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
			int retValue = pop(); // get the return value
			stack->IntArray.size = *rbp+1; // restore the stack size to the base pointer
			*rbp = pop(); // restore the base pointer
			*pc = pop(); // restore the program counter
			
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
			push(*pc); // save the current program counter
			*pc += l; // set program counter to the function position

			push(*rbp); // save the current base pointer
			*rbp = stack->IntArray.size-1; // set the base pointer to the current stack size
			dprintf("rbp: %d, pc: %d\n", *rbp, *pc);

			for (int i = 1;  i <= r;  ++i) {
				push(pick((*rbp - i -1)));//pc
			}
			continue;
		}
		case iPUSH:{
		printOP(iPUSH);
			l = fetch();
			push(l);
			continue;
		}
		case iCLEAN:{
			printOP(iCLEAN);
			l = fetch(); // number of variables to clean
			int retVal = pop(); // get the return value
			for(int i = 0;  i < l;  ++i) {
				if (stack->IntArray.size == 0) {
					fatal("stack is empty");
				}
				pop(); // clean the top element
			}
			push(retVal); // push the return value back to the stack
			// stack->IntArray.size -= l; // clean the top l elements from the stack
			continue;
		}
		case iPRINT:{
			printOP(iPRINT);
			int nArgs = fetch();
			for(int i = 0; i < nArgs; i++) {
				printf("%d ", pop());
			}
			printf("\n");
			continue;
		}
		case iTRANSITION:{
			printOP(iTRANSITION);
			int nextStatePos = fetch();
			intArray_push(agent->Agent.stack,  nextStatePos+(*pc));//relative position:
			return retFlags[TRANSITION_F];
		}
		case iSETSTATE:{
			printOP(iSETSTATE);
			assert(entity->kind == Agent);
			int ehSize = fetch(); // get the number of event handlers
			printf("ehSize: %d\n", ehSize);
			assert(ehSize >= 0);
			entity->Agent.nEvents = ehSize; // set the number of events
			ent *ehs = entity->Agent.eventHandlers = (ent*)gc_beAtomic(malloc(sizeof(ent*) * ehSize)); //initialize the event handlers
			for(int i=0; i<ehSize; ++i){
				op = fetch();
				assert(op == iSETEVENT);
				printOP(iSETEVENT);
				int eventID = fetch(); // get the event ID
				int nThreads = fetch(); // get the number of threads
				printf("eventID: %d, nThreads: %d\n", eventID, nThreads);
				ehs[i] = newEventHandler(eventID, nThreads); // initialize the event handler <------ ERROR: FIX HERE
				EventTable[eventID].init(ehs[i]);
				for(int j=0; j<nThreads; ++j){
					op = fetch();
					assert(op == iSETPROCESS);
					printOP(iSETPROCESS);
					l = fetch(); // get the aPos
					r = fetch(); // get the cPos
					printf("l: %d, r: %d\n", l, r);
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
			switch(entity->kind){
				case Thread:{
					printf("thread finished\n");
					entity->Thread.inProgress = 0; // set the thread to not in progress
					entity->Thread.pc = stack->IntArray.elements[0]; // restore the program counter
					entity->Thread.rbp = 0;
					return retFlags[EOE_F]; // return EOE_F to indicate end of execution
				}
				case Agent:{
					entity->Agent.isActive = 0; // set the agent to not active
					for(int i=0; i<nVariables; i++){
						pop();
					}
					printf("agent %d is inactive\n", entity->Agent.isActive);
					continue;
				}
				default:{
					fatal("execute: unknown entity kind %d", entity->kind);
					return NULL; // should never reach here
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
			entity->Agent.isActive = 0;
			entity->Agent.pc = pop();
			entity->Agent.eventHandlers = NULL;//TODO: don't remove this in the future
			continue;
		}
	    case iHALT:{
			printOP(iHALT);
			pop();//first rbp
			for(int i = 0;  i < stack->IntArray.size;  ++i) {
				printf("%d ", stack->IntArray.elements[i]);
			}
			printf("\n");
			return stack; // return the answer
		}
	    default:fatal("illegal instruction %d", op);
	}
	}
# undef fetch
# undef push
# undef pop
    return 0; // should never reach here
}



/* ========================== COMPILER =============================== */

/* =================================================================== */


/* ========== OBJECT ============ */

/* ============================== */

/* = VARIABLE === */

/* =============== */
ent compile();



/* ==== STATE ==== */

/* =============== */

// STATESSm
// s1: pos, name;
// s2: pos, name;
// s3: pos, name;
enum { APSTATE = 0, ATRANSITION = 1 };

static oop *states = 0;
static int nstates = 0;
static oop *transitions = 0; // transitions between states
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
			oop state = states[i];
			if (state == NULL) continue; // skip null states
			gc_mark(state); //PAIR
		}
	}
	if(ntransitions != 0){
		gc_markOnly(transitions); // mark the transitions array itself
		for(int i = 0;  i < ntransitions;  ++i)
		{
			oop trans = transitions[i];
			if(trans == NULL) continue; // skip null transitions
			gc_mark(trans);//PAIR
		}
	}
	return 0; 
}


void appendS0T1(oop name, int pos,int type)
{
	if(type != 0 && type != 1) {
		printf("type must be 0 or 1, got %d\n", type);
		return;//error
	}
	oop *lists = type == APSTATE ? states : transitions;
	int *listSize = type == APSTATE ? &nstates : &ntransitions;
	for (int i = 0;  i < *listSize;  ++i) {
		oop stateName = get(lists[i], Pair,a);
		if (stateName == name) {
			printf("state %s already exists\n", get(name, Symbol,name));
			return;
		}
	}

	gc_pushRoot((void*)&name);
	switch(type){
		case APSTATE:{
			states = realloc(states, sizeof(oop) * (nstates + 1));
			states[nstates] = newPair(name, newInteger(pos));
			nstates++;
			break;
		}
		case ATRANSITION:{
			transitions = realloc(transitions, sizeof(oop) * (ntransitions + 1));
			transitions[ntransitions] = newPair(name, newInteger(pos));
			ntransitions++;
			break;
		}
	}
	gc_popRoots(1);
	return;
}

void setTransPos(ent prog){
	int *code = prog->IntArray.elements;
	for (int i = 0;  i < ntransitions;  ++i) {
		oop trans = transitions[i];
		oop transName = get(trans, Pair,a);
		int transPos = Integer_value(get(trans, Pair,b));//pos of after emitII(prog, iTRANSITION, 0);
		for (int j = 0;  j < nstates;  ++j) {
			oop state = states[j];
			oop stateName = get(state, Pair,a);
			if (stateName == transName) {
				int statePos = Integer_value(get(state, Pair,b));
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



void emitL (ent array, oop object) 	  { intArray_append(array, Integer_value(object)); }
void emitI (ent array, int i     ) 	  { intArray_append(array, i); }
void emitII(ent array, int i, int j)      { emitI(array, i); emitI(array, j); }
void emitIL(ent array, int i, oop object) { emitI(array, i); emitL(array, object); }
void emitIII(ent array, int i, int j, int k)
{
	emitI(array, i);
	emitI(array, j);
	emitI(array, k);
}

void printCode(ent code);

int emitOn(ent prog,oop vars, oop ast)
{
	assert(getType(vars) == EmitContext);
	int ret = 0; /* ret 0 indicates success */
    switch (getType(ast)) {
		case Undefined:
		case Integer:{
			printTYPE(_Integer_);
			emitIL(prog, iPUSH, ast);
			return 0;
		}
		case Array:{
			printTYPE(_Array_);
			oop *elements = get(ast, Array,elements);
			int size = get(ast, Array,size);
			emitII(prog, iPUSH, size); // push the size of the array
			for (int i = 0;  i < size;  ++i) {
				if(emitOn(prog, vars, elements[i]))return 1; // compile each element
			}
			return 0; 
		}
		case Binop:{
			printTYPE(_Binop_);
			if(emitOn(prog, vars, get(ast, Binop,lhs))) return 1;
			if(emitOn(prog, vars, get(ast, Binop,rhs))) return 1;
			switch (get(ast, Binop,op)) {
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
			fatal("file %s line %d unknown Binop operator %d", __FILE__, __LINE__, get(ast, Binop,op));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Unyop:{
			printTYPE(_Unyop_);
			oop rhs = get(ast, Unyop,rhs);
			switch (get(ast, Unyop,op)) {
				case NEG: emitII(prog,iPUSH, 0);emitOn(prog, vars, rhs); emitI(prog, iSUB); return 0;
				case NOT: emitII(prog,iPUSH, 0);emitOn(prog, vars, rhs); emitI(prog, iEQ);  return 0;
				default: break;
			}
			oop variable = searchVariable(vars, get(rhs, GetVar,id));
			if(variable == NULL){
				reportError(ERROR, 0, "variable %s not found", get(get(rhs, GetVar,id), Symbol,name));
				return 1;
			}
			switch(get(ast, Unyop,op)) {
					case BINC: if(emitOn(prog, vars, rhs))return 1;emitII(prog, iPUSH, 1); emitI(prog, iADD);emitII(prog, iSETVAR, Integer_value(get(variable,Pair,b)));if(emitOn(prog, vars, rhs))return 1; return 0;
					case BDEC: if(emitOn(prog, vars, rhs))return 1;emitII(prog, iPUSH, 1); emitI(prog, iSUB);emitII(prog, iSETVAR, Integer_value(get(variable,Pair,b))); if(emitOn(prog, vars, rhs))return 1;return 0;
					case AINC: if(emitOn(prog, vars, rhs))return 1;if(emitOn(prog, vars, rhs))return 1;emitII(prog, iPUSH, 1); emitI(prog, iADD);emitII(prog, iSETVAR, Integer_value(get(variable,Pair,b))); return 0;
					case ADEC: if(emitOn(prog, vars, rhs))return 1;if(emitOn(prog, vars, rhs))return 1;emitII(prog, iPUSH, 1); emitI(prog, iSUB);emitII(prog, iSETVAR, Integer_value(get(variable,Pair,b))); return 0;
				default: break;
			}
			fatal("file %s line %d unknown Unyop operator %d", __FILE__, __LINE__, get(ast, Unyop,op));
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case GetVar: {
			printTYPE(_GetVar_);
			oop sym = get(ast, GetVar,id);
			oop variable = searchVariable(get(vars, EmitContext, local_vars),sym);// search in local variables first
			if(variable){
				printf("found local variable %s\n", get(sym, Symbol,name));
				emitII(prog, iGETVAR, Integer_value(get(variable,Pair,b)));
				return 0; 
			}
			variable  = searchVariable(get(vars, EmitContext, state_vars),sym); // search in state variables
			if(variable){
				printf("found state variable %s\n", get(sym, Symbol,name));
				emitII(prog, iGETSTATEVAR, Integer_value(get(variable,Pair,b)));
				return 0; 
			}
			variable = searchVariable(get(vars, EmitContext, global_vars),sym); // search in global variables
			if (variable) {
				printf("found global variable %s\n", get(sym, Symbol,name));
				emitII(prog, iGETGLOBALVAR, Integer_value(get(variable,Pair,b)));
				return 0; 
			}
			reportError(ERROR, get(ast,GetVar,line), "variable %s not found", get(sym, Symbol,name));
			return 1;
		}
		case Call:{
			//Standard library functions / user defined functions
			printTYPE(_Call__);
			oop id   = get(ast, Call, function);
			id = get(id, GetVar, id); // get the variable from the GetVar
			oop args = get(ast, Call,arguments);

			oop func = get(id, Symbol, value); // get the function from the variable;

			switch(getType(func)){
				case EventH:{
					printTYPE(_EventH_);
					int index = get(func, EventH,id);// get the index of the event handler
					int nArgs = get(func, EventH,nArgs);
					
					int argsCount = 0;
					while(args != nil){
						oop arg = get(args, Pair,a);
						if(emitOn(prog, vars, arg))return 1; // compile argument
						args = get(args, Pair,b);
						argsCount++;
					}
					if(nArgs != argsCount){
						reportError(ERROR, get(ast,Call,line), "event %s expects %d arguments, but got %d", get(func, Symbol,name), nArgs, argsCount);
						return 1;
					}
					emitIII(prog, iPCALL,  index, nArgs); // call the event handler
					return 0; 
				}
				case StdFunc:{
					printTYPE(_StdFunc_);
					int funcIndex = get(func, StdFunc, index); // get the index of the standard function
					int argsCount = 0;
					while(args != nil){
						oop arg = get(args, Pair,a);
						if(emitOn(prog, vars, arg))return 1; // compile argument
						args = get(args, Pair,b);
						argsCount++;
					}
					if(StdFuncTable[funcIndex].nArgs != argsCount){
						reportError(ERROR, get(ast,Call,line), "standard function %s expects %d arguments, but got %d", get(id, Symbol,name), StdFuncTable[funcIndex].nArgs, argsCount);
						return 1;
					}

					emitII(prog, iSCALL, funcIndex); // call the standard function
					//NOTE: we don't need to clean the stack after the call, because the standard function will do it
					//      BUT we need to check the return value (not implemented yet)
					//emitII(prog, iCLEAN, nReturns); // clean the stack after the call
					return 0; 
				}
				case Closure:{
					printTYPE(_Closure_);
					int nArgs = get(func, Closure,nArgs);
					int pos   = get(func, Closure,pos);
					int argsCount = 0;
					while(args != nil){
						oop arg = get(args, Pair,a);
						if(emitOn(prog, vars, arg))return 1; // compile argument
						args = get(args, Pair,b);
						argsCount++;
					}
					if(nArgs != argsCount){
						reportError(ERROR, get(ast, Call, line), "function %s expects %d arguments, but got %d", get(id, Symbol,name), nArgs, argsCount);
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
		case SetVar: {
			printTYPE(_SetVar_);
			oop sym = get(ast, SetVar,id);
			oop rhs = get(ast, SetVar,rhs);
			switch(getType(rhs)){
				case UserFunc:{
					printTYPE(_Function_);
					if(sym->Symbol.value !=false) {
						reportError(ERROR, get(ast,SetVar,line), "variable %s is already defined as a function", get(sym, Symbol,name));
						return 1;
					}
					oop params = get(rhs, UserFunc,parameters);
					oop body = get(rhs, UserFunc,body);
					get(vars, EmitContext, local_vars) = newVariables();
					GC_PUSH(oop, closure, newClosure());
					while(params != nil){
						oop param = get(params, Pair,a);
						if (searchVariable(get(vars, EmitContext, local_vars), param) != NULL) {
							reportError(ERROR, get(ast,SetVar,line), "parameter %s is already defined", get(param, Symbol,name));
							GC_POP(closure);
							get(vars, EmitContext, local_vars) = NULL;
							return 1;
						}
						insertVariable(get(vars, EmitContext, local_vars), param); // insert parameter into local variables
						closure->Closure.nArgs++;
						params = get(params, Pair,b);
					}
					emitII(prog, iJUMP, 0); //jump to the end of the function // TODO: once call jump 
					int jump4EndPos = prog->IntArray.size - 1; // remember the position of the jump // TODO: once call jump
					closure->Closure.pos = prog->IntArray.size; // remember the position of the closure
					sym->Symbol.value = closure; // set the closure as the value of the variable
					emitII(prog, iMKSPACE, 0); // reserve space for local variables
					int codePos = prog->IntArray.size - 1; // remember the position of the function code
					if(emitOn(prog, vars, body)) {
						GC_POP(closure);
						return 1; // compile function body
					}
					prog->IntArray.elements[codePos] = get(get(vars, EmitContext, local_vars), Variables, size); // set the size of local variables
					prog->IntArray.elements[jump4EndPos] = (prog->IntArray.size - 1) - jump4EndPos; // set the jump position to the end of the function // TODO: once call jump
					GC_POP(closure);
					get(vars, EmitContext, local_vars) = NULL; // clear local variables
					return 0;
				}
				case Integer:
				case GetVar:
				case Unyop:
				case Binop:{
					int scope = get(ast, SetVar,scope);
					switch(scope) {
						case SCOPE_LOCAL:{
							printf("defining local variable %s\n", get(sym, Symbol,name));
							if(emitOn(prog,vars, rhs)) return 1;
							oop variable = insertVariable(get(vars, EmitContext, local_vars),sym);
							emitII(prog, iSETVAR, Integer_value(get(variable,Pair,b)));
							return 0;
						}
						case SCOPE_STATE_LOCAL:{
							printf("defining state variable %s\n", get(sym, Symbol,name));
							if(emitOn(prog,vars, rhs)) return 1;
							oop variable = insertVariable(get(vars, EmitContext, state_vars),sym);
							emitII(prog, iSETSTATEVAR, Integer_value(get(variable,Pair,b)));
							return 0;
						}
						case SCOPE_GLOBAL:{
							printf("defining global variable %s\n", get(sym, Symbol,name));
							if(emitOn(prog,vars, rhs)) return 1;
							oop variable = insertVariable(get(vars, EmitContext, global_vars),sym);
							emitII(prog, iSETGLOBALVAR, Integer_value(get(variable,Pair,b)));
							return 0;
						}
					}
					fatal("file %s line %d emitOn: unknown SetVar scope %d", __FILE__, __LINE__, scope);
					reportError(DEVELOPER,get(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
				default:{
					fatal("file %s line %d emitOn: unknown SetVar type %d", __FILE__, __LINE__, getType(rhs));
					reportError(DEVELOPER, get(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
			}
			fatal("file %s line %d emitOn: unknown SetVar type %d", __FILE__, __LINE__, getType(rhs));
			reportError(DEVELOPER, get(ast,SetVar,line), "please contact %s", DEVELOPER_EMAIL);
			return 0;
		}
		case Pair:{
			printTYPE(_Pair_);
			oop a = get(ast, Pair,a);
			oop b = get(ast, Pair,b);
			fatal("file %s line %d emitOn: Pair is not supported yet", __FILE__, __LINE__);
			reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
			return 1;
		}
		case Print:{
			printTYPE(_Print_);
			oop args = get(ast, Print,arguments);
			int nArgs = 0;
			while (args != nil) {
				oop arg = get(args, Pair,a);
				if(emitOn(prog, vars, arg)) return 1; // compile argument
				args = get(args, Pair,b);
				nArgs++;
			}
			if(nArgs == 0) {
				reportError(ERROR, get(ast, Print, line), "print statement requires at least one argument");
				return 1;
			}
			emitII(prog, iPRINT, nArgs); // print the result
			return 0;
		}
		case If:{
			printTYPE(_If_);
			//NEXT-TODO
			int variablesSize = get(vars, EmitContext, local_vars) ? get(get(vars, EmitContext, local_vars), Variables, size) : 0; // remember the size of variables
			oop condition = get(ast, If,condition);
			oop statement1 = get(ast, If,statement1);
			oop statement2 = get(ast, If,statement2);

			if(emitOn(prog, vars, condition)) return 1; // compile condition

			emitII(prog, iJUMPIF, 0); // emit jump if condition is true
			int jumpPos = prog->IntArray.size-1; // remember position for jump

			if(emitOn(prog, vars, statement1)) return 1; // compile first statement
			if (statement2 != false) {
				emitII(prog, iJUMP, 0);
				int jumpPos2 = prog->IntArray.size-1; // remember position for second jump
				if(emitOn(prog, vars, statement2)) return 1; // compile second statement
				prog->IntArray.elements[jumpPos] = jumpPos2 - jumpPos; // set jump position for first jump
				prog->IntArray.elements[jumpPos2] = (prog->IntArray.size - 1) - jumpPos2; // set jump position
			} else {
				prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position for first jump	
			}
			//NEXT-TODO
			discardVariables(get(vars, EmitContext, local_vars), variablesSize); // discard variables
			return 0;
		}
		case Loop:{
			printTYPE(_Looop_);
			oop initialization = get(ast, Loop,initialization);
			oop condition      = get(ast, Loop,condition);
			oop iteration      = get(ast, Loop,iteration);
			oop statement      = get(ast, Loop,statement);
			//NEXT-TODO
			int variablesSize = get(vars, EmitContext, local_vars) ? get(get(vars, EmitContext, local_vars), Variables, size) : 0; // remember the size of variables

			if (initialization != false) {
				if(emitOn(prog, vars, initialization)) return 1; // compile initialization
			}
			int stLoopPos = prog->IntArray.size; // remember the position of the loop start
			int jumpPos = 0; // position for the jump if condition is true
			if(condition != false) {
				if(emitOn(prog, vars, condition)) return 1; // compile condition
				emitII(prog, iJUMPIF, 0); // emit jump if condition is true
				jumpPos = prog->IntArray.size - 1; // remember position for jump
			}

			if(emitOn(prog, vars, statement)) return 1; // compile statement

			if (iteration != false) {
				if(emitOn(prog, vars, iteration)) return 1; // compile iteration
			}
			emitII(prog, iJUMP, 0); // jump to the beginning of the loop
			prog->IntArray.elements[prog->IntArray.size - 1] = stLoopPos - prog->IntArray.size; // set jump position to the start of the loop
			if(condition != false) {
				prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position for condition
			}
			//NEXT-TODO
			discardVariables(get(vars, EmitContext, local_vars), variablesSize); // discard variables
			return 0;
		}
		case Return:{
			printTYPE(_Return_);
			oop value = get(ast, Return,value);
			if (value != false) {
				if(emitOn(prog, vars, value)) return 1; // compile return value
			} else {
				emitII(prog, iPUSH, 0); // return 0 if no value is specified
			}
			emitI(prog, iRETURN); // emit return instruction
			return 0;
		}
		case Block:{
			printTYPE(_Block_);
			oop *statements = ast->Block.statements;
			int nStatements = ast->Block.size;
			for (int i = 0;  i < nStatements;  ++i) {
				if(emitOn(prog, vars, statements[i])) return 1; // compile each statement
			}
			return 0;
		}
		case Transition:{
			printTYPE(_Transition_);
			oop id = get(ast, Transition,id);
			emitII(prog, iTRANSITION, 0);
			/* WARN: 実際に値を入れるときは、-1して値を挿入する */
			appendS0T1(id, prog->IntArray.size, ATRANSITION); // append state to states
			return 0;
		}
		case State:{
			printTYPE(_State_);
			oop id = get(ast, State,id);
			oop params = get(ast, State,parameters);
			oop events = get(ast, State,events);
			dprintf("State: %s\n", get(id, Symbol,name));

			// compile events
			oop *eventList = events->Block.statements;
			int nElements = 0;
			int elements[events->Block.size]; // collect elements: 0: empty, other: number of event handlers block
			int nEventHandlers = 0;
			oop preid = false;
			emitII(prog, iJUMP, 0);
			int jumpPos = prog->IntArray.size - 1; // remember the position of the jump
			get(vars, EmitContext, state_vars) = newVariables(); // set state variables for the state

			for (int i = 0;  i < get(events, Block, size);  ++i) {
				if(eventList[i] == NULL){
					fatal("%s %d ERROR: eventList[%d] is NULL\n", __FILE__, __LINE__, i);
					reportError(DEVELOPER, 0,"please contact %s", DEVELOPER_EMAIL);
					return 1;
				}
				if((get(eventList[i], Event,id) == entryEH) || (get(eventList[i], Event,id) == exitEH)){
					dprintf("entryEH or exitEH\n");
					elements[nElements++] = 0; // collect empty events
					continue;
				}
				if(getType(eventList[i])!=Event){
					fatal("file %s line %d emitOn: eventList[%d] is not an Event", __FILE__, __LINE__, i);
					reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
					return 1;
					elements[nElements++] = 0; // collect empty events
				}
				else if(get(eventList[i], Event,id) != preid) {
					dprintf("new event\n");
					nEventHandlers++;
					preid = get(eventList[i], Event,id);
					elements[nElements]=1; // collect unique events
				}else if(get(eventList[i], Event,id) == preid){
					dprintf("same event\n");
					elements[nElements]++;
				}
				dprintf("eventList[%d]: %s\n", i, get(eventList[i], Event,id)->Symbol.name);
				if(emitOn(prog, vars, eventList[i])) return 1; // compile each event
			}
			dprintf("Finished collecting events\n");

			prog->IntArray.elements[jumpPos] = (prog->IntArray.size - 1) - jumpPos; // set jump position to the end of the events
			// state initialization
			appendS0T1(id, prog->IntArray.size, APSTATE); // append state to states
			//entryEH
			if(get(eventList[0], Event,id) == entryEH){
				if(emitOn(prog, vars, eventList[0])) return 1; // compile entryEH
			}
			emitII(prog, iSETSTATE, nEventHandlers); // set the number of events and position
			for(int i =0 ,  ehi =0;  i < get(events, Block, size);  ++i) {
				if(elements[i] == 0){ ehi++;continue;} // skip empty events
				oop id = get(eventList[i], Event,id);
				printlnObject(id,1);
				oop eh = get(id,Symbol,value);

				int eventID = eh->EventH.id; // get event ID
				emitIII(prog, iSETEVENT, eventID, elements[ehi]); // emit SETEVENT instruction
				for(int j = 0; j < elements[ehi]; ++j) {
					oop event = eventList[i++];
					oop posPair = get(event, Event,block);
					printlnObject(get(event,Event,id),1);
					emitIII(prog, iSETPROCESS ,Integer_value(get(posPair,Pair,a)),Integer_value(get(posPair,Pair,b))); // set the position of the event handler)
				}
				ehi++; // increment event handler index
			}
			emitI(prog, iIMPL);
			// exitEH
			for(int i = 0; i < get(events, Block, size); i++){
				if(get(eventList[i], Event,id) == exitEH){ 
					if(emitOn(prog, vars, eventList[i])) return 1; // compile exitEH
				}
				if(i == 1)break;//0: entryEH, 1: exitEH ...
			}
			//NEXT-TODO
			// discardVariables(vars->EmitContext.state_vars, 0); // discard variables
			get(vars, EmitContext, state_vars) = NULL; // clear state variables
			emitII(prog, iEOS, 0); // emit EOS instruction to mark the end of the state
			return 0;
		}
		case Event:{
			printTYPE(_Event_);
			oop id = get(ast, Event,id);
			oop params = get(ast, Event,parameters);
			oop block = get(ast, Event,block);

			oop eh = get(id,Symbol,value);
			if(getType(eh) != EventH) {
				fatal("file %s line %d emitOn: event %s() is not an EventH", __FILE__, __LINE__, get(id, Symbol,name));
				reportError(DEVELOPER, get(ast,Event,line), "please contact %s", DEVELOPER_EMAIL);
				return 1;
			}

			get(vars, EmitContext, local_vars) = newVariables(); // set local variables for the event
			int nArgs = eh->EventH.nArgs;
			int paramSize =0;
			int cPos      = 0;
			while(params != nil) {//a:id-b:cond
				oop param = get(params, Pair,a);
				insertVariable(get(vars, EmitContext, local_vars), get(param,Pair,a)); // add parameter to local variables
				if(get(param, Pair, b)!= false){
					if(cPos==0)cPos = prog->IntArray.size; // remember the position of the condition
					if(emitOn(prog, vars, get(param, Pair,b))) return 1; // compile condition if exists
					emitI(prog,iJUDGE); // emit JUDGE instruction
				}
				params = get(params, Pair,b);
				paramSize++;
			}
			if(cPos != 0){
				emitI(prog, iEOC); // emit EOC instruction if condition exists
			}
			if(paramSize != nArgs) {
				reportError(ERROR, get(ast,Event,line), "event %s has %d parameters, but expected %d", get(id, Symbol,name), paramSize, nArgs);
				get(vars, EmitContext, local_vars) = NULL; // clear local variables
				return 1;
			}
			int aPos = prog->IntArray.size; // remember the position of the event handler
			emitII(prog, iMKSPACE, 0); // reserve space for local variables
			int mkspacepos = prog->IntArray.size - 1; // remember the position of MKSPACE


			if(emitOn(prog, vars, block))return 1; // compile block
			emitII(prog, iEOE, get(get(vars, EmitContext, local_vars), Variables, size)); // emit EOE instruction to mark the end of the event handler

			prog->IntArray.elements[mkspacepos] = get(get(vars, EmitContext, local_vars), Variables, size); // store number of local variables
			get(vars, EmitContext, local_vars) = NULL; // clear local variables
			GC_PUSH(oop,apos, newInteger(aPos));
			GC_PUSH(oop,cpos,newInteger(cPos));
			ast->Event.block = newPair(apos,cpos); // set the position of the event handler
			GC_POP(cpos);
			GC_POP(apos);
			return 0;
		}
		default:break;
	}
    fatal("file %s line %d: emitOn: unimplemented emitter for type %d", __FILE__, __LINE__, getType(ast));
	reportError(DEVELOPER, 0, "please contact %s", DEVELOPER_EMAIL);
	return 1;
}

void printCode(ent code){
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
			case iPRINT: {
				inst = "PRINT";
				int nArgs = code->IntArray.elements[++i];
				printf("%03d: %-10s %03d\n", i-1, inst, nArgs);
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


ent compile()
{
	printf("compiling...\n");
    GC_PUSH(ent, prog, intArray_init()); // create a new program code
	emitII(prog, iMKSPACE, 0); // reserve space for local variables
#if DEBUG
int roots = gc_ctx.nroots;
#endif 
	GC_PUSH(oop, vars, newEmitContext()); // If something goes wrong in the web parser, move to before changing GC roots
	// compile the AST into the program code
	int line = 1;
	assert(getType(vars) == EmitContext);
	while(yyparse()){
		if (ISTAG_FLAG(result)) {
			break;
		}
		printf("test line %d: result flag %d\n", __LINE__, ISTAG_FLAG(result));
		printf("compiling statement %d\n", line);
		if(emitOn(prog, vars, result))return NULL;
		result = parserRetFlags[PARSER_READY];
		printf("compiled statement %d [size %4d]\n",++line, (prog->IntArray.size)*4);
	}
	if(result == parserRetFlags[PARSER_FINISH]){
		printf("compilation finished\n");
		emitI (prog, iHALT); // end of program
		printf("total variable size %d\n", get(vars, EmitContext, global_vars)->Variables.size);
		prog->IntArray.elements[1] = get(vars, EmitContext, global_vars)->Variables.size; // store number of variables
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



void markObject(oop obj){
	switch(getType(obj)){
case Integer :{return;}
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
case Variables:
{
	if (obj->Variables.elements) {
		for (int i = 0;  i < obj->Variables.size;  ++i) {
			if (obj->Variables.elements[i]) {
				gc_mark(obj->Variables.elements[i]);
			}
		}
	}
	if (obj->Variables.elements) {
		gc_markOnly(obj->Variables.elements);
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
			ent code = IrCodeList[i];
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


void markExecutors(ent ptr)
{
	// collect ir code
	switch(ptr->kind){
		case IntArray:{
			dprintf("markExecutors IntArray\n");
			if(ptr->IntArray.elements != NULL){
				gc_markOnly(ptr->IntArray.elements); // mark the int array elements
			}
			dprintf("markExecutors IntArray done\n");
			return;
		}
		case IntQue3:{
			dprintf("markExecutors IntQue3\n");
			int pos = ptr->IntQue3.head;
			for(int i = 0; i < ptr->IntQue3.size; i++){
				if(ptr->IntQue3.que[pos] != NULL){
					gc_markOnly(ptr->IntQue3.que[pos]); // mark the int que elements
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
				gc_markOnly(ptr->Thread.stack); // mark the thread pc
			}
			if(ptr->Thread.queue != NULL){
				gc_mark(ptr->Thread.queue); // mark the thread queue
				gc_markOnly(ptr->Thread.queue); // mark the thread queue itself
			}
			dprintf("markExecutors Thread done\n");
			return;
		}
		case EventHandler:{
			dprintf("markExecutors EventHandler\n");
			if(ptr->EventHandler.data){
				gc_markOnly(ptr->EventHandler.data); // mark the event handler data
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
				gc_mark(ptr->Agent.stack); // mark the agent stack
			}
			if(ptr->Agent.nEvents > 0){
				for(int i = 0; i < ptr->Agent.nEvents; i++){
					if(ptr->Agent.eventHandlers[i]){
						gc_mark(ptr->Agent.eventHandlers[i]); // mark the agent events
					}
				}
			}
			if(ptr->Agent.eventHandlers != NULL){
				gc_markOnly(ptr->Agent.eventHandlers); // mark the agent event handlers
			}
			dprintf("markExecutors Agent done\n");
			return;
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
int compile_init();
int compile_finalize();
ent webcode  = NULL;
ent webagent = NULL;
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
	comctx = ctx    = newGCContext(1024 * 1024); // initialize the garbage collector with 1MB of memory
#else
	#error "MSGCS or MSGC must be defined"
#endif

	return 0; 
}

unsigned int maxNumWebCodes = 1; // maximum number of web codes
ent *webCodes = NULL; // web codes
int nWebCodes = 0; // number of web codes

unsigned int maxNumWebAgents = 1; // maximum number of web agents
ent *webAgents = NULL; // web agents
int nWebAgents = 0; // number of web agents



void collectWeb(void)
{
	assert(ctx == comctx); // check if the context is equal to the compilation context
	gc_markOnly(webCodes); // mark the web codes
	if(nWebCodes > 0) {
		for(int i = 0; i < nWebCodes; ++i){
			if(webCodes[i] != NULL){//manualy mark web codes cause they are called from both side.
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

//WARNING: which ctx you use is important -> comctx
int initWebCodes(int num)
{
	ctx = comctx; // use the context for the garbage collector
	webCodes = (ent *)gc_beAtomic(malloc(sizeof(ent) * num)); // initialize web codes
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

int compile_init()
{
	printf("compile_init\n");
	ctx->nroots = 0; // reset the number of roots
	gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	/* collectEmpty() collect webAgents webCodes*/
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now

	// reset global variables
	initSymbols();
	initSttTrans();

	initLine();
	gc_collect(); // collect garbage

	gc_markFunction = (gc_markFunction_t)markObject; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectObjects; // set the collect function for the garbage collector

    nil   = newUndefine();	gc_pushRoot(nil);
    false = newInteger(0);			gc_pushRoot(false);
    true  = newInteger(1);			gc_pushRoot(true);

	printf("compile_event_init\n");
	compile_event_init(); // initialize the event system
	printf("compile_func_init\n");
	compile_func_init(); // initialize the standard functions
	printf("compile_init done\n");
	return 1;
}

int compile_finalize()
{
	// garbage collection
	rprintf("Running garbage collector...\n");
    gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function for the garbage collector
	gc_collect(); // collect garbage
	return 1; // return 1 to indicate success
}


int compileWebCode(const int doInit,const int index, const char *code)
{
	ctx = comctx; // use the context for the garbage collector
	if(index < 0 || index >= maxNumWebCodes){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	if(doInit)
	{
		ctx = comctx; // use the context for the garbage collector
		gc_collect(); // collect garbage before compiling
	}
	compile_init(); // initialize the compiler
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
	ctx = comctx; // use the context for the garbage collector
	if(num <= 0){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	webAgents = (ent *)gc_beAtomic(malloc(sizeof(ent) * num)); // initialize web agents
	
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
		GC_PUSH(ent,agent,newAgent(0,0)); // create a new agent
		assert(ctx->nroots == 1); // check if the number of roots is equal to 1
		assert(((ent)*ctx->roots[0])->kind == Agent); // check if the root is of type Agent
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
		ent agent = (ent)*ctx->roots[0]; // get the agent from the context memory
		assert(agent->kind == Agent); // check if the agent is of type Agent
		if(agent->Agent.isActive == 0){
			agent = execute(webCodes[i] ,agent , agent);
		}else{
			printf("nEvents: %d\n", agent->Agent.nEvents);
			if(agent->Agent.nEvents == 0){
				printAgent(agent); // print the agent if it has no events
				continue; // skip to the next agent
			}
			for(int j = 0; j < agent->Agent.nEvents; ++j){
				// get event data
				ent eh = agent->Agent.eventHandlers[j];
				EventTable[eh->EventHandler.EventH].eh(eh);
				if(impleBody(webCodes[i], eh, agent)==retFlags[TRANSITION_F]){
					agent->Agent.isActive = 0;
					agent->Agent.pc = intArray_pop(agent->Agent.stack);
					agent->Agent.eventHandlers = NULL;
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
    false = newInteger(0);	gc_pushRoot(false);
    true  = newInteger(1);	gc_pushRoot(true);

	gc_markFunction = (gc_markFunction_t)markObject; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectObjects; // set the collect function for the garbage collector
	rprintf("Compiling code:\n");
	compile_event_init(); // initialize the event system
	compile_func_init(); // initialize the standard functions
	// compile code
	ent code = compile();

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
	runNative(code);


	rprintf("Execution finished.\n");
	gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now
	// free memory
	gc_collect(); 
}
#endif