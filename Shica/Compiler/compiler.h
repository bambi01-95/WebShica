#ifndef COMPILER_H
#define COMPILER_H

#include "../Error/error.h"
#include "../Opcode/opcode.h"
#include "../Object/object.h"
#include "../Parser/parser.h"
#ifdef WEBSHICA
#include "../Platform/WebShica/Library/library.h"
#else // LINUX
#include "../Platform/Linux/Library/library.h"
#endif
int compile_event_init();
//int compile_func_init();

//==== STATE GC ====
int initSttTrans();
int collectSttTrans();

void printCode(node code);
node compile();

#endif // COMPILER_H