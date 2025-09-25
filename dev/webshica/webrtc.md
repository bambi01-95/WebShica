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

A B C D

A
B
C
D

