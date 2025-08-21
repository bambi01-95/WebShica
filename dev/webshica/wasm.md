# wasm conecting functions

- `_memory_init`
- `_compileWebCode`
- `_initRunWeb`
- `_runWeb`
- `_initWebTimerPtr`
- `_initWebClickSTTPtr`
- `_initAnAgnetDataPtr`

int memory_init(void);
int initWebCodes(int num);

int addWebCode(void);

int compileWebCode(const int doInit,const int index, const char *code);

int deleteWebCode(const int index)

int initWebAgents(int num);
int executeWebCodes(void);


## wasm function

### for file

Simulation 
- `int getWebCompiledCode(int index)`: get compiled web code at index

Error: `./Shica/Error/*.*`
combination two function, you can out put error msg.
- `int getNumOfErrorMsg(void)`: return number of error.
- `char* getErrorMsg(void)`: return a error msg. 