#ifndef Error_h
#define Error_h

#include <stdio.h>
#include <stdlib.h>
#include "../GC/gc.h"
#define MESSAGE_MAX_LENGTH 100
#define DEVELOPER_EMAIL "hiroto.shikada@gmail.com"

void _fatal(const char *file, int line, const char *msg, ...);
#define fatal(msg, ...) _fatal(__FILE__, __LINE__, msg, ##__VA_ARGS__)

void _stop(const char *file, int line);
#define stop() _stop(__FILE__, __LINE__)
/*
 * it is related we Error Component.
*/
typedef enum {
    DEVELOPER,
    INFO,
    LOG,
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

void _reportError(const int type, const int line, const char * fmt, ...);

#ifdef NDEBUG
#define reportError(TYPE, LINE, FMT, ...) _reportError(TYPE, LINE, FMT, ##__VA_ARGS__)
#else
#define reportError(TYPE, LINE, FMT, ...) _reportError(TYPE, LINE, FMT, ##__VA_ARGS__); \
    fprintf(stderr, "[Debug] Reported error at %s:%d\n", __FILE__, __LINE__);
#endif


void printErrorList();
void freeErrorList();

void collectErrorList(void);

#ifdef WEBSHICA
int getNumOfErrorMsg();
char* getErrorMsg(void);
#endif // WEBSHICA

#endif // Error_h
