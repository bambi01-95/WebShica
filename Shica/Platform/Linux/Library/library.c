// Linux Standard Library
#ifndef LIBRARY_C
#define LIBRARY_C
#include "library.h"

//-------------------------------
// COMMON CODE
//-------------------------------
// Event Handler Types
enum{
// Standard Event Handlers
	EVENT_EH = 0x00, // Loop Handler
	TIMER_EH, // Timer Handler
//Event Object Event Handlers
	CHAT_RECEIVED_EH, // WebRTC Broadcast Event Object
	T_TIMER_SEC_EH, // Timer second event handler
	T_TIMER_MIN_EH, // Timer minute event handler
	T_TIMER_HOUR_EH, // Timer hour event handler

	#ifdef RPI // Raspberry Pi specific event handlers (from 0x08:  decimal)
	PI_GPIO_0_31_EH, //= 0x08, // Raspberry Pi GPIO 0-31 Event Handler
	PI_GPIO_EO_N_EH, // Raspberry Pi  GPIO Event object (PIN N) Event Handler
	#endif
	END_EH, /* DO NOT REMOVE THIS LINE */
} EventHandlerType;
// Standard Library Function Types
 enum {
	EXIT_FUNC=0,   // exit function
	MATH_SQRT_FUNC, // math sqrt function
	CHAT_SEND_FUNC, // chat send function
	CHAT_POST_FUNC, // chat post function
	CHAT_SELF_FUNC, // chat self function
	T_TIMER_RESET_FUNC, // timer reset function
	#ifdef RPI // Raspberry Pi specific functions (from 0x08:  decimal)
	PI_GPIO_SET_OUTPUT_FUNC, //= 0x08, // Raspberry Pi GPIO set output function
	PI_GPIO_WRITE_FUNC, // Raspberry Pi GPIO write function
	PI_GPIO_EO_WRITE_FUNC, // Raspberry Pi GPIO Event Object write function
	#endif
} StdLibFuncType;
// Event Object Types
enum {
	CHAT_BROADCAST_EO = 0x00, // WebRTC Broadcast Event Object
	TIME_EO, // Timer Event Object
	END_EO, /* DO NOT REMOVE THIS LINE */
	#ifdef RPI // Raspberry Pi specific event objects (from 0x08:  decimal)
	PI_GPIO_EO, //= 0x08, // Raspberry Pi GPIO Event Object
	#endif
} EventObjectType;



//-------------------------------
// SHICA COMPILER CODE
//-------------------------------
#ifdef SHICACOMP
int compile_eh_init(){
	//standard event handler
	node EH = NULL;
	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH); // 1 argument
    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH); // 1 argument

#ifdef RPI
	EH = intern("pi_gpio_0_31_EH");
	EH->Symbol.value = newEventH(PI_GPIO_0_31_EH); // 2 arguments
#endif
	return 0; // return 0 to indicate success
}
//nArgs, argTypes, nFuncs
//-------------------------------
struct CompEventTable __CompEventTable__[] = {
	[EVENT_EH] = {0, NULL, 0},      // EVENT_EH
	[TIMER_EH] = {1,(char []){Integer}, 2},      // TIMER_EH
	[CHAT_RECEIVED_EH] = {2,(char []){String/*sender*/, String/*msg*/}, 3},      // CHAT_RECEIVED_EH
	[T_TIMER_SEC_EH] = {1,(char []){Integer}, 1},      // T_TIMER_SEC_EH
	[T_TIMER_MIN_EH] = {1,(char []){Integer}, 1},      // T_TIMER_MIN_EH
	[T_TIMER_HOUR_EH] = {1,(char []){Integer}, 1},      // T_TIMER_HOUR_EH
#ifdef RPI
	[PI_GPIO_0_31_EH] = {1,(char []){Integer}, 1},      // PI_GPIO_0_31_EH
	[PI_GPIO_EO_N_EH] = {1,(char []){Integer}, 1},      // PI_GPIO_EO_N_EH
#endif
};

