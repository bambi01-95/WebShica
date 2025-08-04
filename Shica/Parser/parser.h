#ifndef PARSER_H
#define PARSER_H

#include "../Object/object.h"

#ifdef WEBSHICA
#include "../Platform/WebShica/Library/library.h" // for WebText, WebTextPos
#else // LINUX
#include "../Platform/Linux/Library/library.h"
#endif 

#define YYTYPE oop
extern YYTYPE result;

int yyparse();
void initLine(void);
void initYYContext(); //yyrelease(ctx);

#ifdef MSGC
void collectYYContext();//
#endif // MSGC


#endif // PARSER_H