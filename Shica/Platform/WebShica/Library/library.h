// WEBSHICA
#ifndef SHICA_LIBRARY_H
#define SHICA_LIBRARY_H
#include "../../../Object/object.h"
#include "../../../Node/node.h"
#include "../../../GC/gc.h"
/*===============Other =====================*/
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

#define   WEBTEXT_MAX_SIZE   2048 // 2024 bytes
extern char  WebText[WEBTEXT_MAX_SIZE];
extern int   WebTextPos;

int store(const char* msg); // Store a message in WebText

#define getchar getchar_from_text
int getchar_from_text();

/*
 * This variable is used for storing text data for the web environment.
*/
extern int WEB_TIMER; // Timer for the web environment
extern int WEB_CLICK_STT[3]; // x, y, click status

//AGENT DATA
struct AgentData {
	int index; // offset 0
	int x,y,vx,vy;// offset 4,8,12,16
	int isClick; // offset 20
	int isCollision; // offset 24
	int status;	 // offset 28
	char red, green, blue; // offset 32,33,34
	char isLEDOn; // offset 35
};
typedef struct AgentData *AgentPtr;

/*==============   TIMER  ================= */
int *initWebTimerPtr();
int setWebTimer(int value);
int *getWebTimerPtr();
/*==============   CLICK  ================= */
int *initWebClickSTTPtr();
int *getWebClickSTTPtr();

/*===============  AGENT ===================*/
//Web内のroberのデータ共有で使用
int *initAnAgentDataPtr(); //Initialize the agent data structure and return its address
int *getAnAgentDataPtr(int index); // Get the agent data pointer at the given index

/*===============  AGENTS ==========*/
int **initALLAgentDataPtr(int size); // Initialize all agent data pointers
int **getAllAgentDataPtr(); // Get all agent data pointers
int getAllAgentDataSizePtr(); // Get the size of all agent data pointers

int setActiveAgent(int index); // Set the active agent data by index

/*================ COLLISION =================*/
int collision_calculation(int n);

/*================Event Handler==============*/
enum {
	ENTRY_EH = 0,
	EXIT_EH,
	EVENT_EH,
	TIMER_EH,
	TOUCH_EH,
	COLLISION_EH,
	SELF_STATE_EH,
	CLICK_EH,
	WEB_RTC_BROADCAST_RECEIVED_EH,
	T_TIMER_SEC_EH, // Timer second event handler
	T_TIMER_MIN_EH, // Timer minute event handler
	T_TIMER_HOUR_EH, // Timer hour event handler
	END_EH,
};

extern int _web_rtc_broadcast_receive_(int id, void *ptr, char* message, int sender);//CCALL
extern int _lib_web_rtc_broadcast_send_(int index, char* msg, int num);// JSCALL
extern int _web_rtc_broadcast_eo_(int index, char* channel, char* password, void* ptr);// JSCALL

int event_handler_init(oop eh);
int event_handler(oop eh);

int timer_handler_init(oop eh);
int timer_handler(oop eh);

int touch_handler(oop eh);
int collision_handler(oop eh);
int self_state_handler(oop eh);
int click_handler(oop eh);

/*=============== Event Table ===============*/

extern  struct EventTable __EventTable__[];
int compile_eh_init();
/*===============STANDARD LIBRARY==============*/

extern struct StdFuncTable __StdFuncTable__[];
int compile_func_init();

/*=============== Event Object Table ===============*/
extern  eo_func_t __EventObjectFuncTable__[2];
int compile_eo_init();

/*================END =========================*/
#endif