// Standard Library Functions
int compile_func_init()
{
	node FUNC = NULL;
	FUNC = intern("exit");
	FUNC->Symbol.value = newStdFunc(EXIT_FUNC); // exit function

	FUNC = intern("sqrt");
	FUNC->Symbol.value = newStdFunc(MATH_SQRT_FUNC); // math sqrt function
#ifdef RPI
	FUNC = intern("gpioSetOutput");
	FUNC->Symbol.value = newStdFunc(PI_GPIO_SET_OUTPUT_FUNC); // Raspberry Pi GPIO set output function
	FUNC = intern("gpioWrite");
	FUNC->Symbol.value = newStdFunc(PI_GPIO_WRITE_FUNC); // Raspberry Pi GPIO write function
#endif
	return 0; // return 0 to indicate success
}
struct CompStdFuncTable __CompStdFuncTable__[] = {
	[EXIT_FUNC] = {1, (int[]){Integer}, Undefined}, // exit function takes 1 argument
	[CHAT_SEND_FUNC] = {1, (int[]){String/*msg*/}, Undefined}, // chat send function takes 2 arguments
	[CHAT_POST_FUNC] = {1, (int[]){String/*msg*/}, Undefined}, // chat post function takes 2 arguments
	[CHAT_SELF_FUNC] = {0, NULL, String/*id*/}, // chat self function takes 0 arguments and returns a string
	[MATH_SQRT_FUNC] = {1, (int[]){Integer}, Integer}, // math sqrt function takes 1 argument and returns an integer
	[T_TIMER_RESET_FUNC] = {1, (int[]){Integer}, Undefined}, // timer reset function takes 1 argument
#ifdef RPI
	[PI_GPIO_SET_OUTPUT_FUNC] = {1, (int[]){Integer/*pin*/}, Undefined}, // Raspberry Pi GPIO set output function takes 2 arguments
	[PI_GPIO_WRITE_FUNC] = {2, (int[]){Integer/*pin*/, Integer/*value*/}, Undefined}, // Raspberry Pi GPIO write function takes 2 arguments
	[PI_GPIO_EO_WRITE_FUNC] = {1, (int[]){Integer/*value*/}, Undefined}, // Raspberry Pi GPIO Event Object write function takes 2 arguments
#endif
};
//-------------------------------
// Event Object Table
//-------------------------------
int compile_eo_init(){
	node sym = NULL;
	node func = NULL;
	node eo = NULL;
// Broadcast Event Object
	sym = intern("broadcastEO");
	eo = newEventObject(sym, CHAT_BROADCAST_EO);// var chat = broadcast(channel, password);
	func = newEventH(CHAT_RECEIVED_EH);sym = newSymbol("receivedEH");
	putFuncToEo(eo, func, sym, 0);// chat.received(sender, msg);
	func = newStdFunc(CHAT_SEND_FUNC);sym = newSymbol("send");
	putFuncToEo(eo, func, sym, 1); // chat.send(msg, recipient);
	func = newStdFunc(CHAT_POST_FUNC);sym = newSymbol("post");
	putFuncToEo(eo, func, sym, 2); // chat.post(msg);
	func = newStdFunc(CHAT_SELF_FUNC);sym = newSymbol("self");
	putFuncToEo(eo, func, sym, 3); // chat.self();
// Timer Event Object
	sym = intern("timerEO");
	eo = newEventObject(sym, TIME_EO);// var t = timer(interval, label);
	func = newEventH(T_TIMER_SEC_EH);sym = newSymbol("secEH");
	putFuncToEo(eo, func, sym, 0);// t.sec(second);
	func = newEventH(T_TIMER_MIN_EH);sym = newSymbol("minEH");
	putFuncToEo(eo, func, sym, 1);// t.min(minute);
	func = newEventH(T_TIMER_HOUR_EH);sym = newSymbol("hourEH");
	putFuncToEo(eo, func, sym, 2);// t.hour(hour);
	func = newStdFunc(T_TIMER_RESET_FUNC);sym = newSymbol("reset");
	putFuncToEo(eo, func, sym, 3);// t.x();
// Other Event Objects can be added here
#ifdef RPI
// Raspberry Pi GPIO Event Object
	sym = intern("gpioEO");
	eo = newEventObject(sym, PI_GPIO_EO);// var gpio = pi_gpio(pin);
	func = newEventH(PI_GPIO_EO_N_EH);sym = newSymbol("readEH");
	putFuncToEo(eo, func, sym, 0);// gpio.readEH(value);
	func = newStdFunc(PI_GPIO_EO_WRITE_FUNC);sym = newSymbol("write");
	putFuncToEo(eo, func, sym, 1);// gpio.write(value);
#endif
	return 0;
}

	// int nArgs; // number of arguments
	// int nFuncs; // number of functions
	// int *argTypes; // types of arguments
