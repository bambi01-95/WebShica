# msgc

## mark leg 

You need collect following two yy struct.

```C
struct _yycontext;
struct _yythunk;
```

main root of yy struct is `yyctx`, that is __yycontext struct.
`__yycontext` holds `_yythunk` as array. 

> [!NOTE]
> `yyctx->__yythunks` is original data of `yythunk`. It means, it is not pointer of thunks obj.

