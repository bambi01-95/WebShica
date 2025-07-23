# Shica -Web-

## WEB Shica TO DO LIST



### vm.leg (C code)

```text
+- [0] FIXME / TASK
|   |- (***) runtime error
|   |- (**-) learn sepa. heap memory
|   |- (*--) sepa. code
|
+- [1] Add more web event handler and function
|   |(o) change the speed
|   |(-) change the color               <- in progress
|   |(-) detect touch
|   |(-) detect collision
|   |(-) trigger web timer
|   |(-) change the rober color
|
+- [2] Web UI/UX
|  |- (o) add cimpile and run logging
|  |- (-) Add page outline comp.       <- in progress
|  |- (-) light/dark mode
|
+- [3] Add original shica feature
|   |(o) state transition
|   |(o) entry()event function
|   |(o) exit() event function
|   |(-) add peg/leg struct into union object to collect memory. <-- next step
|   |  |- (o) yyrelease 
|   |  |- (x) mark                     <- fixme!!!
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
|   |(-) event definition <---
|   |(-) Pointcut
|   |[-] Add more feature
|         |(-) Event variable
|         |(-) State parameter/argument
|         |(-) Concurrent grammer/feature?
|         |(-) Distributed grammer/feature?
|         |(-) Apply real device (micro controller)
|
+- [4] Run multiple agent
|   |(o) sotre one or more codes
|   |(-) store one or more argents
|   |(-) implement all
|   |(-) msgc (each heap memory)
|
+- [5] Add C interface generator
|   |(-) make interface generator that works web
|   |(-) 
```

## MECHA. MEMO

### vms



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

### Other README

## web server

go to `http://localhost:8000`

## cacheを消す

なかなか、jsの変更などが反映されないときは、開発者ツールnetwork->Disable casheを選択！
site[https://qiita.com/kondo0602/items/7cc6e0e7783b3533ce2f]

## WASM Compile

```bash
CFLAGS = -O3 -s WASM=1 -s INITIAL_MEMORY=64MB -s EXPORTED_FUNCTIONS='["_memory_init","_compileWebCode","_initRunWeb","_runWeb","_initWebTimerPtr","_initWebClickSTTPtr","_initAnAgnetDataPtr"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32"]' --no-entry -o $(OUT)
```

## README

[how to write](https://docs.github.com/ja/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)


(A) Emscripten を導入 (C/C++ → WASM コンパイル)
Homebrew がインストール済みか確認


>brew --version
>emsdk (Emscripten SDK) を clone


>git clone https://github.com/emscripten-core/emsdk.git
>cd emsdk
emsdk をインストール


>./emsdk install latest
>./emsdk activate latest
環境変数をセット
>source ./emsdk_env.sh
動作確認

>emcc -v
emcc は emscripten の C コンパイラで、.wasm を生成できます。