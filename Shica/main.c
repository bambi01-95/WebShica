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
#include "./Compiler/compiler.h"
#include "./Object/object.h"
#include "./Executor/executor.h"
#include "./Tool/tool.h"

#ifdef WEBSHICA

#include "./Platform/WebShica/Library/library.h"
#else // LINUX
#include "./Platform/Linux/Library/library.h"
#endif





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
		case Queue:{
			dprintf("markExecutors Queue\n");
			int pos = ptr->Queue.head;
			for(int i = 0; i < ptr->Queue.size; i++){
				if(ptr->Queue.que[pos] != NULL){
					gc_mark(ptr->Queue.que[pos]); 
				}
				pos = (pos + 1) % ptr->Queue.capacity;
			}
			gc_markOnly(ptr->Queue.que); // mark the queue elements
			dprintf("markExecutors Queue done\n");
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
		case RunCtx:{
			dprintf("markExecutors RunCtx\n");
			if(ptr->RunCtx.agent){
				gc_mark(ptr->RunCtx.agent); // mark the runctx agent
			}
			if(ptr->RunCtx.code){
				gc_mark(ptr->RunCtx.code); // mark the runctx code
			}
			dprintf("markExecutors RunCtx done\n");
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
int compile_init(int index);
int compile_finalize();
oop webcode  = NULL;
oop webagent = NULL;
static gc_context *compiler_gc_ctx = NULL; // context for the garbage collector

//once called, initialize the memory for the garbage collector
int memory_init(void)
{
#ifdef MSGC
	gc_init(1024 * 1024); // initialize the garbage collector with 1MB of memory
#elif defined(MSGCS)
	gc_init(1024 * 1024 * 4); // initialize the garbage collector with 4MB of memory
	compiler_gc_ctx = current_gc_ctx  = newGCContext(1024 * 1024); // initialize the garbage collector with 1MB of memory
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




void collectWeb(void)
{
	assert(current_gc_ctx == compiler_gc_ctx); // check if the context is equal to the compilation context
	gc_markOnly(webCodes); // mark the web codes
	dprintf("collectWeb: nWebCodes=%d, size %lu\n", nWebCodes, sizeof(oop) * maxNumWebCodes);
	if(nWebCodes > 0) {
		for(int i = 0; i < nWebCodes; ++i){
			if(webCodes[i] != NULL){//manualy mark web codes cause they are called from both side.
				dprintf("\tmarking web code %d, size %lu\n", i, sizeof(int) * (webCodes[i]->IntArray.size));
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
//WARNING: which current_gc_ctx you use is important -> compiler_gc_ctx
int initWebCodes(int num)
{
	current_gc_ctx = compiler_gc_ctx; // use the context for the garbage collector
	webCodes = (oop *)gc_beAtomic(malloc(sizeof(oop) * num)); // initialize web codes
	maxNumWebCodes = num; // set the maximum number of web codes
	return 0; 
}

int addWebCode(void)
{
	assert(current_gc_ctx == compiler_gc_ctx); // check if the context is equal to the compilation context
	if(nWebCodes >= maxNumWebCodes){
		dprintf("%s %d contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	webCodes[nWebCodes++] = NULL; // add a new web code
	return 0; 
}

int compile_init(int index)
{
	dprintf("compile_init\n");
	webCodes[index] = NULL; // initialize the web code
	current_gc_ctx = compiler_gc_ctx; // use the context for the garbage collector
	current_gc_ctx->nroots = 0; // reset the number of roots
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

	dprintf("compile_event_init\n");
	compile_event_init(); // initialize the event system
	dprintf("compile_func_init\n");
	compile_func_init(); // initialize the standard functions
	dprintf("compile_eo_init\n");
	compile_eo_init(); // initialize the eo system
	dprintf("compile_init done\n");
	return 1;
}

int compile_finalize()
{
	// garbage collection
	dprintf("Running garbage collector...\n");
    gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now
	gc_collect(); // collect garbage
	return 1; // return 1 to indicate success
}

//CCALL
int compileWebCode(const int index, const char *code)
{
	dprintf("nroot: %d\n", current_gc_ctx->nroots);
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
	dprintf("Printing bytecode for web code %d:\n", index);
#if DEBUG
	printCode(webCodes[index]);
#endif
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


//WARNING: which current_gc_ctx you use is important -> compiler_gc_ctx
int initWebAgents(int num)
{
	current_gc_ctx = compiler_gc_ctx; // use the context for the garbage collector
	executor_event_init(); // initialize the event system for the executor
	executor_func_init(); // initialize the standard functions for the executor
	buildRetFlags(); // build the return flags for the executor
	if(num <= 0){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	for(int i = 0; i<num; ++i)
	{
		WebExecs[i] = newRunCtx(NULL, webCodes[i]); // create a new run context for each web agent
	}
	
	printf("Initializing web %d agents...\n", num);
	gc_markFunction = (gc_markFunction_t)markExecutors; // set the mark function for the garbage collector
    gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector
	current_gc_ctx = origin_gc_ctx_ptr(); // use the context for execution
	memset(current_gc_ctx->memory, 0, (char*)current_gc_ctx->memend - (char*)current_gc_ctx->memory); // clear the memory
	gc_separateContext(num,0); // separate context for the garbage collector
	assert(num == current_gc_ctx->nroots); // check if the number of roots is equal to the number of web agents
	gc_context **ctxs = (gc_context **)current_gc_ctx->roots; // initialize web agents
	printf("\x1b[34m[C] nRoots: %d\x1b[0m\n\n", current_gc_ctx->nroots);
	for(int i = 0; i<num; ++i)
	{
		current_gc_ctx = ctxs[i];
		current_gc_ctx->nroots = 0; // reset the number of roots
		#ifdef DEBUG
		printf("\x1b[34m[C] Agent[%d]: Context size %ld: %p -> %p\n\x1b[0m", i, (char*)current_gc_ctx->memend - (char*)current_gc_ctx->memory, current_gc_ctx->memory, current_gc_ctx->memend);
		#endif
		oop *slot = (oop *)gc_alloc(sizeof(oop)); // allocate memory for the agent
		*slot = newAgent(0,0); // create a new agent
		current_gc_ctx->roots[current_gc_ctx->nroots++] = (void *)slot; // add the agent to the roots
		oop agent = (oop)*current_gc_ctx->roots[0];
		assert(getKind(agent) == Agent); // check if the agent is of type Agent
		assert(current_gc_ctx->nroots == 1); // check if the number of roots is equal to 1
		WebExecs[i]->RunCtx.agent = agent; // set the agent in the run context
	}
	maxNumWebAgents = num; // set the maximum number of web agents
	printf("Initialized %d web agents.\n", num);
	nWebAgents = num; // set the number of web agents
	current_gc_ctx = origin_gc_ctx_ptr(); // reset the context to the execution context
	return 0; 
}
//WARNING: which current_gc_ctx you use is important -> origin_gc_ctx_ptr() (initWebAgents)
int executeWebCodes(void)
{
	gc_context **ctxs = (gc_context **)current_gc_ctx->roots; // initialize web agents
	int _ret = collision_calculation(nWebAgents);
	oop ret = retFlags[ERROR_F];
	for(int i = 0; i<nWebAgents ; i++){
#ifdef DEBUG
		printf("\n\x1b[34m[C] Agendd[%d] ----------------- \x1b[0m\n", i);
#endif
		setActiveAgent(i); // set the agent as active
		current_gc_ctx = ctxs[i]; // set the context to the current web agent
		oop agent = (oop)*current_gc_ctx->roots[0]; // get the agent from the context memory
		if(agent==retFlags[ERROR_F])continue;
		// Wen you want to ...
		// printf("\x1b[34m[C] Agent[%d] -> agent[%p] = memory[%p]\x1b[0m\n", i, agent, current_gc_ctx->roots[0]);
		if(getObj(agent, Agent, isActive) == 0){
			// WebExecs[i] =
			ret = execute(WebExecs[i], agent);
			if(ret==retFlags[TRANSITION_F]){
				for(int k = 0; k < getObj(agent, Agent, nEvents); ++k){
					oop eh2 = getObj(agent, Agent, eventHandlers)[k];
					reinitializeEventObject(eh2);
				}
				getObj(agent, Agent, isActive) = 0;
				getObj(agent, Agent, pc) = IntVal_value(popStack(getObj(agent, Agent, stack)));
				getObj(agent, Agent, eventHandlers) = NULL;
				continue;
			}
			if(ret==retFlags[ERROR_F]){
				continue;
			}
			if(ret==retFlags[HALT_F]){
				continue;
			}
		}else{
			for(int j = 0; j < getObj(agent, Agent, nEvents); ++j){
				// get event data
				oop eh = getObj(agent, Agent, eventHandlers)[j];
				EventTable[getObj(eh, EventHandler, EventH)].eh(WebExecs[i], eh);
				ret = impleBody(WebExecs[i], eh);
				if(ret==retFlags[TRANSITION_F]){
					for(int k = 0; k < getObj(agent, Agent, nEvents); ++k){
						oop eh2 = getObj(agent, Agent, eventHandlers)[k];
						reinitializeEventObject(eh2);
					}
					getObj(agent, Agent, isActive) = 0;
					getObj(agent, Agent, pc) = IntVal_value(popStack(getObj(agent, Agent, stack)));
					getObj(agent, Agent, eventHandlers) = NULL;
					break;
				}
			}
		}
		// printCode(webCodes[i]); // print the bytecode of the web code
	}
	current_gc_ctx = origin_gc_ctx_ptr(); // reset the context to the execution context
	return 0; 
}
int stopWebCodes(void)
{
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
int runNative(oop code){
	GC_PUSH(oop, agent, newAgent(0,0));//nroos[0] = agent; // set the first root to the agent
	oop exec = newRunCtx(agent, code);
	GC_POP(agent);
	gc_pushRoot((void*)&exec);
	dprintf("agent: %d\n", getObj(agent, Agent, isActive));
	oop ret = retFlags[ERROR_F];
	while(1){
		if(getObj(agent, Agent, isActive) == 0) {
			ret = execute(exec, agent);
			if(ret == retFlags[TRANSITION_F]){
				//initialize event objects
				for(int k = 0; k < getObj(agent, Agent, nEvents); ++k){
					oop eh2 = getObj(agent, Agent, eventHandlers)[k];
					reinitializeEventObject(eh2);
				}
				getObj(agent,Agent,isActive) = 0;
				getObj(agent, Agent, pc) = IntVal_value(popStack(getObj(agent, Agent, stack)));
				getObj(agent, Agent, eventHandlers) = NULL;
				continue;
			}
			if(ret == retFlags[ERROR_F]){
				GC_POP(exec);
				return 1; // return 1 to indicate error
			}
			if(ret == retFlags[HALT_F]){
				GC_POP(exec);
				return 0; // return 0 to indicate success
			}
		}else{
			agent = exec->RunCtx.agent;
			for(int i = 0; i< getObj(agent,Agent,nEvents); ++i){
				// get event data
				oop eh = getObj(agent,Agent,eventHandlers)[i];
				EventTable[getObj(eh, EventHandler, EventH)].eh(exec, eh);
				ret = impleBody(exec, eh);
				if(ret == retFlags[TRANSITION_F]){
					//initialize event objects
					for(int k = 0; k < getObj(agent, Agent, nEvents); ++k){
						oop eh2 = getObj(agent, Agent, eventHandlers)[k];
						reinitializeEventObject(eh2);
					}
					getObj(agent,Agent,isActive) = 0;
					getObj(agent, Agent, pc) = IntVal_value(popStack(getObj(agent, Agent, stack)));
					getObj(agent, Agent, eventHandlers) = NULL;
					break;
				}
				if(ret == retFlags[ERROR_F]){
					GC_POP(exec);
					return 1; // return 1 to indicate error
				}
				if(ret == retFlags[HALT_F]){
					GC_POP(exec);
					return 0; // return 0 to indicate success
				}
			}
		}
	}
	GC_POP(exec);
	return 0; // return 0 to indicate success
}

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
	dprintf("Compiling code:\n");
	compile_event_init(); // initialize the event system
	compile_func_init(); // initialize the standard functions
	compile_eo_init(); // initialize the eo functions

	// compile code
	oop code = compile();

	if(code == NULL){
		dprintf("Compilation failed.\n");
		printErrorList(); // print the error list
		return 1; // return 1 to indicate failure
	}

	// print bytecode 
	dprintf("Print IR code:\n");
	printCode(code);

	// garbage collection
	dprintf("Running garbage collector...\n");
    gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector
	initYYContext();// initialize the yycontext
#ifdef MSGC
	nroots = 0; // reset the number of roots
#elif defined(MSGCS)
	setOriginalGCtxNRoots(0); // reset the number of roots
#else
	#error "MSGCS or MSGC must be defined"
#endif
	gc_markOnly(code); // mark the code itself
	gc_markOnly(code->IntArray.elements); // mark the code elements
	gc_collect();


	if(opt_c == 1){
		dprintf("end of compilation\n");
		return 0;
	}
	printCode(code); // print the bytecode



	gc_markFunction = (gc_markFunction_t)markExecutors; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector

	// execute code
	dprintf("Executing code...\n");
	executor_event_init(); // initialize the event system for the executor
	executor_func_init(); // initialize the standard functions for the executor
/*
 * CHECK: if assertion is happening, please check the initialization of above functions
*/
#ifdef MSGC
	assert(nroots == 0); // check if the number of roots is equal to 0
#elif defined(MSGCS)
	assert(getOriginalGCtxNRoots() == 0); // check if the number of roots is equal to 0
#else
	#error "MSGCS or MSGC must be defined"
#endif
	gc_pushRoot(code);
	printf("\tExecuting...\n");
	int ret = runNative(code);
	printf("\tExecution done.\n");

	if(ret){
		dprintf("Execution error.\n");
		printErrorList(); // print the error list
		return 1; // return 1 to indicate failure
	}
	
	dprintf("Execution finished.\n");
	gc_markFunction = (gc_markFunction_t)markEmpty; // set the mark function to empty for now
	gc_collectFunction = (gc_collectFunction_t)collectEmpty; // set the collect function to empty for now
	// free memory
	gc_collect(); 
}
#endif