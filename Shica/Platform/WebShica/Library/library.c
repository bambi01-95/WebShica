// WEBSHICA
#ifndef SHICA_LIBRARY_C
#define SHICA_LIBRARY_C
#include <stdlib.h>
#include "library.h"

char  WebText[WEBTEXT_MAX_SIZE] = {0}; // Initialize WebText with zeros
int   WebTextPos  = 0;

int store(const char* msg) {
	printf("store called with msg: \n%s\n", msg);

	int len = 0;
	while (msg[len] != '\0' && len < WEBTEXT_MAX_SIZE) {
		WebText[len] = msg[len];
		len++;
	}
	WebText[len] = '\0'; // Null-terminate the string
	if (len >= WEBTEXT_MAX_SIZE) {
		printf("Error: Message too long to store in WebText\n");
		return 0; // Error: message too long
	}
	WebTextPos = 0; 
	printf("end of store function\n");
	return 1;
}

int getchar_from_text()
{
    if ( WebText[WebTextPos] == '\0') {
        return -1; // EOF の代わり
    }
    return WebText[WebTextPos++];
}

/*
 * Variable for the web environment
 * This variable is useded for event handling and soem funcitions.
*/


//TIMER
//Web内のタイマーイベントのデータ共有で使用
int WEB_TIMER = 0;

//CLICK
//Web内のクリックイベントのデータ共有で使用
int WEB_CLICK_STT[3] = {0, 0, 0}; // x, y, click status


struct AgentData *AN_AGENT_DATA = NULL;// Web内のゴーストのデータ共有で使用
struct AgentData anAgentData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Initialize the agent data

int ALL_AGENT_SIZE = 0; // Size of the agent data array
struct AgentData **ALL_AGENT_DATA = NULL; // Web内のゴーストのデータ共有で使用

// Initial agent data: FIXME: set all {0,i*50,0,0,0,0,0,0,0,0}
struct AgentData allAgentData[12] = {
	[0] = { 0, 0,  0, 0,0,0,0,0,0,0,0},
	[1] = { 1, 50,  0, 0,0,0,0,0,0,0,0},
	[2] = { 2, 100,  0, 0,0,0,0,0,0,0,0},
	[3] = { 3, 150,  0, 0,0,0,0,0,0,0,0},
	[4] = { 4, 200,  0,0,0,0,0,0,0,0,0},
	[5] = { 5, 250,  0,0,0,0,0,0,0,0,0},
	[6] = { 6, 300,  0,0,0,0,0,0,0,0,0},
	[7] = { 7, 350,  0,0,0,0,0,0,0,0,0},
	[8] = { 8, 400,  0,0,0,0,0,0,0,0,0},
	[9] = { 9, 450,  0,0,0,0,0,0,0,0,0},
	[10] = { 10, 500,  0,0,0,0,0,0,0,0,0},
	[11] = { 11, 550,  0,0,0,0,0,0,0,0,0}
}; // Initialize the agent data array with 12 agents




/*==============   TIMER  ================= */
int *initWebTimerPtr(){
    if (WEB_TIMER != 0) {
        WEB_TIMER = 0; // Initialize to 0 or some default value
    }
    return &WEB_TIMER;
}
int setWebTimer(int value){
    WEB_TIMER = value;
    return 0;
}
int *getWebTimerPtr(){
    return &WEB_TIMER;
}


/*==============   CLICK  ================= */
int *initWebClickSTTPtr(){
    WEB_CLICK_STT[0] = 0; // x-coordinate
    WEB_CLICK_STT[1] = 0; // y-coordinate
    WEB_CLICK_STT[2] = 0; // click status
    return WEB_CLICK_STT;
}

int *getWebClickSTTPtr(){
    return WEB_CLICK_STT;
}


/*==============   AGENT_INFO  ================= */

