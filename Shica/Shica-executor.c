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

//CCALL
//index starts from 1
int addWebCode(int index)
{
	current_gc_ctx = compiler_gc_ctx; // use the context for the garbage collector
	if(index <= 0 || index > maxNumWebCodes){
		printf("%s %d: contact the developer %s\n", __FILE__, __LINE__, DEVELOPER_EMAIL);
		reportError(DEVELOPER, 0, "out of range compiler.");
		return 1; // return 1 to indicate failure
	}
	webCodes[index -1] = NULL; // add a new web code
	for(int i = index; i<maxNumWebCodes;i++)webCodes[i] = NULL; // initialize the web code to NULL
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
		if(WebExecs[i]==retFlags[ERROR_F] || WebExecs[i]==retFlags[HALT_F])continue;
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
				WebExecs[i] = retFlags[ERROR_F];
				continue;
			}
			if(ret==retFlags[HALT_F]){
				WebExecs[i] = retFlags[HALT_F];
				continue;
			}
		}else{
			for(int j = 0; j < getObj(agent, Agent, nEvents); ++j){
				// get event data
				oop eh = getObj(agent, Agent, eventHandlers)[j];
				EventTable[getObj(eh, EventHandler, EventH)].eh(WebExecs[i], eh);
				ret = impleBody(WebExecs[i], eh);
				if(ret == retFlags[EOE_F] || ret == retFlags[CONTINUE_F]){
					continue; // continue to the next event handler
				}
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
				if(ret==retFlags[ERROR_F]){
					WebExecs[i] = retFlags[ERROR_F];
					break;
				}
				if(ret==retFlags[HALT_F]){
					WebExecs[i] = retFlags[HALT_F];
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