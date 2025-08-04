#ifndef Error_h
#define Error_h

#include <stdio.h>
#include <stdlib.h>
#include "../GC/gc.h"
#define MESSAGE_MAX_LENGTH 256

void fatal(const char *msg, ...);

typedef enum {
    WANNING,
    ERROR,
    FATAL,
    DEVELOPER,
    /* UNSUPPORTED */
    ERROR_UNSUPPORTED
} ErrorType;

typedef struct ErrorList {
    ErrorType type;
    int line;
    char* message;
    struct ErrorList *next; // Pointer to the next error in the list
} ErrorList;

void initErrorList();

void reportError(const int type, const int line, const char *message);

void freeErrorList();

void collectErrorList(void);

#ifdef WEBSHICA
int getNumOfErrorMsg();
char* getErrorMsg(void);
#endif // WEBSHICA

#endif // Error_h