static int CurrentAgentIndex = 0; // Current active agent index
int *initAnAgentDataPtr(){
	AN_AGENT_DATA = &anAgentData; // Initialize the agent data pointer
	AN_AGENT_DATA->x = 50; // x-coordinate
	AN_AGENT_DATA->y = 50; // y-coordinate
	AN_AGENT_DATA->vx = 0; // x velocity
	AN_AGENT_DATA->vy = 0; // y velocity
	AN_AGENT_DATA->isClick = 0; // is click
	AN_AGENT_DATA->distance = 0; // distance
	AN_AGENT_DATA->status = 0; // status
	AN_AGENT_DATA->red = 0; // red
	AN_AGENT_DATA->green = 0; // green
	AN_AGENT_DATA->blue = 0; // blue
	AN_AGENT_DATA->isLEDOn = 0; // is LED on
	return (int*)AN_AGENT_DATA; // Return pointer to the agent data
}
//DON'T CHANGE 2025/08/07-2025/08/08
int *getAnAgentDataPtr(int index){
	return (int*)&allAgentData[index]; // Return pointer to the agent data at the given index
}

int setActiveAgent(int index)
{
	if (index < 0 || index >= ALL_AGENT_SIZE) {
		printf("Error: Index out of range\n");
		return -1; // Error: index out of range
	}
	CurrentAgentIndex = index;
	AN_AGENT_DATA = &allAgentData[index]; // Set the active agent data
	return 0; // Success
}

int **initALLAgentDataPtr(int size){
	if(size <= 0 || size > 12) {
		fprintf(stderr, "Error: Invalid size for ALL_AGENT_DATA\n");
		return 0; // Error: invalid size
	}
#ifdef DEBUG
	for(int i = 0; i< size; i++){
		printf("Agent %d: %p\n", i, &allAgentData[i]);
	}
#endif
	ALL_AGENT_SIZE = size;    
    return (int**)allAgentData; // Return pointer to the array of agent data pointers
}

int **getAllAgentDataPtr(){
    if (ALL_AGENT_DATA == NULL) {
        fprintf(stderr, "ALL_AGENT_DATA is not initialized\n");
        return NULL;
    }
    return (int**)ALL_AGENT_DATA; // Return pointer to the array of agent data pointers
}

int getAllAgentDataSizePtr(){
    return ALL_AGENT_SIZE;
}




/*
 * Event handler for the web environment
 */


