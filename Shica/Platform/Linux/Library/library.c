// Linux Standard Library
#ifndef LIBRARY_C
#define LIBRARY_C
#include "library.h"

int event_handler(oop eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		oop thread = eh->EventHandler.threads[0];
		oop stack = newStack(0);
		enqueue(eh, pushStack(stack, newIntVal(0))); // enqueue a stack with value 0
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(oop eh){
	return 1;
}
int event_object_handler_init(oop eh){
	return 1;
}

#include <time.h>
int timer_handler(oop eh){
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	if(now - IntVal_value(eh->EventHandler.data[0]) >= 1){
		eh->EventHandler.data[0] = newIntVal(now);
		eh->EventHandler.data[1]++;
		oop stack = newStack(0);
		enqueue(eh, pushStack(stack, eh->EventHandler.data[1])); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0; // return 0 to indicate no event
}
int timer_handler_init(oop eh){//normal timer handler (not EO)
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	eh->EventHandler.data[0] = newIntVal(now); //start time
	eh->EventHandler.data[1] = 0;   //count
	return 1;
}

int compile_eh_init(){
	//standard event handler
	node EH = NULL;
	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH); // 1 argument
    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH); // 1 argument
	return 0; // return 0 to indicate success
}

int timer_sec_handler(oop eh){
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
		enqueue(eh, pushStack(stack, fields[1])); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0;
}
int timer_min_handler(oop eh){
	return 0;
}
int timer_hour_handler(oop eh){
	return 0;
}

struct EventTable __EventTable__[] = {
	[EVENT_EH] = {event_handler,      event_handler_init, 0, NULL, 0},      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init, 1,(char []){Integer}, 2},      // TIMER_EH
	[T_TIMER_SEC_EH] = {timer_sec_handler,      event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_SEC_EH
	[T_TIMER_MIN_EH] = {timer_min_handler,      event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_MIN_EH
	[T_TIMER_HOUR_EH] = {timer_hour_handler,    event_object_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_HOUR_EH
};


/*
 * Standard library functions
 * These functions are used in the web code to interact with the web environment.
*/
 enum {
	EXIT_FUNC=0,   // exit function
	CHAT_SEND_FUNC, // chat send function
	T_TIMER_RESET_FUNC, // timer reset function
};

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

 struct StdFuncTable __StdFuncTable__[] =
{
	{lib_exit, 1, (int[]){Integer}, Undefined}, // exit function takes 1 argument
	{lib_chat_send, 2, (int[]){String, Integer}, Undefined}, // chat send function takes 2 arguments
	{lib_timer_reset, 1, (int[]){Integer}, Undefined}, // timer reset function takes 1 argument
};


int compile_func_init()
{
	node FUNC = NULL;
	FUNC = intern("exit");
	FUNC->Symbol.value = newStdFunc(EXIT_FUNC); // exit function

	return 0; // return 0 to indicate success
}
/*=============== Event Object Table ===============*/
enum {
	WEB_RTC_BROADCAST_EO, // WebRTC broadcast event object
	TIME_EO, // Timer Event Object
	END_EO, /* DO NOT REMOVE THIS LINE */
};

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

eo_func_t __EventObjectFuncTable__[] = {
	[WEB_RTC_BROADCAST_EO] = wifi_udp_broadcast_eo,
	[TIME_EO] = time_eo,
};

struct EventObjectTable __EventObjectTable__[] = {
	[WEB_RTC_BROADCAST_EO] = {3, 1, (int[]){String, Integer, String}}, // WebRTC broadcast event object with 3 arguments and 1 function
	[TIME_EO] = {0, 4, NULL}, // Timer event object with 0 arguments and 4 functions
};

int compile_eo_init(){
	setEventObjectTable(__EventObjectTable__);
	node sym = NULL;
	node func = NULL;
	node eo = NULL;
// Broadcast Event Object
	sym = intern("broadcast");
	eo = newEventObject(sym, WEB_RTC_BROADCAST_EO);// var chat = broadcast(channel, password);
	func = newEventH(CHAT_RECEIVED_EH);sym = newSymbol("received");
	putFuncToEo(eo, func, sym, 0);// chat.received(sender, msg);
	func = newStdFunc(CHAT_SEND_FUNC);sym = newSymbol("send");
	putFuncToEo(eo, func, sym, 1); // chat.send(msg, recipient);
// Timer Event Object
	sym = intern("timer");
	eo = newEventObject(sym, TIME_EO);// var t = timer(interval, label);
	func = newEventH(T_TIMER_SEC_EH);sym = newSymbol("sec");
	putFuncToEo(eo, func, sym, 0);// t.sec(second);
	func = newEventH(T_TIMER_MIN_EH);sym = newSymbol("min");
	putFuncToEo(eo, func, sym, 1);// t.min(minute);
	func = newEventH(T_TIMER_HOUR_EH);sym = newSymbol("hour");
	putFuncToEo(eo, func, sym, 2);// t.hour(hour);
	func = newStdFunc(T_TIMER_RESET_FUNC);sym = newSymbol("reset");
	putFuncToEo(eo, func, sym, 3);// t.x();
// Other Event Objects can be added here

	return 0;
}
#endif // STDLIB_C