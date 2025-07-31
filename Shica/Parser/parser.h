#ifndef PARSER_H
#define PARSER_H

#include "../Object/object.h"

#define YYTYPE oop
extern YYTYPE result;

int yyparse();
void initLine(void);
void initYYContext(); //yyrelease(ctx);

#ifdef MSGC
void collectYYContext();//
#endif // MSGC


#endif // PARSER_H