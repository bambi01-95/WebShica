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