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
struct AgentData allAgentData[12] = {0}; // Initialize the agent data array with 12 agents

int setActiveAgent(int index)
{
	if (index < 0 || index >= ALL_AGENT_SIZE) {
		printf("Error: Index out of range\n");
		return -1; // Error: index out of range
	}
	AN_AGENT_DATA = ALL_AGENT_DATA[index]; // Set the active agent data
	return 0; // Success
}


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

int *getAnAgentDataPtr(){
	return (int*)AN_AGENT_DATA;
}


int **initALLAgentDataPtr(int size){
	if(size <= 0 || size > 12) {
		fprintf(stderr, "Error: Invalid size for ALL_AGENT_DATA\n");
		return 0; // Error: invalid size
	}
    if (ALL_AGENT_DATA == NULL) {
		ALL_AGENT_SIZE = size;
		ALL_AGENT_DATA = (struct AgentData **)allAgentData; // Initialize the ALL_AGENT_DATA pointer
    }
    return (int**)ALL_AGENT_DATA; // Return pointer to the array of agent data pointers
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


int event_handler(ent eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		ent thread = eh->EventHandler.threads[0];
		int val[1] = {0}; // initialize value to 0
		enqueue3(eh, val);
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(ent eh){
#ifdef DEBUG
	printf("event_handler_init called\n");
#endif
	return 1;
}


int timer_handler(ent eh)
{
	int time = eh->EventHandler.data[0];
	if (WEB_TIMER-time >=1000) {
		eh->EventHandler.data[1]++;
		eh->EventHandler.data[0] = WEB_TIMER;
		enqueue3(eh,&eh->EventHandler.data[1]); // dequeue the first element
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}
int timer_handler_init(ent eh){
	eh->EventHandler.data[0] = WEB_TIMER;
	eh->EventHandler.data[1] = 0;
	return 1;
}

int touch_handler(ent eh)
{
	int touch = 0;
	if (eh->IntQue3.head < eh->IntQue3.tail) {
		enqueue3(eh, &touch); // dequeue the first element
		printf("touch event: %d\n", touch);
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}
int collision_handler(ent eh)
{
	int collision = 0;
	if (eh->IntQue3.head < eh->IntQue3.tail) {
		enqueue3(eh, &collision); // dequeue the first element
		printf("collision event: %d\n", collision);
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int self_state_handler(ent eh)
{
	int state = 0;
	if (eh->IntQue3.head < eh->IntQue3.tail) {
		enqueue3(eh, &state); // dequeue the first element
		printf("self state event: %d\n", state);
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int click_handler(ent eh)
{
	printf("triggered click event\n");
	if (WEB_CLICK_STT[2]==1) {
		printf("clicked!!!\n");
		int click[2] = {WEB_CLICK_STT[0], WEB_CLICK_STT[1]};
		enqueue3(eh, click); // dequeue the first element
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}



int compile_eh_init(){
	//standard event handler
	printf("compile_eh_init\n");
	oop EH = NULL;

	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH,EventTable[EVENT_EH].nArgs); // 1 argument

    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH,EventTable[TIMER_EH].nArgs); // 1 argument

    EH = intern("touchEH");
    EH->Symbol.value = newEventH(TOUCH_EH,EventTable[TOUCH_EH].nArgs); // 1 argument

    EH = intern("collisionEH");
    EH->Symbol.value = newEventH(COLLISION_EH,EventTable[COLLISION_EH].nArgs); // 2 arguments

    EH = intern("selfStateEH");
    EH->Symbol.value = newEventH(SELF_STATE_EH,EventTable[SELF_STATE_EH].nArgs); // 0 arguments

    EH = intern("clickEH");
    EH->Symbol.value = newEventH(CLICK_EH,EventTable[CLICK_EH].nArgs); // 2
    return 1; // return 1 to indicate success
}

 struct EventTable __EventTable__[] = {
	[EVENT_EH] = {event_handler,      event_handler_init, 0, 0},      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init, 1, 2},      // TIMER_EH
	[TOUCH_EH] = {touch_handler,      event_handler_init, 1, 0},      // TOUCH_EH
	[COLLISION_EH] = {collision_handler,  event_handler_init, 2, 0},  // COLLISION_EH
	[SELF_STATE_EH] = {self_state_handler, event_handler_init, 0, 0}, // SELF_STATE_EH
	[CLICK_EH] = {click_handler,      event_handler_init, 2, 0},      // CLICK_EH
};


/* 
 * Standard library functions
 * These functions are used in the web code to interact with the web environment.
*/

int lib_log(ent stack)
{
	int value = intArray_pop(stack); // get value from stack
	printf("log: %d\n", value); // print value to console
	return 0; // return 0 to indicate success
}

// This function sets the x and y coordinates of the agent
int lib_setxy(ent stack)
{
	int y = intArray_pop(stack); // get x coordinate from stack
	int x = intArray_pop(stack); // get y coordinate from stack
	AN_AGENT_DATA->x = x; // set x coordinate
	AN_AGENT_DATA->y = y; // set y coordinate
	return 0; // return 0 to indicate success
}

int lib_setvx(ent stack)
{
	int vx = intArray_pop(stack); // get x velocity from stack
	AN_AGENT_DATA->vx = vx; // set x velocity
	AN_AGENT_DATA->vx = vx; // set x velocity
	return 0; // return 0 to indicate success
}

int lib_setvy(ent stack)
{
	int vy = intArray_pop(stack); // get y velocity from stack
	AN_AGENT_DATA->vy = vy; // set y velocity
	AN_AGENT_DATA->vy = vy; // set y velocity
	return 0; // return 0 to indicate success
}

// Function initialization
 enum {
	LOG_FUNC,   // log function
	SETXY_FUNC, // setXY function
	SETVX_FUNC, // setVX function
	SETVY_FUNC, // setVY function
	NUMBER_OF_FUNCS,/* DO NOT REMOVE THIS LINE */
};



struct StdFuncTable __StdFuncTable__[] =
{
	{lib_log, 1}, // log function takes 1 argument
	{lib_setxy, 2}, // setXY function takes 2 arguments
	{lib_setvx, 1}, // setVX function takes 1 argument
	{lib_setvy, 1}, // setVY function takes 1 argument
};

int compile_func_init()
{
	oop FUNC = NULL;
	FUNC = intern("log");
	FUNC->Symbol.value = newStdFunc(LOG_FUNC); // log function

	FUNC = intern("setXY");
	FUNC->Symbol.value = newStdFunc(SETXY_FUNC);

	FUNC = intern("setVX");
	FUNC->Symbol.value = newStdFunc(SETVX_FUNC); 

	FUNC = intern("setVY");
	FUNC->Symbol.value = newStdFunc(SETVY_FUNC);
	return 1; // return 1 to indicate success
}

#endif // SHICA_LIBRARY_C