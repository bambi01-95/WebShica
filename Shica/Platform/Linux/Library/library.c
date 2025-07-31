// Linux Standard Library
#ifndef LIBRARY_C
#define LIBRARY_C
#include "library.h"

int event_handler(ent eh){
	if(eh->EventHandler.threads[0]->Thread.inProgress == 0) {
		ent thread = eh->EventHandler.threads[0];
		int val[1] = {0}; // initialize value to 0
		enqueue3(eh, val);
	}
	return 0; // return 0 to indicate no event
}
int event_handler_init(ent eh){
	return 1;
}

#include <time.h>
int timer_handler(ent eh){
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	if(now - eh->EventHandler.data[0] >= 1){
		eh->EventHandler.data[0] = now;
		eh->EventHandler.data[1]++;
		enqueue3(eh, &eh->EventHandler.data[1]);
		return 1;
	}
	return 0; // return 0 to indicate no event
}

int timer_handler_init(ent eh){
	time_t t = time(NULL);
	int now = (int)(t % 10000);
	eh->EventHandler.data[0] = now; //start time
	eh->EventHandler.data[1] = 0;   //count
	return 1;
}

int compile_eh_init(){
	//standard event handler
	oop EH = NULL;
	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH,EventTables[EVENT_EH].nArgs); // 1 argument
    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH,EventTables[TIMER_EH].nArgs); // 1 argument
	return 0; // return 0 to indicate success
}

const struct EventTable LinuxEventTable[] = {
	[EVENT_EH] = {event_handler,      event_handler_init, 0, 0},      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init, 1, 2},      // TIMER_EH
};


#endif // STDLIB_C