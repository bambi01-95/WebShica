//Linux
#ifndef LIBRARY_H
#define LIBRARY_H
#include "../../../Entity/entity.h"
#include "../../../Object/object.h"
/*===============Other =====================*/


/*================Event Handler==============*/
#define EVENT_EH 0x00 // Loop Handler
#define TIMER_EH 0x01 // Timer Handler

int event_handler_init(ent eh);
int event_handler(ent eh);

int timer_handler_init(ent eh);
int timer_handler(ent eh);


extern  struct EventTable __EventTable__[];
int compile_eh_init();
int executor_event_init();
/*===============STANDARD LIBRARY==============*/
extern  struct StdFuncTable __StdFuncTable__[];
int compile_func_init();
int executor_func_init();


#endif // LIBRARY_H