#ifndef Error_c
#define Error_c
#include "error.h"

#include <string.h>
#include <assert.h>
#include <stdarg.h>

#ifdef MSGC
#define malloc(size) gc_alloc(size)
#define free(ptr) gc_free(ptr)
#else 
#include <stdlib.h>
#endif

void _fatal(const char *file, int line, const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
#ifdef WEBSHICA
    fprintf(stdin, "\nFatal error [%s:%d]:\n ", file, line);
    vfprintf(stdin, msg, ap);
    fprintf(stdin, "\n");
    fflush(stdin);
#else
	fprintf(stderr, "\nFatal error [%s:%d]: ", file, line);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
    fflush(stderr);
#endif
	va_end(ap);
}

static ErrorList *errorListHeader = NULL; // Global error list header

void initErrorList() {
    errorListHeader = NULL; // Initialize the error list to NULL
}

void reportError(const int type, const int line, const char * fmt, ...) {
    ErrorList* e = malloc(sizeof(ErrorList));
    e->line = line;
    assert(type < ERROR_UNSUPPORTED); // Ensure type is valid
    e->type = type;

    char buffer[MESSAGE_MAX_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);  // フォーマット展開
    va_end(args);

    e->message = gc_strdup(buffer); // フォーマット済み文字列をコピー
    e->next = errorListHeader;
    errorListHeader = e;
    return; 
}
void printErrorList() {
    ErrorList *current = errorListHeader;
    char *typeNames[] = {"DEVELOPER", "INFO", "WARNING", "ERROR", "FATAL", "DEBUG_", "UNSUPPORTED"};
    while (current != NULL) {
        printf("[%s] Line %d: %s\n", typeNames[current->type], current->line, current->message);
        current = current->next; // Move to the next error
    }
}

void freeErrorList() {
    ErrorList *current = errorListHeader;
    ErrorList *next;
    while (current != NULL) {
        next = current->next;
        free(current); // Free the current error node
        current = next;
    }
    errorListHeader = NULL; // Set the original list pointer to NULL
    // to avoid dangling pointers
}

void collectErrorList(void)
{
    ErrorList *current = errorListHeader;
    while (current != NULL) {
        ErrorList *next = current->next;
        gc_markOnly(current); // Mark the error node itself
        gc_markOnly(current->message); // Mark the message string
        current = next; // Move to the next error
    }
    return ;
}



#ifdef WEBSHICA

char webErrorMsg[MESSAGE_MAX_LENGTH];

int getNumOfErrorMsg()
{
    int count = 0;
    ErrorList *current = errorListHeader;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}
/* ERROR FORMAT: 
    [type][line][message]
    type: 0=WARNING, 1=ERROR, 2=FATAL, 3=DEVELOPER, 4=UNSUPPORTED
    line: 4 digits (padded with zeros if necessary)
    message: up to MESSAGE_MAX_LENGTH - 6 characters (to fit in the buffer)
*/
char* getErrorMsg(void)
{
#define MAX_CODE_LINE 4
    assert(errorListHeader != NULL); // Ensure the error list is not empty
    ErrorList *current = errorListHeader;
    webErrorMsg[0] = '\0'; // Initialize the error message buffer
    int index = 0;
    webErrorMsg[index++] = current->type + '0'; // Store the error type
    int line = current->line;
    for(int i = MAX_CODE_LINE; i > 0; i--) {
        webErrorMsg[i] = line % 10 + '0'; // Store the line number as a character
        line /= 10; // Move to the next digit
    }
    index += MAX_CODE_LINE; // Move the index past the line number
    char *msg = current->message;
    int msgLen = strlen(msg);
    for(int i = 0; i < msgLen; i++) {
        webErrorMsg[index++] = msg[i]; // Copy the message into the buffer
    }
    webErrorMsg[index] = '\0'; // Null-terminate the string
    errorListHeader = current->next; // Remove the first error from the list
    printf("getErrorMsg: %s\n", webErrorMsg);
    return webErrorMsg; // Return the message of the first error
#undef MAX_CODE_LINE
}

#endif // WEBSHICA

#ifdef MSGC
#undef malloc
#undef free
#endif

#endif // Error.c
