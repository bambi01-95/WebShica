//Linux
#ifndef LIBRARY_H
#define LIBRARY_H


#include "../../../GC/gc.h"
#ifdef SHICAEXEC
#include "../../../Object/object.h"
#endif
#ifdef SHICACOMP
#include "../../../Node/node.h"
extern struct CompEventTable __CompEventTable__[];
extern struct CompStdFuncTable __CompStdFuncTable__[];
extern struct CompEventObjectTable  __CompEventObjectTable__[];
#endif

/*================Event Handler==============*/
#ifdef SHICAEXEC
//extern  struct EventTable __EventTable__[];
int executor_event_init();
#endif
#ifdef SHICACOMP
int compile_eh_init();
#endif

/*===============STANDARD LIBRARY==============*/
// extern  struct StdFuncTable __StdFuncTable__[];
#ifdef SHICAEXEC
int executor_func_init();
#endif
#ifdef SHICACOMP
int compile_func_init();
#endif

/*=============== Event Object Table ===============*/
#ifdef SHICAEXEC
// extern  eo_func_t __EventObjectFuncTable__[];
#endif
#ifdef SHICACOMP
int compile_eo_init();
#endif


#endif // LIBRARY_H