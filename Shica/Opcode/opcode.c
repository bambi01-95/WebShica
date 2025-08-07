#ifndef OPCODE_C
#define OPCODE_C

#include "./opcode.h"

#ifdef DEBUG
const char *getOpcodeName(int opcode) {
    if (opcode < 0 || opcode >= sizeof(_opcode_names) / sizeof(_opcode_names[0])) {
        return "UNKNOWN_OPCODE";
    }
    return _opcode_names[opcode];
}
#define printOP(op) printf("%s\n", getOpcodeName(op))
#else
const char *getOpcodeName(int opcode) {
    return "_PLEASE_COMPILE_WITH_DEBUG_: opcode_names not available";
}
#endif // DEBUG

#endif // OPCODE_C