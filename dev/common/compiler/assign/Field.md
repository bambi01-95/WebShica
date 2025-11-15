# Array and User-defined type 

## Expression list

```c
//ASSGIN
a = 0;
a[0] = 0;
a[0][0] = 0;

a.x = 0;
a.x.y = 0;

a.x[10] = 0;
a.x[10].y = 0;

//GET
b = a;
b = a[0];
b = a[0][0];

b = a.x;
b = a.x.y;

b = a.x[10];
b = a.x[10].y;
```

## GET

1. `b = a`

```c
GETVAR 0
SETVAR 1 //b
```

2. `b = a[10]`

```c
GETVAR 0
    LOAD 10
    GETARRAY
SETVAR 0
```

3. `b = a[10][10]`

```c
GETVAR 0
    LOAD 10
    GETARRAY
        LOAD 10
        GETARRAY
SETVAR 0
```

4. `b = a.x`

```c
GETVAR 0
    LOAD 0
    GETARRAY
SETVAR 0
```

5. `b = a.x.y`

```c
GETVAR 0
    LOAD 0
    GETARRAY
        LOAD 1
        GETARRAY
SETVAR 0
```

6. `b = a.x[10]`

```c
GETVAR 0
    LOAD 0
    GETARRAY
        LOAD 10
        GETARRAY
SETVAR 0
```

7. `b = a.x[10].y`

```c
GETVAR 0
    LOAD 0
    GETARRAY
        LOAD 10
        GETARRAY
            LOAD 1
            GETARRAY
SETVAR 0
```


## ASSIGN

1. `a = 0`

```c
LOAD 0
SETVAR 0
```

2. `a[0] = 0;`

```c
LOAD 0
GETVAR 0
    LOAD 0
    SETARRAY
```

3. `a[0][0] = 0;`

```c
LOAD 0
GETVAR 0
    LOAD 0
    GETARRAY
        LOAD 0
        SETARRAY
```

4. `a.x = 0;`

```c
LOAD 0
GETVAR 0
    LOAD 0
    SETARRAY
```

5. `a.x.y = 0;`

```c
LOAD 0
GETVAR 0
    LOAD 0 //first ele
    GETARRAY 
        LOAD 1 //second ele
        SETARRAY
```

***`y` is second element***

6. `a.x[10].y = 0;`

```c
LOAD 0
GETVAR 0
    LOAD 0 //.x
    GETARRAY 
        LOAD 10 //[10]
        SETARRAY
```

7. `a.x[10].y = 0;`

```c
LOAD 0
GETVAR 0
    LOAD 0 //.x
    GETARRAY 
        LOAD 10 //[10]
        GETARRAY
            LOAD 1 //.y
            SETARRAY
```