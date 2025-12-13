// WEBSHICA
#ifndef SHICA_LIBRARY_C
#define SHICA_LIBRARY_C
#include <stdlib.h>
#include "library.h"
#include <stdarg.h>
#include <time.h>

#define STAGE_WIDTH  480
#define STAGE_HEIGHT 480
#define AGENT_SIZE   20
//red print
void console(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	printf("\x1b[31m[C]: ");
	vprintf(msg, ap);
	printf("\x1b[0m");
	va_end(ap);
}
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
	AN_AGENT_DATA->isCollision = 0; // collision
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

int getCurrentAgentIndex(){
	return CurrentAgentIndex;
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
	COLLISION DETECTION FUNCTION
*/


int collision_calculation(int n) {

    // Reset collision status for all agents
    for (int i = 0; i < n; i++) {
        allAgentData[i].isCollision = 0;
    }

    for (int i = 0; i < n; i++) {
        struct AgentData *a = &allAgentData[i];

        // --- Collision with walls ---
        if (a->x < -20 || a->x > STAGE_WIDTH ||
            a->y < -20 || a->y > STAGE_HEIGHT) {
			console("Agent %d collided with wall at position (%d, %d)\n", a->index, a->x, a->y);
            a->isCollision = 1;
        }

        // --- Collision with other agents ---
        for (int j = i + 1; j < n; j++) {
            struct AgentData *b = &allAgentData[j];

            int ax1 = a->x;
            int ay1 = a->y;
            int ax2 = a->x + AGENT_SIZE;
            int ay2 = a->y + AGENT_SIZE;

            int bx1 = b->x;
            int by1 = b->y;
            int bx2 = b->x + AGENT_SIZE;
            int by2 = b->y + AGENT_SIZE;

            // AABB (Axis-Aligned Bounding Box) collision detection
            if (!(ax2 < bx1 || ax1 > bx2 || ay2 < by1 || ay1 > by2)) {
                a->isCollision = 1;
                b->isCollision = 1;
            }
        }
    }
    return 0;
}

/* ---------------------------------------------------
 * Event handler for the web environment
 *---------------------------------------------------*/


int event_handler(oop eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		oop thread = eh->EventHandler.threads[0];
		oop stack = newStack(0);
		enqueue(eh, pushStack(stack, newIntVal(0))); // enqueue a stack with value 0
		return 1;
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(oop eh){
#ifdef DEBUG
	console("event_handler_init called\n");
#endif
	return 1;
}

int timer_handler(oop eh)
{
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	if(now - IntVal_value(eh->EventHandler.data[0]) >= 1){
		console("timer_handler: now=%d, last=%d\n", now, IntVal_value(eh->EventHandler.data[0]));
		eh->EventHandler.data[0] = newIntVal(now);
		int count = IntVal_value(eh->EventHandler.data[1]) + 1;
		eh->EventHandler.data[1] = newIntVal(count);
		oop stack = newStack(0);
		enqueue(eh, pushStack(stack, newIntVal(count))); // enqueue a stack with value
		return 1; // return 1 to indicate event was handled
	}
	return 0; // return 0 to indicate no event
	//when using web js timer
	// int time = IntVal_value(eh->EventHandler.data[0]);//Integer type
	// if (WEB_TIMER-time >=1000) {
	// 	eh->EventHandler.data[1] = newIntVal(IntVal_value(eh->EventHandler.data[1]) + 1);
	// 	eh->EventHandler.data[0] = newIntVal(WEB_TIMER);
	// 	oop stack = newStack(0);
	// 	enqueue(eh, pushStack(stack, eh->EventHandler.data[1])); // enqueue a stack with value
	// 	return 1; // return 1 to indicate success
	// }
	// return 0; // return 0 to indicate no event
}

int timer_handler_init(oop eh){
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	eh->EventHandler.data[0] = newIntVal(now); //start time
	eh->EventHandler.data[1] = newIntVal(0);   //count
	return 1;
	//when using web js timer
	// eh->EventHandler.data[0] = newIntVal(WEB_TIMER);
	// eh->EventHandler.data[1] = 0;
	// return 1;
}

//When touch agent
int touch_handler(oop eh)
{
	if(WEB_CLICK_STT[2]==1){
		console("touch_handler: click status = %d\n", WEB_CLICK_STT[2]);
		struct AgentData *ag = &allAgentData[CurrentAgentIndex];
		if( (ag->x <= WEB_CLICK_STT[0]+AGENT_SIZE) && (ag->x >= WEB_CLICK_STT[0]-AGENT_SIZE) &&
			(ag->y <= WEB_CLICK_STT[1]+AGENT_SIZE) && (ag->y >= WEB_CLICK_STT[1]-AGENT_SIZE) ){
			oop stack = newStack(0);
			assert(getKind(eh->EventHandler.data[0]) == IntVal);
			int count = IntVal_value(eh->EventHandler.data[0]) + 1;
			eh->EventHandler.data[0] = newIntVal(count);
			enqueue(eh, pushStack(stack, newIntVal(count))); // enqueue a stack with value 1
			printf("touch event: agent %d touched at (%d, %d)\n", CurrentAgentIndex, WEB_CLICK_STT[0], WEB_CLICK_STT[1]);
			return 1; // return 1 to indicate success
		}
	}
	return 0; // return 0 to indicate no event
}
int touch_handler_init(oop eh){
	eh->EventHandler.data[0] = newIntVal(0);
	return 1;
}

//When collision detected
int collision_handler(oop eh)
{
	struct AgentData *ag = &allAgentData[CurrentAgentIndex];
	console("collision_handler: isCollision = %d\n", ag->isCollision);
	if(ag->isCollision == 1){
		oop stack = newStack(0);
		console("collision event: agent %d collided\n", CurrentAgentIndex);
		enqueue(eh, stack); // enqueue a stack
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}

int self_state_handler(oop eh)
{
	int state = 0;
	if (eh->Queue.head < eh->Queue.tail) {
		oop stack = newStack(0);
		enqueue(eh, pushStack(stack, newIntVal(state))); // enqueue a stack with value
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
		enqueue(eh, stack); // enqueue the stack
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}
//CCALL id: agnet index, ptr: event handler pointer, message: received message
int _web_rtc_broadcast_receive_(int id, void *ptr, char* message, int sender)//CCCALL
{
	gc_context *copy_ctx = ctx;
	gc_context **ctxs = (gc_context **)ctx->roots;
	ctx = ctxs[id]; // use the first web agent context
	gc_check_ctx(ctx); // check the validity of the context
	oop *ehp = (oop *)ptr;
	oop eh = *ehp;
	if(getKind(eh) != EventHandler){
		ctx = copy_ctx; // restore context
		return 1;
	}// this agent does not have event handler
	console("agnet %d received message from %d: %s\n", id, sender, message);
	oop stack = newStack(0);
	char buf[3];
	sprintf(buf, "%d", sender);// DON'T REMOVE THIS IS NOT PRINT FUNCTION
	pushStack(stack, newStrVal(message)); // message
	pushStack(stack, newStrVal(buf)); // sender
	enqueue(eh, stack); // enqueue the stack
	ctx = copy_ctx; // restore context
	return 1;
}

int web_rtc_broadcast_receive_handler(oop eh)
{
	// Placeholder for WebRTC broadcast receive event handling
	return 0; // return 0 to indicate no event
}

int web_rtc_broadcast_receive_handler_init(oop eh){
	printf("\x1b[31m[C] web_rtc_broadcast_receive_handler_init called\x1b[0m\n");
	oop instance = eh->EventHandler.data[0];
	assert(instance->kind == Instance);
	instance->Instance.fields[0]= eh; // hold event handler pointer
	return 1;
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
	[TOUCH_EH] = {touch_handler,      touch_handler_init, 1,(char[]){Integer}, 1},      // TOUCH_EH
	[COLLISION_EH] = {collision_handler,  event_handler_init, 0,NULL, 0},  // COLLISION_EH
	[SELF_STATE_EH] = {self_state_handler, event_handler_init, 0,NULL, 0}, // SELF_STATE_EH
	[CLICK_EH] = {click_handler,      event_handler_init, 2,(char[]) {Integer,Integer}, 0},      // CLICK_EH
	[WEB_RTC_BROADCAST_RECEIVED_EH] = {web_rtc_broadcast_receive_handler, web_rtc_broadcast_receive_handler_init, 2,(char[]){String,String}, 0}, // WEB_RTC_BROADCAST_RECEIVE_EH
	[T_TIMER_SEC_EH] = {timer_sec_handler,      event_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_SEC_EH
	[T_TIMER_MIN_EH] = {timer_min_handler,      event_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_MIN_EH
	[T_TIMER_HOUR_EH] = {timer_hour_handler,    event_handler_init, 1,(char []){Integer}, 1},      // T_TIMER_HOUR_EH
};


/* 
 * Standard library functions
 * These functions are used in the web code to interact with the web environment.
*/



int lib_log(oop stack)
{
	int value = IntVal_value(popStack(stack)); // get value from stack
	printf("log: %d\n", value); // print value to console
	return 0;
}

/*
	position functions
*/
int lib_setxy(oop stack)
{
	int y = IntVal_value(popStack(stack)); // get x coordinate from stack
	int x = IntVal_value(popStack(stack)); // get y coordinate from stack
	AN_AGENT_DATA->x = x; // set x coordinate
	AN_AGENT_DATA->y = y; // set y coordinate
	printf("setXY: x = %d, y = %d\n", x, y); // print coordinates to console
	return 0;
}
int lib_setx(oop stack)
{
	int x = IntVal_value(popStack(stack)); // get x coordinate from stack
	AN_AGENT_DATA->x = x; // set x coordinate
	return 0;
}
int lib_sety(oop stack)
{
	int y = IntVal_value(popStack(stack)); // get y coordinate from stack
	AN_AGENT_DATA->y = y; // set y coordinate
	return 0;
}

int lib_getx(oop stack)
{
	pushStack(stack, newIntVal(AN_AGENT_DATA->x)); // push x coordinate to stack
	return 0;
}

int lib_gety(oop stack)
{
	pushStack(stack, newIntVal(AN_AGENT_DATA->y)); // push y coordinate to stack
	return 0;
}
/*
	velocity functions
*/

int lib_setvxy(oop stack)
{
	int vy = IntVal_value(popStack(stack)); // get y velocity from stack
	int vx = IntVal_value(popStack(stack)); // get x velocity from stack
	AN_AGENT_DATA->vx = vx; // set x velocity
	AN_AGENT_DATA->vy = vy; // set y velocity
	console("setVXY: vx = %d, vy = %d\n", vx, vy); // print velocities to console
	return 0; // return 0 to indicate success
}

int lib_setvx(oop stack)
{
	int vx = IntVal_value(popStack(stack)); // get x velocity from stack
	console("\tsetVX: vx = %d\n", vx); // print x velocity to console
	AN_AGENT_DATA->vx = vx; // set x velocity
	return 0; // return 0 to indicate success
}

int lib_setvy(oop stack)
{
	int vy = IntVal_value(popStack(stack)); // get y velocity from stack
	console("\tsetVY: vy = %d\n", vy); // print y velocity to console
	AN_AGENT_DATA->vy = vy; // set y velocity
	return 0; // return 0 to indicate success
}

int lib_getvx(oop stack)
{
	pushStack(stack, newIntVal(AN_AGENT_DATA->vx)); // push x velocity to stack
	return 0; // return 0 to indicate success
}

int lib_getvy(oop stack)
{
	pushStack(stack, newIntVal(AN_AGENT_DATA->vy)); // push y velocity to stack
	return 0; // return 0 to indicate success
}

/*
	color functions
*/

int lib_setcolor(oop stack)
{
	int  red = IntVal_value(popStack(stack)); // get red value from stack
	int green = IntVal_value(popStack(stack)); // get green value from stack
	int blue = IntVal_value(popStack(stack)); // get blue value from stack
	AN_AGENT_DATA->red = (char)red; // set red value
	AN_AGENT_DATA->green = (char)green; // set green value
	AN_AGENT_DATA->blue = (char)blue; // set blue value
	console("setColor: r = %d, g = %d, b = %d\n", red, green, blue); // print color values to console
	return 0; // return 0 to indicate success
}

// extern int __lib_web_rtc_broadcast_send__(int index, char* msg, int num);// JSCALL
int lib_web_rtc_broadcast_send(oop stack)
{
	char* msg = getObj(popStack(stack), StrVal, value); // get message from stack
	int num = IntVal_value(popStack(stack)); // get channel from stack
	oop instance = popStack(stack); // get instance from stack
	assert(getKind(instance) == Instance);
	int index = CurrentAgentIndex;
	_lib_web_rtc_broadcast_send_(index, msg, num); // call the WebRTC broadcast send function
	if(index < 0){
		reportError(DEVELOPER, 0, "lib_web_rtc_broadcast_send: Invalid agent index %d\n", index);
		return -1; // return -1 to indicate error
	}
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

// Function initialization
 enum {
	LOG_FUNC,   // log 
	SETXY_FUNC, // set X & Y position
	SETX_FUNC,  // set X position
	SETY_FUNC,  // set Y position
	GETX_FUNC,  // get X position
	GETY_FUNC,  // get Y position
	SETVXY_FUNC, // set velocity of X & Y 
	SETVX_FUNC, // set velocity X 
	SETVY_FUNC, // set velocity Y 
	GETVX_FUNC, // get velocity X 
	GETVY_FUNC, // get velocity Y 
	SETCOLOR_FUNC, // set Color 
	WEB_RTC_BROADCAST_SEND_FUNC,
	T_TIMER_RESET_FUNC,
	NUMBER_OF_FUNCS,/* DO NOT REMOVE THIS LINE */
};
//<-- argTypes
struct StdFuncTable __StdFuncTable__[] =
{
	[LOG_FUNC]   = {lib_log, 1, (int[]){Integer}, Undefined}, // log function takes 1 argument
	[SETXY_FUNC] = {lib_setxy, 2, (int[]){Integer, Integer}, Undefined}, // setXY function takes 2 arguments
	[SETX_FUNC]  = {lib_setx, 1, (int[]){Integer}, Undefined}, // setX function takes 1 argument
	[SETY_FUNC]  = {lib_sety, 1, (int[]){Integer}, Undefined}, // setY function takes 1 argument
	[GETX_FUNC]  = {lib_getx, 0, NULL, Integer}, // getX function returns an integer
	[GETY_FUNC]  = {lib_gety, 0, NULL, Integer}, // getY function returns an integer
	[SETVXY_FUNC] = {lib_setvxy, 2, (int[]){Integer, Integer}, Undefined}, // setVXY function takes 2 arguments
	[SETVX_FUNC] = {lib_setvx, 1, (int[]){Integer}, Undefined}, // setVX function takes 1 argument
	[SETVY_FUNC] = {lib_setvy, 1, (int[]){Integer}, Undefined}, // setVY function takes 1 argument
	[GETVX_FUNC] = {lib_getvx, 0, NULL, Integer}, // getVX function returns an integer
	[GETVY_FUNC] = {lib_getvy, 0, NULL, Integer}, // getVY function returns an integer
	[SETCOLOR_FUNC] = {lib_setcolor, 3, (int[]){Integer, Integer, Integer}, Undefined}, // setColor function takes 3 arguments
	[WEB_RTC_BROADCAST_SEND_FUNC] = {lib_web_rtc_broadcast_send, 2, (int[]){Integer, String}, Undefined}, // WebRTC broadcast send function takes 2 arguments
	[T_TIMER_RESET_FUNC] = {lib_timer_reset, 1, (int[]){Integer}, Undefined}, // timer reset function takes 1 argument
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

	FUNC = intern("getX");
	FUNC->Symbol.value = newStdFunc(GETX_FUNC); // getX function
	FUNC = intern("getY");
	FUNC->Symbol.value = newStdFunc(GETY_FUNC); // getY function

	FUNC = intern("setVXY");
	FUNC->Symbol.value = newStdFunc(SETVXY_FUNC); // setVXY function

	FUNC = intern("setVX");
	FUNC->Symbol.value = newStdFunc(SETVX_FUNC); 
	FUNC = intern("setVY");
	FUNC->Symbol.value = newStdFunc(SETVY_FUNC);

	FUNC = intern("getVX");
	FUNC->Symbol.value = newStdFunc(GETVX_FUNC); 
	FUNC = intern("getVY");
	FUNC->Symbol.value = newStdFunc(GETVY_FUNC);

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
	getInstanceField(instance, 0) = NULL ;// placeholder for internal pointer (web_rtc_broadcast_receive__ will set this)
	getInstanceField(instance, 1) = popStack(stack); // channel
	getInstanceField(instance, 2) = popStack(stack); // password
	_web_rtc_broadcast_eo_(CurrentAgentIndex,
							   getObj(getInstanceField(instance,1), StrVal, value),
	                           getObj(getInstanceField(instance,2), StrVal, value),
	                           (void*)&instance->Instance.fields[0]);// set internal pointer
	GC_POP(instance);
	return instance;
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

enum {
	WEB_RTC_BROADCAST_EO, // WebRTC broadcast event object
	TIME_EO, // Timer event object
	END_EO, /* DO NOT REMOVE THIS LINE */
};

eo_func_t __EventObjectFuncTable__[] = {
	[WEB_RTC_BROADCAST_EO] = web_rtc_broadcast_eo,
	[TIME_EO] = time_eo,
};

//nArgs, nFuncs, argTypes
struct EventObjectTable __EventObjectTable__[] = {
	[WEB_RTC_BROADCAST_EO] = {2, 2, (int[]){String, String}}, // WebRTC broadcast event object with 2 arguments and 2 functions
	[TIME_EO] = {0, 4, NULL}, // Timer event object with 1 argument and 1 function
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
//TIMER EO
	sym = intern("timer");
	eo = newEventObject(sym, TIME_EO);// var t = timer(interval, label);
	func = newEventH(T_TIMER_SEC_EH);sym = newSymbol("sec");
	putFuncToEo(eo, func, sym, 0);// t.sec(second);
	func = newEventH(T_TIMER_MIN_EH);sym = newSymbol("min");
	putFuncToEo(eo, func, sym, 1);// t.min(minute);
	func = newEventH(T_TIMER_HOUR_EH);sym = newSymbol("hour");
	putFuncToEo(eo, func, sym, 2);// t.hour(hour);
	func = newStdFunc(T_TIMER_RESET_FUNC);sym = newSymbol("reset");
	putFuncToEo(eo, func, sym, 3);// t.reset();
	return 0; // return 0 to indicate success
}

#endif // SHICA_LIBRARY_C