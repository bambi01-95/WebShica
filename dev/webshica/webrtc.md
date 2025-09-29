# WebRTC event handler

```shica: sample.shica
event received = webrtc();
stt normal(){
    received(str addr, str msg){
        
    }
}

stt withFilter(){
    recevied(str addr,str msg:list data=jsonInclude(msg,'x','y')){
        move(addr, data.x, data.y);
    }
}
```
## JS - C
```ts
ondatachannel = (event) => {
    push(event.data);
};
```

```c
typedef char* json_t;

struct WebMsgQueue{
    char* id;
    json_t msg;
}

int enqueueWebMsg(int id, char* msg)
{
    oop new = newObject(WebMsgQueue);
    new->WebMsgQueue.id = id
    new->WebMsgQueue.msg = (json_t)msg;
    return new;
}

int searchTopicsInJson(json_t* topic, json_t json)
{
    //search topic from json model   
}
```

## TODO

- str msg = 'hello world';
- list topics = jsonToStrList(msg);
- int x = itos(topics.x);
- udp: communication

## process

```c
typedef char ret_t;
ret_t pushMessage(void* ptr)
{
    pushMSG(ptr);
}
exprort ret_t addBroadcaseMemeber(char *queue);

ret_t intiGetMessage_lib(ent agent, ent eh)
{
    ent data = newQueue(msg);
    push(msg)= return;
    
}
void getMSG_lib(ent agent,ent eh){
    const queue = getWebQueue(eh->EH.value[0]);
    if(queue==NULL)return;
    char* list = json(queue);
    if(eval(list,eh))return;
    enqueue(newList(list));
    return;
}
```