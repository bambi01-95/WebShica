#ifndef TOOL_H
#define TOOL_H

#include "../Opcode/opcode.h"
#ifdef SHICAEXEC
#include "../Object/object.h"
#else
#include "../Node/node.h"
#endif

typedef const int cint;
typedef const float cfloat;
void printCode(
#ifdef SHICAEXEC
    oop code
#else
    node code
#endif
);

#endif // TOOL_H