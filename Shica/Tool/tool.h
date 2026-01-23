#ifndef TOOL_H
#define TOOL_H

#include "opcode.h"
#ifdef SHICAEXEC
#include "object.h"
#else
#include "node.h"
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