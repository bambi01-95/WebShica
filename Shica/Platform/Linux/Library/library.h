//Linux
#ifndef LIBRARY_H
#define LIBRARY_H


#include "gc.h"
#ifdef SHICAEXEC
#include "object.h"
extern struct ExecEventTable __ExecEventTable__[];
extern struct ExecStdFuncTable __ExecStdFuncTable__[];
extern struct ExecEventObjectTable  __ExecEventObjectTable__[];
#endif
#ifdef SHICACOMP
#include "node.h"
extern struct CompEventTable __CompEventTable__[];
extern struct CompStdFuncTable __CompStdFuncTable__[];
extern struct CompEventObjectTable  __CompEventObjectTable__[];
#endif

/*================Event Handler==============*/
#ifdef SHICAEXEC

#endif
#ifdef SHICACOMP
int compile_eh_init();
#endif

/*===============STANDARD LIBRARY==============*/
// extern  struct StdFuncTable __StdFuncTable__[];
#ifdef SHICAEXEC

#endif
#ifdef SHICACOMP
int compile_func_init();
#endif

/*=============== Event Object Table ===============*/
#ifdef SHICAEXEC

#endif
#ifdef SHICACOMP
int compile_eo_init();
#endif


#endif // LIBRARY_H