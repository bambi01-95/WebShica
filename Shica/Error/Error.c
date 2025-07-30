#ifndef Error_c
#define Error_c
#include "Error.h"

#include <string.h>
#include <assert.h>

#ifdef MSGC
#include "./gc/msgc/msgc.h"
#define malloc(size) gc_alloc(size)
#define free(ptr) gc_free(ptr)
#else 
#include <stdlib.h>
#endif


void initErrorList(Error **list) {
    *list = NULL; // Initialize the error list to NULL
}

void reportError(Error **list,const int type, const int line, const char *message) {
    Error* e = malloc(sizeof(Error));
    e->line = line;
    assert(type < ERROR_UNSUPPORTED); // Ensure type is valid
    e->type = type;
    assert(strlen(message) < MESSAGE_MAX_LENGTH); // Ensure message length is within bounds
    strncpy(e->message, message, MESSAGE_MAX_LENGTH - 1);
    e->message[MESSAGE_MAX_LENGTH - 1] = '\0'; // Ensure null termination
    e->next = *list;
    *list = e;
}

void freeErrorList(Error **list) {
    Error *current = *list;
    Error *next;
    while (current != NULL) {
        next = current->next;
        free(current); // ガベージコレクタを使わない場合、ここで解放が必要
        current = next;
    }
    *list = NULL; // Set the original list pointer to NULL
    // to avoid dangling pointers
}

#ifdef MSGC
#undef malloc
#undef free
#endif

#endif // Error.c
