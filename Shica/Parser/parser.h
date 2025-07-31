#ifndef PARSER_H
#define PARSER_H

#include "../Object/object.h"

#define YYTYPE oop
extern YYTYPE result;

int yyparse();
void yyinit(); //yyrelease(ctx);

#ifdef MSGC
void yycollect();//
#endif // MSGC


#endif // PARSER_H