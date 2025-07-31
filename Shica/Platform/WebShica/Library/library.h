// WEBSHICA
#ifndef SHICA_LIBRARY_H
#define SHICA_LIBRARY_H
#include "../../Entity/entity.h"
#include "../../Object/object.h"

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

extern const struct EventTable WebEventTable[];

void compile_eh_init();
#endif