//Linux
#ifndef LIBRARY_H
#define LIBRARY_H
#include "../../../Object/object.h"
#include "../../../Node/node.h"
/*===============Other =====================*/


/*================Event Handler==============*/
#define EVENT_EH 0x00 // Loop Handler
#define TIMER_EH 0x01 // Timer Handler
#define CHAT_RECEIVED_EH 0x02 // WebRTC Broadcast Event Object
#define T_TIMER_SEC_EH 0x03 // Timer second event handler
#define T_TIMER_MIN_EH 0x04 // Timer minute event handler
#define T_TIMER_HOUR_EH 0x05 // Timer hour event handler

int event_handler_init(oop eh);
int event_handler(oop eh);

int timer_handler_init(oop eh);
int timer_handler(oop eh);


extern  struct EventTable __EventTable__[];
int compile_eh_init();
int executor_event_init();

/*===============EVENT OBJECT==============*/
extern  eo_func_t __EventObjectFuncTable__[];

/*===============STANDARD LIBRARY==============*/
extern  struct StdFuncTable __StdFuncTable__[];
int compile_func_init();
int executor_func_init();
int compile_eo_init();


#endif // LIBRARY_H