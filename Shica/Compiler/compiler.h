#ifndef COMPILER_H
#define COMPILER_H

#include "error.h"
#include "opcode.h"
#include "parser.h"
#ifdef WEBSHICA
#include "Platform/WebShica/Library/library.h"
#else // LINUX
#include "Platform/Linux/Library/library.h"
#endif
int compile_event_init();
//int compile_func_init();

//==== STATE GC ====
int initSttTrans();
int collectSttTrans();

void printCode(node code);
node compile();

#endif // COMPILER_H