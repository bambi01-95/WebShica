#ifndef EXECUTOR_C
#define EXECUTOR_C
#include "./executor.h"

#ifdef WEBSHICA
//EXECUTOR 
// ONLY CALL xPRINT
//LOG STYLE:125) 0 type| 4 line | other 
#define LOG_MAX_SIZE 120
static char *strcatlog(char *dest, const char *src){

	if(dest==NULL){
		gc_pushRoot((void*)&src);
		dest = (char*)gc_beAtomic(gc_alloc(sizeof(char)*(LOG_MAX_SIZE)));
		dest[0] = '\0';
		gc_popRoots(1);
		gc_pushRoot((void*)&dest);//should be poped flashPRINT
	}
	if(src==NULL)return dest;
	unsigned int len = 0;
	while(dest[len] != '\0') len++;
	while(*src != '\0'){
		dest[len++] = *src++;
		if(len >= LOG_MAX_SIZE -1) break;
	}
	dest[len] = '\0';
	return dest;
}
oop WebExecs[12] = {
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
};
#endif // WEBSHICA

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






#if WEBSHICA
oop retFlags[7];
void buildRetFlags(){
	for(int i=0;i<7;++i){
		retFlags[i] = newRETFLAG();
	}
	return;
}
#else
oop retFlags[8] = {
	MAKE_FLAG(ERROR_F),
	MAKE_FLAG(FALSE_F),
	MAKE_FLAG(TRUE_F),
	MAKE_FLAG(NONE_F),
	MAKE_FLAG(HALT_F),
	MAKE_FLAG(EOE_F),
	MAKE_FLAG(CONTINUE_F),
	MAKE_FLAG(TRANSITION_F),
};
#endif


oop impleBody(oop exec, oop eh){
	assert(getKind(exec) == RunCtx);
	assert(getKind(eh) == EventHandler);
	oop *threads = getObj(eh, EventHandler, threads);
	oop ret = retFlags[NONE_F];
	for(int i=0;i<getObj(eh, EventHandler, size); ++i){
		oop thread = threads[i];
		if(getObj(thread, Thread, inProgress) == 1){
			ret = execute(exec, thread);
		}else if(getObj(thread, Thread, queue)->Queue.size > 0){
			getObj(thread, Thread, stack) = dequeue(thread);
			assert(getObj(thread, Thread, stack) != NULL);
			ret = execute(exec, thread);
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

oop execute(oop exec, oop entity)
{
#define push(O)	pushStack(stack, O)
#define pop()	popStack(stack)
#define top()	lastStack(stack)
#define pick(I)  stack->Stack.elements[I]
#ifdef DEBUG
	assert(getKind(exec) == RunCtx);
#endif // DEBUG
	dprintf("Execute Start: entity kind %d\n", getKind(entity));
	int opstep = 20; // number of operations to execute before returning
    int* code = getObj(exec->RunCtx.code, IntArray, elements);
	int size = getObj(exec->RunCtx.code, IntArray, size);
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
#ifdef WEBSHICA
char *log_message = 0; // for logging
int locked = 0; // for print functions
#endif 

# define fetch()	code[(*pc)++]

    for (;;) {

#ifdef WEBSHICA
	if (opstep-- <= 0 && !locked) {
		return retFlags[CONTINUE_F]; // return CONTINUE_F to indicate that the execution is not finished
	}
#else
	if (opstep-- <= 0) {
		return retFlags[CONTINUE_F]; // return CONTINUE_F to indicate that the execution is not finished
	}
#endif

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
#ifdef WEBSHICA //DEVICE that cannot suport tagged pointers
	    case iGT:printOP(iGT);    r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l > r));  continue;
		case iGE:printOP(iGE);    r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l >= r)); continue;
		case iEQ:printOP(iEQ);    r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l == r)); continue;
		case iNE:printOP(iNE);    r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l != r)); continue;
		case iLE:printOP(iLE);    r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l <= r)); continue;
		case iLT:printOP(iLT);    r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l < r));  continue;
	    case iADD:printOP(iADD);  r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l + r));  continue;
	    case iSUB:printOP(iSUB);  r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l - r));  continue;
	    case iMUL:printOP(iMUL);  r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l * r));  continue;
	    case iDIV:printOP(iDIV);  r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l / r));  continue;
	    case iMOD:printOP(iMOD);  r = IntVal_value(pop());  l = IntVal_value(pop());  push(newIntVal(l % r));  continue;
