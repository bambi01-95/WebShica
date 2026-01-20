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
#include "./Object/object.h"
#include "./Executor/executor.h"
#include "./Tool/tool.h"
#include "./File/file.h"

#include "./Platform/Linux/Library/library.h"


void markEmpty(void* ptr){ return;}
void collectEmpty(void){ return; }


void collectExecutors(void)
{
	return;
}

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
				ExecEventTable[getObj(eh, EventHandler, EventH)].eh(exec, eh);
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
	gc_init(1024 * 1024);// initialize the garbage collector with 1MB of memory 
	gc_markFunction = (gc_markFunction_t)markExecutors; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectExecutors; // set the collect function for the garbage collector

	ExecEventTable = __ExecEventTable__; // initialize the ExecEventTable
	ExecEventObjectTable = __ExecEventObjectTable__; // initialize the ExecEventObjectTable
	ExecStdFuncTable = __ExecStdFuncTable__; // initialize the ExecStdFuncTable
	char* filename = "shica.stt"; // default filename
	if(argc > 1){
		filename = argv[1]; // get the filename from the command line arguments
	}
	// load bytecode
	memoryRead(filename);
	oop code = intArray_init();
	code->IntArray.size = memoryCastIntSize();
	code->IntArray.elements = memoryCastIntAddr();
#ifdef DEBUG
	printf("Loading bytecode...\n");
	printCode(code); // print the bytecode
#endif 

	// execute code
#ifdef MSGC
	assert(nroots == 0); // check if the number of roots is equal to 0
#elif defined(MSGCS)
	assert(getOriginalGCtxNRoots() == 0); // check if the number of roots is equal to 0
#else
	#error "MSGCS or MSGC must be defined"
#endif

	gc_pushRoot(code);
	int ret = runNative(code);

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