struct CompEventObjectTable  __CompEventObjectTable__[] = {
	[CHAT_BROADCAST_EO] = {2, 4, (int[]){String/*channel*/, String/*password*/}}, // WebRTC broadcast event object with 3 arguments and 1 function
	[TIME_EO] = {0, 4, NULL}, // Timer event object with 0 arguments and 4 functions
#ifdef RPI
	[PI_GPIO_EO] = {3, 2, (int[]){Integer/*pin*/, Integer/*mode*/, Integer/*init: pud|vol*/}}, // Raspberry Pi GPIO event object with 3 arguments and 2 functions
#endif
};

//When multiple libraries is needed and think aobut this.
// //Compiler
// //Executor mapping
// const int index_adapt(int index){
// 	switch(index){
// 		case EVENT_EH: return EVENT_EH;
// 		case TIMER_EH: return TIMER_EH;
// 		case T_TIMER_SEC_EH: return T_TIMER_SEC_EH;
// 		case T_TIMER_MIN_EH: return T_TIMER_MIN_EH;
// 		case T_TIMER_HOUR_EH: return T_TIMER_HOUR_EH;
// 		#ifdef RPI
// 		case PI_GPIO_0_31_EH: return PI_GPIO_0_31_EH;
// 		case PI_GPIO_EO_N_EH: return PI_GPIO_EO_N_EH;
// 		#endif
// 		default:
// 			fatal("file %s line %d index_adapt: unknown event handler index %d", __FILE__, __LINE__, index);
// 			return -1;
// 		}
// 	return 0;
// }

#endif // SHICACOMP



//-------------------------------+
// SHICA EXECUTER CODE.          |
//-------------------------------+



#ifdef SHICAEXEC
#include <math.h>
#include <time.h>
// Event Handlers and its initializers
int event_handler(oop exec, oop eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		oop thread = eh->EventHandler.threads[0];
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, newIntVal(0))); // enqueue a stack with value 0
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(oop eh){
	return 1;
}
int event_object_handler_init(oop eh){
	return 1;
}

int timer_handler(oop exec, oop eh){
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	if(now - IntVal_value(eh->EventHandler.data[0]) >= 1){
		eh->EventHandler.data[0] = newIntVal(now);
		eh->EventHandler.data[1] = newIntVal(IntVal_value(eh->EventHandler.data[1]) + 1); // increment count
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, eh->EventHandler.data[1])); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0; // return 0 to indicate no event
}
int timer_handler_init(oop eh){//normal timer handler (not EO)
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	eh->EventHandler.data[0] = newIntVal(now); //start time
	eh->EventHandler.data[1] = newIntVal(0);   //count
	return 1;
}

//EO Event Handlers
int timer_sec_handler(oop exec, oop eh){
	oop instance = eh->EventHandler.data[0];
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);
	int interval = IntVal_value(obj);

	int pre = IntVal_value(fields[2]);	
	time_t t = time(NULL);
	int now = (int)(t % 10000);

	if(now - pre >= interval){
		fields[2] = newIntVal(now);
		fields[1] = newIntVal(IntVal_value(fields[1]) + 1); // increment count
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, fields[1])); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
int timer_min_handler(oop exec, oop eh){
	return 0;
}
int timer_hour_handler(oop exec, oop eh){
	return 0;
}

