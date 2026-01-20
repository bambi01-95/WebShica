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
#include "./Tool/tool.h"

#include "./Platform/Linux/Library/library.h"

void markEmpty(void* ptr){ return;}
void collectEmpty(void){ return; }

/*===============COMPILE MARKER=============*/

/* ==================================*/

void collectObjects(void)	// pre-collection funciton to mark all the symbols
{
	collectSymbols();
	collectSttTrans();
    collectYYContext();
}


int main(int argc, char **argv)
{
	int opt_c = 0;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-c") == 0){
			opt_c = 1;
		}
	}
//GC INIT
	gc_init(1024 * 1024);// initialize the garbage collector with 1MB of memory 
	gc_markFunction = (gc_markFunction_t)markObject; // set the mark function for the garbage collector
	gc_collectFunction = (gc_collectFunction_t)collectObjects; // set the collect function for the garbage collector
//GC INIT UNITIALIZE
    nil   = newUndefine();	gc_pushRoot((void*)&nil);
	FALSE = newInteger(0);	gc_pushRoot((void*)&FALSE);
    TRUE  = newInteger(1);	gc_pushRoot((void*)&TRUE);

	entryEH = intern("entryEH");
	entryEH->Symbol.value = newEventH(0); // 0 argument
	exitEH = intern("exitEH");
	exitEH->Symbol.value =  newEventH(0); // 0 argument

    CompEventTable = __CompEventTable__;
    CompStdFuncTable = __CompStdFuncTable__;
    CompEventObjectTable = __CompEventObjectTable__;
	compile_event_init(); // initialize the event system
    compile_eh_init(); // initialize the event system
	compile_func_init(); // initialize the standard functions
	compile_eo_init(); // initialize the eo functions

	// compile code
	node code = compile();

	if(code == NULL){
		dprintf("Compilation failed.\n");
		printErrorList(); // print the error list
		return 1; // return 1 to indicate failure
	}

	// print bytecode 
	dprintf("Print IR code:\n");
	printCode(code);

	gc_collect();
    return 0;
}