// WEBSHICA
#ifndef SHICA_LIBRARY_H
#define SHICA_LIBRARY_H
#include "../../Entity/entity.h"
#include "../../Object/object.h"

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
	int x;
	int y;
	int vx;
	int vy;
	int isClick;
	int distance;
	int status;
	char red, green, blue, isLEDOn;
};
typedef struct AgentData *AgentPtr;
extern struct AgentData AN_AGENT_DATA; // Agent data for the web environment

/*==============   TIMER  ================= */
int *initWebTimerPtr();
int setWebTimer(int value);
int *getWebTimerPtr();
/*==============   CLICK  ================= */
int *initWebClickSTTPtr();
int *getWebClickSTTPtr();

/*===============  AGENT ===================*/
//Web内のゴーストのデータ共有で使用
int *initAnAgnetDataPtr(); //Initialize the agent data structure and return its address
int *getAnAgentDataPtr();
AgentPtr initAgent(); // Initialize an agent and return its pointer


/*================Event Handler==============*/
#define ENTRY_EH        0x00 // Entry Handler
#define EXIT_EH         0x00 // Exit Handler
#define EVENT_EH        0x00 // Event Handler
#define	TIMER_EH	    0x01 
#define	TOUCH_EH	    0x02
#define	COLLISION_EH	0x03
#define	SELF_STATE_EH	0x04
#define CLICK_EH		0x05


int event_handler_init(ent eh);
int event_handler(ent eh);

int timer_handler_init(ent eh);
int timer_handler(ent eh);

int touch_handler(ent eh);
int collision_handler(ent eh);
int self_state_handler(ent eh);
int click_handler(ent eh);

extern  struct EventTable __EventTable__[];

int compile_eh_init();
/*===============STANDARD LIBRARY==============*/

extern struct StdFuncTable __StdFuncTable__[];
int compile_func_init();

/*================END =========================*/
#endif