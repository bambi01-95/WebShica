#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "gc.h"
#include "error.h"
#include "opcode.h"
#include "object.h"
#include "executor.h"

#ifdef WEBSHICA
#include "Platform/WebShica/Library/library.h"
#else // LINUX
#include "Platform/Linux/Library/library.h"
#endif

#define TAGBITS 2
enum {
	TAG_PTR = 0b00, // ordinary pointer (lower 2 bits = 00)
	TAG_INT=0b01,
	TAG_FLT=0b10,
	TAG_FLAG=0b11,
};
typedef enum { ERROR_F, FALSE_F,TRUE_F, NONE_F, HALT_F, EOE_F, CONTINUE_F,TRANSITION_F } retFlag_t;
#define MAKE_FLAG(f) ((oop)(((intptr_t)(f) << TAGBITS) | TAG_FLAG))
extern oop retFlags[8];
#ifdef WEBSHICA
void buildRetFlags();
#endif

oop impleBody(oop exec, oop eh);
oop execute(oop exec, oop entity);

#ifdef WEBSHICA
extern oop WebExecs[12];
#endif		



#endif // EXECUTOR_H