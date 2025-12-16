#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "../GC/gc.h"
#include "../Error/error.h"
#include "../Opcode/opcode.h"
#include "../Object/object.h"
#include "../Executor/executor.h"

#ifdef WEBSHICA
#include "../Platform/WebShica/Library/library.h"
#else // LINUX
#include "../Platform/Linux/Library/library.h"
#endif

int executor_event_init();
int executor_func_init();

#define TAGBITS 2
enum {
	TAG_PTR = 0b00, // ordinary pointer (lower 2 bits = 00)
	TAG_INT=0b01,
	TAG_FLT=0b10,
	TAG_FLAG=0b11,
};
typedef enum { ERROR_F,NONE_F, HALT_F, EOE_F, EOC_F, CONTINUE_F,TRANSITION_F } retFlag_t;
#define MAKE_FLAG(f) ((oop)(((intptr_t)(f) << TAGBITS) | TAG_FLAG))
extern oop retFlags[7];
#ifdef WEBSHICA
void buildRetFlags();
#endif

oop impleBody(oop code, oop eh, oop agent);
oop execute(oop prog,oop entity, oop agent);
#endif // EXECUTOR_H