int event_handler(oop eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		oop thread = eh->EventHandler.threads[0];
		oop stack = newStack(0);
		enqueue3(eh, pushStack(stack, newIntVal(0))); // enqueue a stack with value 0
		return 1;
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(oop eh){
#ifdef DEBUG
	printf("event_handler_init called\n");
#endif
	return 1;
}

int timer_handler(oop eh)
{
	int time = IntVal_value(eh->EventHandler.data[0]);//Integer type
	if (WEB_TIMER-time >=1000) {
		eh->EventHandler.data[1] = newIntVal(IntVal_value(eh->EventHandler.data[1]) + 1);
		eh->EventHandler.data[0] = newIntVal(WEB_TIMER);
		oop stack = newStack(0);
		enqueue3(eh, pushStack(stack, eh->EventHandler.data[1])); // enqueue a stack with value
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int timer_handler_init(oop eh){
	eh->EventHandler.data[0] = newIntVal(WEB_TIMER);
	eh->EventHandler.data[1] = 0;
	return 1;
}

int touch_handler(oop eh)
{
	int touch = 0;
	if (eh->IntQue3.head < eh->IntQue3.tail) {
		oop stack = newStack(0);
		enqueue3(eh, pushStack(stack, newIntVal(touch))); // enqueue a stack with value
		printf("touch event: %d\n", touch);
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}
int collision_handler(oop eh)
{
	int collision = 0;
	if (eh->IntQue3.head < eh->IntQue3.tail) {
		oop stack = newStack(0);
		enqueue3(eh, pushStack(stack, newIntVal(collision))); // enqueue3(eh, pushStack(stack, newIntVal(collision))); // enqueue a stack with value
		printf("collision event: %d\n", collision);
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int self_state_handler(oop eh)
{
	int state = 0;
	if (eh->IntQue3.head < eh->IntQue3.tail) {
		oop stack = newStack(0);
		enqueue3(eh, pushStack(stack, newIntVal(state))); // enqueue a stack with value
		printf("self state event: %d\n", state);
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int click_handler(oop eh)
{
		printf("\t agent %d is clicked [%d]\n", CurrentAgentIndex, WEB_CLICK_STT[2]);
	if (WEB_CLICK_STT[2]==1) {
		oop stack = newStack(0);
		pushStack(stack, newIntVal(WEB_CLICK_STT[0])); // x
		pushStack(stack, newIntVal(WEB_CLICK_STT[1])); // y
		enqueue3(eh, stack); // enqueue the stack
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int _web_rtc_broadcast_receive_(void *ptr, char* message)//CCCALL
{
	oop eh = (oop)ptr;
	oop stack = newStack(0);
	pushStack(stack, newStrVal(message)); // message
	enqueue3(eh, stack); // enqueue the stack
	return 1;
}

int web_rtc_broadcast_receive_handler(oop eh)
{
	// Placeholder for WebRTC broadcast receive event handling
	return 0; // return 0 to indicate no event
}

int web_rtc_broadcast_receive_handler_init(oop eh){
	eh->EventHandler.data[2] = eh;
	return 1;
}


int compile_eh_init(){
	//standard event handler
	printf("compile_eh_init\n");
	node EH = NULL;

	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH); // 1 argument

    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH); // 1 argument

    EH = intern("touchEH");
    EH->Symbol.value = newEventH(TOUCH_EH); // 1 argument

    EH = intern("collisionEH");
    EH->Symbol.value = newEventH(COLLISION_EH); // 2 arguments

    EH = intern("selfStateEH");
    EH->Symbol.value = newEventH(SELF_STATE_EH); // 0 arguments

    EH = intern("clickEH");
    EH->Symbol.value = newEventH(CLICK_EH); // 2 arguments
    return 1; // return 1 to indicate success
}

 struct EventTable __EventTable__[] = {
	[EVENT_EH] = {event_handler,      event_handler_init, 0,NULL, 0},      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init, 1,(char[]) {Integer}, 2},      // TIMER_EH
	[TOUCH_EH] = {touch_handler,      event_handler_init, 1,(char[]){Integer}, 0},      // TOUCH_EH
	[COLLISION_EH] = {collision_handler,  event_handler_init, 2,(char[]){Integer,Integer}, 0},  // COLLISION_EH
	[SELF_STATE_EH] = {self_state_handler, event_handler_init, 0,NULL, 0}, // SELF_STATE_EH
	[CLICK_EH] = {click_handler,      event_handler_init, 2,(char[]) {Integer,Integer}, 0},      // CLICK_EH
	[WEB_RTC_BROADCAST_RECEIVED_EH] = {web_rtc_broadcast_receive_handler, event_handler_init, 2,(char[]){String,String}, 0}, // WEB_RTC_BROADCAST_RECEIVE_EH
};


/* 
 * Standard library functions
 * These functions are used in the web code to interact with the web environment.
*/



int lib_log(oop stack)
{
	int value = IntVal_value(popStack(stack)); // get value from stack
	printf("log: %d\n", value); // print value to console
	return 0; // return 0 to indicate success
}

// This function sets the x and y coordinates of the agent
int lib_setxy(oop stack)
{
	int y = IntVal_value(popStack(stack)); // get x coordinate from stack
	int x = IntVal_value(popStack(stack)); // get y coordinate from stack
	AN_AGENT_DATA->x = x; // set x coordinate
	AN_AGENT_DATA->y = y; // set y coordinate
	printf("setXY: x = %d, y = %d\n", x, y); // print coordinates to console
	return 0; // return 0 to indicate success
}
int lib_setx(oop stack)
{
	int x = IntVal_value(popStack(stack)); // get x coordinate from stack
	AN_AGENT_DATA->x = x; // set x coordinate
	return 0; // return 0 to indicate success
}
int lib_sety(oop stack)
{
	int y = IntVal_value(popStack(stack)); // get y coordinate from stack
	AN_AGENT_DATA->y = y; // set y coordinate
	return 0; // return 0 to indicate success
}

int lib_setvxy(oop stack)
{
	int vy = IntVal_value(popStack(stack)); // get y velocity from stack
	int vx = IntVal_value(popStack(stack)); // get x velocity from stack
	AN_AGENT_DATA->vx = vx; // set x velocity
	AN_AGENT_DATA->vy = vy; // set y velocity
	printf("setVXY: vx = %d, vy = %d\n", vx, vy); // print velocities to console
	return 0; // return 0 to indicate success
}

int lib_setvx(oop stack)
{
	int vx = IntVal_value(popStack(stack)); // get x velocity from stack
	printf("\tsetVX: vx = %d\n", vx); // print x velocity to console
	AN_AGENT_DATA->vx = vx; // set x velocity
	return 0; // return 0 to indicate success
}

int lib_setvy(oop stack)
{
	int vy = IntVal_value(popStack(stack)); // get y velocity from stack
	printf("\tsetVY: vy = %d\n", vy); // print y velocity to console
	AN_AGENT_DATA->vy = vy; // set y velocity
	return 0; // return 0 to indicate success
}

int lib_setcolor(oop stack)
{
	int  red = IntVal_value(popStack(stack)); // get red value from stack
	int green = IntVal_value(popStack(stack)); // get green value from stack
	int blue = IntVal_value(popStack(stack)); // get blue value from stack
	AN_AGENT_DATA->red = (char)red; // set red value
	AN_AGENT_DATA->green = (char)green; // set green value
	AN_AGENT_DATA->blue = (char)blue; // set blue value
	printf("setColor: r = %d, g = %d, b = %d\n", red, green, blue); // print color values to console
	return 0; // return 0 to indicate success
}

// extern int __lib_web_rtc_broadcast_send__(int index, char* channel, char* msg);// JSCALL
int lib_web_rtc_broadcast_send(oop stack)
{
	char* msg = getObj(popStack(stack), StrVal, value); // get message from stack
	char* channel = getObj(popStack(stack), StrVal, value); // get channel from stack
	int index = CurrentAgentIndex;
	_lib_web_rtc_broadcast_send_(index, channel, msg); // call the WebRTC broadcast send function
	if(index < 0){
		reportError(DEVELOPER, 0, "lib_web_rtc_broadcast_send: Invalid agent index %d\n", index);
		return -1; // return -1 to indicate error
	}
	printf("WebRTC Broadcast Send: channel = %s, msg = %s\n", channel, msg); // print message to console
	return 0; // return 0 to indicate success
}

// Function initialization
 enum {
	LOG_FUNC,   // log function
	SETXY_FUNC, // setXY function
	SETX_FUNC,  // setX function
	SETY_FUNC,  // setY function
	SETVXY_FUNC, // setVXY function
	SETVX_FUNC, // setVX function
	SETVY_FUNC, // setVY function
	SETCOLOR_FUNC, // setColor function
	WEB_RTC_BROADCAST_SEND_FUNC,

	NUMBER_OF_FUNCS,/* DO NOT REMOVE THIS LINE */
};

struct StdFuncTable __StdFuncTable__[] =
{
	[LOG_FUNC] = {lib_log, 1, (int[]){Integer}, Undefined}, // log function takes 1 argument
	[SETXY_FUNC] = {lib_setxy, 2, (int[]){Integer, Integer}, Undefined}, // setXY function takes 2 arguments
	[SETX_FUNC] = {lib_setx, 1, (int[]){Integer}, Undefined}, // setX function takes 1 argument
	[SETY_FUNC] = {lib_sety, 1, (int[]){Integer}, Undefined}, // setY function takes 1 argument
	[SETVXY_FUNC] = {lib_setvxy, 2, (int[]){Integer, Integer}, Undefined}, // setVXY function takes 2 arguments
	[SETVX_FUNC] = {lib_setvx, 1, (int[]){Integer}, Undefined}, // setVX function takes 1 argument
	[SETVY_FUNC] = {lib_setvy, 1, (int[]){Integer}, Undefined}, // setVY function takes 1 argument
	[SETCOLOR_FUNC] = {lib_setcolor, 3, (int[]){Integer, Integer, Integer}, Undefined}, // setColor function takes 3 arguments
	[WEB_RTC_BROADCAST_SEND_FUNC] = {lib_web_rtc_broadcast_send, 2, (int[]){Integer, Integer}, Undefined}, // WebRTC broadcast send function takes 2 arguments
};

int compile_func_init()
{
	node FUNC = NULL;
	FUNC = intern("log");
	FUNC->Symbol.value = newStdFunc(LOG_FUNC); // log function

	FUNC = intern("setXY");
	FUNC->Symbol.value = newStdFunc(SETXY_FUNC);

	FUNC = intern("setX");
	FUNC->Symbol.value = newStdFunc(SETX_FUNC); // setX function

	FUNC = intern("setY");
	FUNC->Symbol.value = newStdFunc(SETY_FUNC); // setY function

	FUNC = intern("setVXY");
	FUNC->Symbol.value = newStdFunc(SETVXY_FUNC); // setVXY function

	FUNC = intern("setVX");
	FUNC->Symbol.value = newStdFunc(SETVX_FUNC); 

	FUNC = intern("setVY");
	FUNC->Symbol.value = newStdFunc(SETVY_FUNC);

	FUNC = intern("setColor");
	FUNC->Symbol.value = newStdFunc(SETCOLOR_FUNC); // setColor function
	return 1; // return 1 to indicate success
}

//  _____                 _      ___  _     _           _   
// | ____|_   _____ _ __ | |_   / _ \| |__ (_) ___  ___| |_ 
// |  _| \ \ / / _ \ '_ \| __| | | | | '_ \| |/ _ \/ __| __|
// | |___ \ V /  __/ | | | |_  | |_| | |_) | |  __/ (__| |_ 
// |_____| \_/ \___|_| |_|\__|  \___/|_.__// |\___|\___|\__|
//                                       |__/               

//extern int __web_rtc_broadcast_eo__(char* channel, char* password, void* ptr);// JSCALL
oop web_rtc_broadcast_eo(oop stack)
{
/*
0: channel string
1: password string
2: eh internal pointer
*/
	GC_PUSH(oop,instance,newInstance(3));
	getInstanceField(instance, 0) = popStack(stack); // channel
	getInstanceField(instance, 1) = popStack(stack); // password
	getInstanceField(instance, 2) = NULL; // placeholder for internal pointer (web_rtc_broadcast_receive__ will set this)
	_web_rtc_broadcast_eo_(CurrentAgentIndex,
							   getObj(getInstanceField(instance,0), StrVal, value),
	                           getObj(getInstanceField(instance,1), StrVal, value),
	                           (void*)instance->Instance.fields[2]);// set internal pointer
	GC_POP(instance);
	return instance;
}

oop time_eo(oop stack){
	// Placeholder implementation for timer EO
	printf("timer_eo called\n");
	return 0;
}

enum {
	WEB_RTC_BROADCAST_EO, // WebRTC broadcast event object
	TIME_EO, // Timer event object
	END_EO, /* DO NOT REMOVE THIS LINE */
};

eo_func_t __EventObjectFuncTable__[2] = {
	[WEB_RTC_BROADCAST_EO] = web_rtc_broadcast_eo,
	[TIME_EO] = time_eo,
};

//nArgs, nFuncs, argTypes
struct EventObjectTable __EventObjectTable__[] = {
	[WEB_RTC_BROADCAST_EO] = {2, 2, (int[]){String, String}}, // WebRTC broadcast event object with 2 arguments and 2 functions
	[TIME_EO] = {1, 1, (int[]){Integer}}, // Timer event object with 1 argument and 1 function
};

int compile_eo_init(){
	setEventObjectTable(__EventObjectTable__);
	node sym = NULL;
	node func = NULL;
	node eo = NULL;

	sym = intern("broadcast");
	eo = newEventObject(sym, WEB_RTC_BROADCAST_EO);// var chat = broadcast(channel, password);
	func = newEventH(WEB_RTC_BROADCAST_RECEIVED_EH);sym = newSymbol("received");
	putFuncToEo(eo, func, sym, 0);// chat.received(sender, msg);
	func = newStdFunc(WEB_RTC_BROADCAST_SEND_FUNC);sym = newSymbol("send");
	putFuncToEo(eo, func, sym, 1); // chat.send(msg, recipient);

	return 0; // return 0 to indicate success
}

#endif // SHICA_LIBRARY_C