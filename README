# Shica -Web-

## WEB Shica TO DO LIST

### vm.leg (C code)

|-add more web event handler and function
|   |(o) change the speed
|   |(-) detect touch
|   |(-) change the color               <- in progress
|   |(-) detect collision
|   |(-) trigger web timer
|   |(-) change the rober color
|
|-add original shica feature
|   |(o) state transition
|   |(-) entry(), exit() event function <- in progress
|   |(-) global variable
|   |(-) event condition
|   |(-) event definition
|   |(-) Pointcut
|   |(-) Event variable
|   |(-) Concurrent grammer/feature?
|   |(-) Distributed grammer/feature?
|   |(-) String value
|   |(-) Float value
|   |(-) Apply real device (micro controller)
|
|-run multiple agent
|   |(-) sotre one or more codes
|   |(-) store one or more argents
|   |(-) combine web

### index.html (WEB code)

|- agent
|   |(-) setRGB(r,g,b): change the robot color <- in progress
|   |(-) setLED(x): change the robot LED status
|   |(-) speak('msg'): output text to the robot
|- UI
|   |(-) make the code selection more readable

## MECHA. MEMO

### vms

S [number of Eventhandler]
    E [libray number] [clone number + G-L bool + number of process]
        P [pPos] [cPos]
        P [pPos] [cPos]
    E [libray number] [clone number + G-L bool + number of process]
        P [pPos] [cPos]
        P [pPos] [cPos]

### user-define function call

func(1,2)

s00: rbp s00
s01: arg 2
s02: arg 1
s03: rpc x
s04: rbp s00
s05: var 1
s06: var 2

### MSGC

GC should be change at swtiching compile and executor

### compile

    step 0
        define gc_markFunction_t    gc_markFunction    = markObject;
        define gc_collectFunction_t gc_collectFunction = collectObjects;
        set collectObjects() that includes
            - bytefiles
            - sysmbols
    step 1
        call markObject(webSize)
    step 2

### execute

    step 1
        define gc_markFunction_t gc_markFunction       = markIntarray;
        define gc_collectFunction_t gc_collectFunction = collectIntArray;
        set collectIntArray() that includes
            - bytefiles
            - vms
    step 2
        call gc_collect()

## web server

`http://localhost:8080`

## WASM Compile

CFLAGS = -O3 -s WASM=1 -s INITIAL_MEMORY=64MB -s EXPORTED_FUNCTIONS='["_memory_init","_compileWebCode","_initRunWeb","_runWeb","_initWebTimerPtr","_initWebClickSTTPtr","_initAnAgnetDataPtr"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32"]' --no-entry -o $(OUT)

