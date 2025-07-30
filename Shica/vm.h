#ifndef VM_H
#define VM_H

// FOR EVENT HANDLING
    //TIMER
    //Web内のタイマーイベントのデータ共有で使用
    int WEB_TIMER = 0;
    int *initWebTimerPtr();// Initialize the timer pointer and return its address
    //CLICK
    //Web内のクリックイベントのデータ共有で使用
    int WEB_CLICK_STT[3] = {0, 0, 0}; // x, y, click status
    int *initWebClickSTTPtr();// Initialize the click status pointer and return its address

    //AGENT DATA
    struct AgentData {
        int x;
        int y;
        int vx;
        int vy;
        int isClick;
        int distance;
        int status;
    };
    struct AgentData AN_AGENT_DATA = {0, 0, 0, 0, 0, 0, 0};
    //Web内のゴーストのデータ共有で使用
    int *initAnAgnetDataPtr(); //Initialize the agent data structure and return its address

//FOR ALL: 一度だけ実行
int memory_init(); // Initialize the memory for the compiler and runtime

//FOR COMPILE: コンパイルボタンが押されたら実行
int compileWebCode(const char *code);

//FOR EXECUTION
    //Runボタンが押されたら実行
    int initRunWeb();// Initialize the web runtime environment
    //Runボタンが押されて、initRunWebが実行された後に実行される
    //Stopボタンが押されるまで、何度も実行される。
    int runWeb();

#endif // VM_H