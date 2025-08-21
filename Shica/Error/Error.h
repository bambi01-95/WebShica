#ifndef Error_h
#define Error_h

#include <stdio.h>
#include <stdlib.h>
#include "../GC/gc.h"
#define MESSAGE_MAX_LENGTH 100

void _fatal(const char *file, int line, const char *msg, ...);
#define fatal(msg, ...) _fatal(__FILE__, __LINE__, msg, ##__VA_ARGS__)

/*
 * it is related we Error Component.
*/
typedef enum {
    DEVELOPER,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    DEBUG_,/*not. use*/
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

void reportError(const int type, const int line, const char * fmt, ...);

void freeErrorList();

void collectErrorList(void);

#ifdef WEBSHICA
int getNumOfErrorMsg();
char* getErrorMsg(void);
#endif // WEBSHICA

#endif // Error_h
