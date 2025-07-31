#ifndef OPCODE_C
#define OPCODE_C

#include "./opcode.h"

#ifdef DEBUG
const char *getOpcodeName(int opcode) {
    if (opcode < 0 || opcode >= sizeof(opcode_names) / sizeof(opcode_names[0])) {
        return "UNKNOWN_OPCODE";
    }
    return opcode_names[opcode];
}
#define printOP(op) printf("%s\n", getOpcodeName(op))
#else
const char *getOpcodeName(int opcode) {
    return "PLEASE_COMPILE_WITH_DEBUG";
}
#endif // DEBUG

#endif // OPCODE_C