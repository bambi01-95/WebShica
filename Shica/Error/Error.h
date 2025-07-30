#ifndef Error_h
#define Error_h

#include <stdio.h>
#include <stdlib.h>

#define MESSAGE_MAX_LENGTH 256

typedef enum {
    ERROR_WARNING = 0,
    ERROR_ERROR,
    /* UNSUPPORTED */
    ERROR_UNSUPPORTED
} ErrorType;

typedef struct Error {
    ErrorType type;
    int line;
    char message[MESSAGE_MAX_LENGTH];
    struct Error *next; // Pointer to the next error in the list
} Error;


void initErrorList(Error **list);

void reportError(Error **list,const int type,const int line, const char *message);

void freeErrorList(Error **list);

#endif // Error_h