#ifdef RPI
#include <pigpio.h>
int rpi_gpioRead_0_31event_handler(oop exec, oop eh){
	uint32_t gpio_states = gpioRead_Bits_0_31();
	if(gpio_states != 0){
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, newIntVal((int)gpio_states))); // enqueue a stack with gpio states
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
int rpi_gpioRead_N_event_handler_init(oop eh){
	oop instance = eh->EventHandler.data[0];
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);
	int pin = IntVal_value(obj);

	int level = gpioRead(pin);
	if(level){
		oop stack = newStack(0);
		enqueue(exec, eh, pushStack(stack, newIntVal(level))); // enqueue a stack with gpio level
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
#endif // RPI

// Event Handler Table
	// int (*eh)(oop exec, oop eh); // event handler function
	// int (*init)(oop eh); // initialize function
	// int nArgs; // number of arguments
	// int nData; // number of data fields
struct ExecEventTable  __ExecEventTable__[] = {
	[EVENT_EH] = {event_handler,      event_handler_init,0,0 },      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init,1,2 },      // TIMER_EH
	[T_TIMER_SEC_EH] = {timer_sec_handler,      event_object_handler_init,1,1 },      // T_TIMER_SEC_EH
	[T_TIMER_MIN_EH] = {timer_min_handler,    event_object_handler_init,1,1 },      // T_TIMER_MIN_EH
	[T_TIMER_HOUR_EH] = {timer_hour_handler,    event_object_handler_init,1,1 },      // T_TIMER_HOUR_EH
#ifdef RPI
	[PI_GPIO_0_31_EH] = {rpi_gpioRead_0_31event_handler,      event_handler_init,2,0 },      // PI_GPIO_0_31_EH
	[PI_GPIO_EO_N_EH] = {rpi_gpioRead_N_event_handler,      rpi_gpioRead_N_event_handler_init,1,1 },		  // PI_GPIO_EO_N_EH
#endif
};
//-------------------------------
// Standard Library Functions
//-------------------------------
int lib_exit(oop stack)
{
	int status = intArray_pop(stack); // get exit status from stack
	exit(status); // exit with the given status
	return 0; // return 0 to indicate success
}

int lib_chat_send(oop stack)
{
	oop chat = popStack(stack); // get chat object from stack
	char *msg = StrVal_value(popStack(stack)); // get message from stack
	int recipient = IntVal_value(popStack(stack)); // get recipient from stack
	return 0; // return 0 to indicate success
}

int lib_math_sqrt(oop stack)
{
	int value = IntVal_value(popStack(stack)); // get value from stack
	int result = (int)sqrt((double)value); // calculate square root
	pushStack(stack, newIntVal(result)); // push result back to stack
	return 0; // return 0 to indicate success
}

int lib_timer_reset(oop stack)
{
	GC_PUSH(oop, initVal, popStack(stack)); // get initial value from stack (IntVal)
	oop instance = popStack(stack); // get timer object from stack
	printf("timer reset called with initVal: %d\n", IntVal_value(initVal));
	assert(getKind(instance) == Instance);
	assert(getKind(initVal) == IntVal);
	//DEBUG POINT
	getInstanceField(instance, 0) = newIntVal(1); // reset interval to 0
	getInstanceField(instance, 1) = initVal; // reset the timer to initial value
	time_t t = time(NULL);
	getInstanceField(instance, 2) = newIntVal((int)(t % 10000)); // reset label to current time
	GC_POP(initVal);
	return 0;
}
#ifdef RPI
oop lib_pi_gpio_set_output(oop stack){
	int pin = IntVal_value(popStack(stack));
	gpioSetMode(pin, PI_OUTPUT);
	return 0;
}
oop lib_pi_gpio_write(oop stack){
	int pin = IntVal_value(popStack(stack));
	int value = IntVal_value(popStack(stack));
	gpioWrite(pin, value);
	return 0;
}
oop lib_pi_gpio_eo_write(oop stack){
	oop instance = popStack(stack);
	assert(instance->kind == Instance);
	oop* fields = getObj(instance, Instance, fields);
	assert(fields != NULL);

	oop obj = fields[0];
	assert(obj != NULL);
	assert(getKind(obj) == IntVal);
	int pin = IntVal_value(obj);
	int mode = IntVal_value(obj+1);
	if(mode!= PI_OUTPUT){
		reportError(DEVELOPER,0,"lib_pi_gpio_eo_write: GPIO pin %d not set as output mode", pin);
		return 0;
	}
	int value = IntVal_value(popStack(stack));
	gpioWrite(pin, value);
	return 0;
}
#endif // RPI
struct ExecStdFuncTable  __ExecStdFuncTable__[] = {
	[EXIT_FUNC] = {lib_exit}, // exit function
	[CHAT_SEND_FUNC] = {lib_chat_send}, // chat send function
	[MATH_SQRT_FUNC] = {lib_math_sqrt}, // math sqrt function
	[T_TIMER_RESET_FUNC] = {lib_timer_reset}, // timer reset function
#ifdef RPI
	[PI_GPIO_SET_OUTPUT_FUNC] = {lib_pi_gpio_set_output}, // Raspberry Pi GPIO set output function
	[PI_GPIO_WRITE_FUNC] = {lib_pi_gpio_write}, // Raspberry Pi GPIO write function
	[PI_GPIO_EO_WRITE_FUNC] = {lib_pi_gpio_eo_write}, // Raspberry Pi GPIO Event Object write function
#endif
};
//-------------------------------
//Event Object 
//-------------------------------
oop wifi_udp_broadcast_eo(oop stack){
	return 0;
}
oop time_eo(oop stack){
/*
0: interval
1: count 
2: label (hold time for comparison)
*/
	GC_PUSH(oop,instance,newInstance(3)); // timer eo has 3 fields: interval, count and label
	oop* fields = getObj(instance, Instance, fields);
	fields[0] = newIntVal(1); // interval
	fields[1] = newIntVal(0); // count
	time_t t = time(NULL);
	fields[2] = newIntVal((int)(t % 10000)); // label
	GC_POP(instance);
	return instance;
}
#ifdef RPI
int isInitGPIO = 0;
oop rpi_gpio_eo(oop stack){
/*
0: pin
1: mode
2: init (pud|vol)
*/
	GC_PUSH(oop,instance,newInstance(3)); // gpio eo has 3 fields: pin, mode, init
	oop* fields = getObj(instance, Instance, fields);
	fields[0] = popStack(stack); // pin
	fields[1] = popStack(stack); // mode
	fields[2] = popStack(stack); // init
	if(!isInitGPIO){
		for(int try_init=0; try_init<5; try_init++){
			if(gpioInitialise() >= 0){
				isInitGPIO = 1;
				break;
			}
			else{
				reportError(DEVELOPER,0,"rpi_gpio_eo: gpioInitialise failed, retrying %d/5", try_init+1);
				sleep(1);
			}
		}
		if(!isInitGPIO){
			reportError(DEVELOPER,0,"rpi_gpio_eo: gpioInitialise failed after 5 attempts");
		}
	}
	if(IntVal_value(fields[1]) == PI_INPUT){
		int pud = IntVal_value(fields[2]);
		gpioSetPullUpDown(IntVal_value(fields[0]), pud);
	}else{
		gpioSetMode(IntVal_value(fields[0]), PI_OUTPUT);
		gpioWrite(IntVal_value(fields[0]), IntVal_value(fields[2]));
	}
	GC_POP(instance);
	return instance;
}
#endif // RPI
//-------------------------------

