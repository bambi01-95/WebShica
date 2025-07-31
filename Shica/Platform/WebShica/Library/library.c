// WEBSHICA
#ifndef SHICA_LIBRARY_C
#define SHICA_LIBRARY_C
#include <stdlib.h>


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
	stop();
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
	stop();
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
	stop();
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
	if (WEB_CLICK_STT[2]==1) {
		int click[2] = {WEB_CLICK_STT[0], WEB_CLICK_STT[1]};
		enqueue3(eh, click); // dequeue the first element
		return 1; // return 1 to indicate success
	}
	return 0; // return 0 to indicate no event
}



int compile_event_init(){
	//standard event handler
	oop EH = NULL;

	EH = intern("eventEH");
	EH->Symbol.value = newEventH(EVENT_EH,EventTables[EVENT_EH].nArgs); // 1 argument

    EH = intern("timerEH");
    EH->Symbol.value = newEventH(TIMER_EH,EventTables[TIMER_EH].nArgs); // 1 argument

    EH = intern("touchEH");
    EH->Symbol.value = newEventH(TOUCH_EH,EventTables[TOUCH_EH].nArgs); // 1 argument

    EH = intern("collisionEH");
    EH->Symbol.value = newEventH(COLLISION_EH,EventTables[COLLISION_EH].nArgs); // 2 arguments

    EH = intern("selfStateEH");
    EH->Symbol.value = newEventH(SELF_STATE_EH,EventTables[SELF_STATE_EH].nArgs); // 0 arguments

    EH = intern("clickEH");
    EH->Symbol.value = newEventH(CLICK_EH,EventTables[CLICK_EH].nArgs); // 2
    return 1; // return 1 to indicate success
}

const struct EventTable WebEventTable[] = {
	[EVENT_EH] = {event_handler,      event_handler_init, 0, 0},      // EVENT_EH
	[TIMER_EH] = {timer_handler,      timer_handler_init, 1, 2},      // TIMER_EH
	[TOUCH_EH] = {touch_handler,      event_handler_init, 1, 0},      // TOUCH_EH
	[COLLISION_EH] = {collision_handler,  event_handler_init, 2, 0},  // COLLISION_EH
	[SELF_STATE_EH] = {self_state_handler, event_handler_init, 0, 0}, // SELF_STATE_EH
	[CLICK_EH] = {click_handler,      event_handler_init, 2, 0},      // CLICK_EH
};

#endif // SHICA_LIBRARY_C