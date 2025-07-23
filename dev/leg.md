# LEG/PEG

## yyrelease / yyfree

`yyrelease()` relase / initialize global variale of `yyctx`.
We use it before compiling Shica code.
And also, `yyrelase()` use `yyfree`, that normaly use standard `free()` function.
But Shica use own allocation mechanizm, msgc.
So we use custom option `#define YY_FREE(P,X) X=NULL`.

## reference

- [LEG/PEG](https://www.piumarta.com/software/peg/)