struct ExecEventObjectTable __ExecEventObjectTable__[] = {
	[CHAT_BROADCAST_EO] = {wifi_udp_broadcast_eo},
	[TIME_EO] = {time_eo},
#ifdef RPI
	[PI_GPIO_EO] = {rpi_gpio_eo},
#endif
};
#endif // SHICAEXEC

// struct EventTable __EventTable__[] = {
// 	[EVENT_EH] = {event_handler,      event_handler_init, 0, NULL, 0},      // EVENT_EH
// 	[TIMER_EH] = {timer_handler,      timer_handler_init, 1,(char []){Integer}, 2},      // TIMER_EH
// 	[T_TIMER_SEC_EH] = {timer_sec_handler,      event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_SEC_EH
// 	[T_TIMER_MIN_EH] = {timer_min_handler,      event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_MIN_EH
// 	[T_TIMER_HOUR_EH] = {timer_hour_handler,    event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_HOUR_EH
// };

//  struct StdFuncTable __StdFuncTable__[] =
// {
// 	{lib_exit, 1, (int[]){Integer}, Undefined}, // exit function takes 1 argument
// 	{lib_chat_send, 2, (int[]){String, Integer}, Undefined}, // chat send function takes 2 arguments
// 	{lib_math_sqrt, 1, (int[]){Integer}, Integer}, // math sqrt function takes 1 argument and returns an integer
// 	{lib_timer_reset, 1, (int[]){Integer}, Undefined}, // timer reset function takes 1 argument
// };

// eo_func_t __EventObjectFuncTable__[] = {
// 	[WEB_RTC_BROADCAST_EO] = wifi_udp_broadcast_eo,
// 	[TIME_EO] = time_eo,
// };
// struct EventObjectTable __EventObjectTable__[] = {
// 	[WEB_RTC_BROADCAST_EO] = {3, 1, (int[]){String, Integer, String}}, // WebRTC broadcast event object with 3 arguments and 1 function
// 	[TIME_EO] = {0, 4, NULL}, // Timer event object with 0 arguments and 4 functions
// };

#endif // STDLIB_C