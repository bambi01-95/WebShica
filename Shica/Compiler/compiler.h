#ifndef COMPILER_H
#define COMPILER_H

#include "../Error/error.h"
#include "../Opcode/opcode.h"
#include "../Object/object.h"
#include "../Parser/parser.h"
int compile_event_init();
//int compile_func_init();

//==== STATE GC ====
int initSttTrans();
int collectSttTrans();

void printCode(oop code);
oop compile();

#endif // COMPILER_H