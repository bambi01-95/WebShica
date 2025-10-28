# Shica -Web-


## MEMO

### CURRENT TASK

`test/webrtc/optbroadcast/shica`から`shikada/shica/`へWebRTC JS funcitonを移行する。
そして、WASMとJSを統合する。

- call a `Module.CCALL(_web_rtc_broadcast_receive_, Number, ['number', 'string'])`: 
- make a `_addWebRtcBroadcast(index, channel, password, ptr)`
- make a `_sendWebRtcBroadcast(index, channel, msg)`


### NEXT 

- type
struct UserType{
    type_t _type;
    oop id;
    oop vars;//stack->Stack.elements[x]->Variable;
};
struct Instance{
    kind_t kind;
    char size;
    oop* fields;
};
- array
struct Array{
    type_t _type;//obj type
    oop typep; //own type
    int size;
};

### NOTICE

- EOの`time.min(int m)`や`time.hour(int h)`は、`timer.sec(int s)`と比較して、毎週期実装する必要がない。
  そのため、`eo->func()`の実装頻度を落としても良い。  

### IF ERROR OCCUR

エラー内容：cc
    timer.reset()の配列が正しく保持できていない可能性
    assert(code->kind == IntArray)でエラー　main.c line 162


## pub/sub communication

* libp2p + GossipSub: P2P pub/sub model. (hosting model is ok... but ...)
    - web: Init webRTC module
    - rpi: UDP (wifi)

### vm.leg (C code)

```text
+- [0] FIXME / TASK / THINK
|   |- (***) runtime error??? -> what is happen... <-- forgettting collect main roots. maybe???
|.  |- (*--) display error message!!!
|.  |.  |> current problem: 
|.  |.  |> shica compiler doesn't detect whole syntax error maybe,
|.  |.  |> it display error with error position (line number), but 1 line diff.
|   |- (**-) learn sepa. heap memory.       
|   |          |> compose heap memory for compiling, and then use free memory for run time.
|.  |- (***) current W-Shica's event handler do monitoring event 2 times, so TO BE only onece!
|.  |- (*--) change Object.* and Entity.* files
|.  |.        |> Object should be Node, cause it is used in compiler.
|.  |.        |> Entity shlud be Object or ...
|.  |- (**-) Shared data (timer, click ) is out of gc_memory range!?
|   |- (*--) sepa. code
|
+- [1] Add more web event handler and function
|   |(o) change the speed
|   |(o) change the color      
|   |(*) change the rober color         
|   |(-) detect touch                <- IN PROGRESS
|   |(-) detect collision            <- IN PROGRESS
|   |(-) trigger web timer           <- IN PROGRESS
|
+- [2] Web UI/UX
|  |- (o) add cimpile and run logging
|  |- (o) Add page outline comp.       
|  |- (o) light/dark mode            
|. |- (o) select and adapt font.  
|. |- (-) coloring code                   
|. |- (-) download .shica code and .stt compiled code.           
|.      | (o) change the text to file, and then to be donwloadble
|.      | (o) make `binary()` function that is used in ccall().
|       | (-) import binary code and shica origanl code
|.      | (-) filename changable                                     <- IN PROGRESS
|
+- [3] Add original shica feature
|   |(o) state transition
|   |(o) entry()event function
|   |(o) exit() event function
|   |(o) add peg/leg struct into union object to collect memory. <-- next step
|   |  |- (o) yyrelease 
|   |  |- (o) mark
|   |
|.  |(o) error message
|   |(*) global variable               
|   |(-) state local variable.             <- IN PROGRES
|.  |(o) String type
|   |(*) type checking.                         [type estimation is not done]
|   |  |> push function into function list.
|   |  |> When the function is called for the first time, copile it by its argument type.
|   |  |> This process determines the argument types and the return type.
|   |  |> Afterwards, the behavior is determined based on the argument types and the return type.
|   |     |(o) String value
|   |     |(-) Float value
|   |(-) event condition
|   |(*) event definition                 <- local is not yet 
|   |(-) Pointcut
|   |[-] Add more feature
|         |(-) Event variable
|         |(-) State parameter/argument
|         |(-) Concurrent grammer/feature....
|         |(-) Distributed grammer/feature...
|         |(-) Apply real device (micro controller)... or web app (dear-project)
|
+- [4] Run multiple agent
|   |(o) sotre one or more codes. 
|   |(o) store one or more argents                
|   |(o) implement all.               
|   |(o) msgc (each heap memory)
|.  |     |(o) msgcs: make test.
|.  |     |(-) copyGC:                                  <-- (----)     
|.  |     |(-) stdatomic.h: pallarel GC
|.  |(-) communicaton                      
|.        |(o) think about witch communication model we use.
|.  |.    |(*) WebRTC: emmulate server and each agent communcate server to all.
|   |     |.    build each server for each 'channel'.
|
+- [5] C web editor
|   |(*) web Native (Linux) compiler                       <- IN PROGRESS
|   |(-) make interface generator that works web

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

## How to install wasm for C lang

[how to write](https://docs.github.com/ja/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)


(A) Emscripten を導入 (C/C++ → WASM コンパイル)
Homebrew がインストール済みか確認

```bash
>brew --version
```

emsdk (Emscripten SDK) を clone
```
>git clone https://github.com/emscripten-core/emsdk.git
>cd emsdk
```
emsdk をインストール
``` text
>./emsdk install latest
>./emsdk activate latest
```

環境変数をセット
```
>source ./emsdk_env.sh
```
動作確認

>emcc -v
emcc は emscripten の C コンパイラで、.wasm を生成できます。


## installがうまくいかないとき
* docker build -t shica-next-app-webshica .
* docker run -p 3000:3000 shica-next-app-webshica