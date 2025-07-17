# Shica -Web-

## WEB Shica TO DO LIST

## Web shica page directory

```text
|- components
|  |-code: for instruction page componets
|  |-kunii: for inprimentation page of sub-c/im-c
|  |-shikada: for imprimentation page of shica
|
|- app
|  |- kunii: instructino and introduction of your reseach
|  |  |- sub-C: sub-c implemantation page
|  |  |- im-C: im-c implemetation page
|  |
|  |- shikada: instructtion and introduction of my research
|  |  |- web-shica: web-shica implemetation
|  |  |- shica-compiler: compiler of shica that works in C enviroment
|
|- public
|  |- shicada: static page, or plane code (html, js)
|  |- kunii:  static page, or plane code (html, js)
| 
```

### vm.leg (C code)

```text
|-add more web event handler and function
|   |(o) change the speed
|   |(-) change the color               <- in progress
|   |(-) detect touch
|   |(-) detect collision
|   |(-) trigger web timer
|   |(-) change the rober color
|
|-add original shica feature
|   |(o) state transition
|   |(o) entry()event function
|   |(o) exit() event function
|   |(-) add peg/leg struct into union object to collect memory. <-- next step
|   |  |- (o) yyrelease 
|   |  |- (x) mark                     <-
|   |
|   |(-) global variable               <- in progress
|   |(-) type checking
|   |  |> push function into function list.
|   |  |> When the function is called for the first time, copile it by its argument type.
|   |  |> This process determines the argument types and the return type.
|   |  |> Afterwards, the behavior is determined based on the argument types and the return type.
|   |     |(-) String value
|   |     |(-) Float value
|   |(-) event condition
|   |(-) event definition
|   |(-) Pointcut
|   |[-] Add more feature
|         |(-) Event variable
|         |(-) State parameter/argument
|         |(-) Concurrent grammer/feature?
|         |(-) Distributed grammer/feature?
|         |(-) Apply real device (micro controller)
|
|-run multiple agent
|   |(-) sotre one or more codes
|   |(-) store one or more argents
|   |(-) combine web
|
|-add C interface generator
|   |(-) make interface generator that works web
|   |(-)
````

### index.html (WEB code)

```text
|- agent
|   |(-) setRGB(r,g,b): change the robot color <- in progress
|   |(-) setLED(x): change the robot LED status
|   |(-) speak('msg'): output text to the robot
|- UI
|   |(-) make the code selection more readable
```

## MECHA. MEMO

### vms

```text
+-----------------+
| EventCondition A
+-----------------+
| EventAction A
+-----------------+
| EventCondition B
+-----------------+
| EventAction B
+-----------------+

+-----------------+
| Entry() Action
+-----------------+
| State Definition
+-----------------+
| Exit() Action
+-----------------+

+-----------------+
| Function
+-----------------+

+-----------------+
| Function
+-----------------+

```

```text
S [number of Eventhandler]
    E [libray number] [clone number + G-L bool + number of process]
        P [pPos] [cPos]
        P [pPos] [cPos]
    E [libray number] [clone number + G-L bool + number of process]
        P [pPos] [cPos]
        P [pPos] [cPos]
```

### user-define function call

```text
func(1,2)

s00: rbp s00
s01: arg 2
s02: arg 1
s03: rpc x
s04: rbp s00
s05: var 1
s06: var 2
```

### entry / exit event handler

append entry() to first elements of event block.
And also, append exit() to first or second elements of event block.

### MSGC

GC should be change at swtiching compile and executor

### compile

```text
    step 0
        define gc_markFunction_t    gc_markFunction    = markObject;
        define gc_collectFunction_t gc_collectFunction = collectObjects;
        set collectObjects() that includes
            - bytefiles
            - sysmbols
    step 1
        call markObject(webSize)
    step 2
```

### execute

```text
    step 1
        define gc_markFunction_t gc_markFunction       = markIntarray;
        define gc_collectFunction_t gc_collectFunction = collectIntArray;
        set collectIntArray() that includes
            - bytefiles
            - vms
    step 2
        call gc_collect()
```

## web server

go to `http://localhost:8000`

## cacheを消す

なかなか、jsの変更などが反映されないときは、開発者ツールnetwork->Disable casheを選択！
site[https://qiita.com/kondo0602/items/7cc6e0e7783b3533ce2f]

## WASM Compile

```bash
CFLAGS = -O3 -s WASM=1 -s INITIAL_MEMORY=64MB -s EXPORTED_FUNCTIONS='["_memory_init","_compileWebCode","_initRunWeb","_runWeb","_initWebTimerPtr","_initWebClickSTTPtr","_initAnAgnetDataPtr"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32"]' --no-entry -o $(OUT)
```
