// WEBSHICA
#ifndef SHICA_LIBRARY_H
#define SHICA_LIBRARY_H
#include "../../../Entity/entity.h"
#include "../../../Node/node.h"

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
	int x,y,vx,vy;
	int isClick;
	int distance;
	int status;
	char red, green, blue;
	char isLEDOn;
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