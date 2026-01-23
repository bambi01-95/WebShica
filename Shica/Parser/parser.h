#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "gc.h"
#include "error.h"

#ifdef WEBSHICA
#include "../Platform/WebShica/Library/library.h" // for WebText, WebTextPos
#else // LINUX
#include "../Platform/Linux/Library/library.h"
#endif 

#define YYTYPE node
extern YYTYPE result;

int yyparse();
void initLine(void);
void initYYContext(); //yyrelease(ctx);

enum {
	PARSER_FINISH = 0,
	PARSER_ERROR = 1,
	PARSER_READY = 2,
};

extern node parserRetFlags[3];

#ifdef MSGC
void collectYYContext();//
#elif defined(MSGCS)
void collectYYContext();//
#else
   #error "GC is not defined, please define MSGC or MSGCS"
#endif


#endif // PARSER_H