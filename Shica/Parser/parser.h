#ifndef PARSER_H
#define PARSER_H

#ifndef OBJECT_C
union Object;
typedef union Object *oop;
#endif
//#include "./Object.h"

#define YYTYPE oop
extern YYTYPE result;

int yyparse();
void yyinit(); //yyrelease(ctx);

#ifdef MSGC
void yycollect();//
#endif // MSGC


#endif // PARSER_H