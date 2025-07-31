//Linux
#ifndef LIBRARY_H
#define LIBRARY_H
#include "../../../Entity/entity.h"
#include "../../../Object/object.h"

#define EVENT_EH 0x00 // Loop Handler
#define TIMER_EH 0x01 // Timer Handler

int event_handler_init(ent eh);
int event_handler(ent eh);

int timer_handler_init(ent eh);
int timer_handler(ent eh);


extern const struct EventTable LinuxEventTable[];

int compile_eh_init();
#endif // LIBRARY_H