#else
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
#endif
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
			push(getObj(getObj(exec, RunCtx, agent), Agent, stack)->Stack.elements[symIndex]);
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
			getObj(getObj(exec, RunCtx, agent), Agent, stack)->Stack.elements[symIndex] = pop(); // set the global variable value
			continue;
		}
		case iSETSTATEVAR:{
			printOP(iSETSTATEVAR);
			oop agent = getObj(exec, RunCtx, agent);
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
			if (IntVal_value(pop()))	 { // if top of stack is TRUE
				continue;
			} else {
				*pc+=l; // skip the jump
			}
			continue;
		}
		case iJUDGE:{
			printOP(iJUDGE);
			l = IntVal_value(pop()); // condition 
			if(!l)return retFlags[FALSE_F];
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
#ifdef WEBSHICA
#define reportMessage(AGENT_INDEX, FMT) _reportError(LOG, AGENT_INDEX, "%s", FMT)
		case iPRINT:{
			printOP(iPRINT);
			char buf[32];
			sprintf(buf, "%d", IntVal_value(pop()));
			log_message = strcatlog(log_message, buf);
			locked = 1;
			continue;
		}
		case fPRINT:{
			printOP(fPRINT);
			oop obj = pop();
			char buf[32];
			sprintf(buf, "%f", FloVal_value(obj));
			log_message = strcatlog(log_message, buf);
			locked = 1;
			continue;
		}
		case sPRINT:{
			printOP(sPRINT);
			oop obj = pop();
			char *str = StrVal_value(obj);
			log_message = strcatlog(log_message, str);
			locked = 1;
			continue;
		}
		case flashPRINT:{
			reportMessage(getCurrentAgentIndex(),log_message);
			gc_popRoots(1); // pop log_message (pushed from strcatlog)
			locked = 0;
			log_message = 0;
			continue;
		}
#undef reportMessage
#else
		case iPRINT:{
			printOP(iPRINT);
			int val = IntVal_value(pop());
			printf("%d", val);
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
#endif // Print
		case iTRANSITION:{
			printOP(iTRANSITION);
			int nextStatePos = fetch();
			pushStack(getObj(getObj(exec, RunCtx, agent), Agent, stack),  newIntVal(nextStatePos+(*pc)));//relative position: 
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
				oop instance = NULL;
				switch(op){
					case iGETGLOBALVAR:{
						l = fetch();
						instance = entity->Agent.stack->Stack.elements[l];
						//instance = getObj(getObj(exec, RunCtx, agent), Agent, stack)->Stack.elements[l];
						op = fetch();
						break;
					}
					case iGETSTATEVAR:{
						l = fetch();
						instance = entity->Agent.stack->Stack.elements[entity->Agent.rbp + l];		
						//instance = getObj(getObj(exec, RunCtx, agent), Agent, stack)->Stack.elements[getObj(getObj(exec, RunCtx, agent), Agent, rbp) + l];
						op = fetch();
						break;
					}
					case iSETEVENT:
						// do nothing, continue to the next step
						break;
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
				if(instance == NULL){
					EventTable[eventID].init(ehs[i]);// initialize the event handler data (std event object)
				}else{
					assert(instance->kind == Instance);
					ehs[i]->EventHandler.data[0]/*event object eh data[0] should be hold instance data*/ = instance;
					EventTable[eventID].init(ehs[i]);// initialize the event handler data (std event object)
				}
				for(int j=0; j<nThreads; ++j){
					int opPos = *pc;
					op = fetch();
					assert(op == iSETPROCESS);
					printOP(iSETPROCESS);
					l = opPos + fetch(); // get the aPos
					r = opPos + fetch(); // get the cPos
					ehs[i]->EventHandler.threads[j] = newThread(l,r,eventID); // initialize the thread
				}
			}
			op = fetch();
			assert(op == iIMPL);
			printOP(iIMPL);
			entity->Agent.isActive = 1; // set the agent to active
			return exec; // return the entity
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
			return retFlags[TRUE_F]; // return EOC_F to indicate end of code
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
				dprintf("%3d:<%p>\n",i, (stack->Stack.elements[i]));
			}
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
#undef push
#undef pop
#undef top
#undef pick
}
#endif // EXECUTOR_C