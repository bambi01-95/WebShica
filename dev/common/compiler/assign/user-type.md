# assignment User-Type


## Decriare use-defined type
```c
type Pos = {int x, int y};
```

## Assignment 

```c
Pos p1 = {0,0};
p1.x = 10;
```

```c:parse
//p1 = {0,0};
iload 0
iload 0
????
//p1.x
iload 10
getField p1
getValue  x
```

```opcode
iload 10
local_get 0
sget index
```

```c
void member_put(int field_index) {
    Value val = stack_pop();     // 書き込む値 (10)
    Object* obj = stack_pop();   // 対象オブジェクト p
    obj->fields[field_index] = val;
}
```