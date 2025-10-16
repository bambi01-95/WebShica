# Opt WebRTC wiht channel -> W-Shica

Shicaへの適応のための手順などをここに記載する。

## 必要なパーツの整理

対応が必要なパーツは、主に下記の3である。
1. 初期化（通信への参加とインスタンス化）
2. 送信
3. 受信

### 1.初期化

```ts: shica
var chat = broadcast("foo", "password"); 
```

```js:
_werRTC_broadcast: function(chP, pwdP, EhPtr){
    const cannel = TOSTR(chP);
    const pwd = TOSTR(pwdP);
    const index = addWebRTC(cannel, pwd, EhPtr);
    return index;//-1: error
}
```

### 2.送信
```ts: shica
chat.send(msg);
```

```js:
_werRTC_send: function(index){
    sendWebRTC(index);
}
```

### 3.受信
```ts: shica
chat.received(str addr,str msg){/* action */}
```

```js
module.ccall(pushWBRTCdata, integer, [ptr, ptr],[eh, agt])
```


## 作成しないといけない関数

```
```

1. AddWebChannel()
2. SendWebChannel()
3. Receved...


```js
interface Message {
  id: string;
  sender: string;
  content: string;
}

interface TopicHost {
  topicName: string;
  hostId: string;
  isActive: boolean;
  connectedUsers: Set<string>;
  messageQueue: Message[];
}

interface UserSession {
  userId: string;
  currentTopic: string;
  isConnected: boolean;
  messages: Message[];
  input: string;
}

const ShicaSimulatorPage = () => {
    const [avaliableTopics] = useState([]);//not NEED?
    const [customTopic, setCustomTopic] = useState('');

}
```


how to impl. EObj in C
```c
struct EObj{
    type_t _type;
    int index;
    oop *funcs; // List of Event and Normal Function.
                // First element should be event function.
};
struct EObjTable{
    char argSize;
    char funcSize;
    int *argTypes;
}
extern struct EObjTable *EObjTable;

// web/lib.h
enum {
    CHAT_EOBJ,
    MAX_SIZE_OF_EOBJ,
};
// web/lib.c
struct EObjTable EObjeTable[] = {
    [CHAT_EOBJ] = {2, 2, (int*){String, String}},
}

oop newEObj(const oop sym, const int index){
    assert(getType(sym)==Symbol);
    assert(index>=0);
    oop new = newObject(EObject);
    new->index = index;
    new->funcPtrs = (oop *)gc_alloc(sizeof(oop) * EObjeTable[index]->funcSize);
    new->funcNames = (char *)gc_alloc(sizeof(char*) * EObjeTable[index]->funcSize);
    sym->Symbol.value = new;
    return new;
}

oop putFuncEObj(oop func,const char* funcName, oop eobj, int index){
    assert(index < EObjeTable[eobj->EObject.index].funcSize);//index should be less than func. size.
    assert(eobj->funcs[index]!=NULL);// that elemenet should be non-initialized.
    eobj->funcPtrs[index] = func;
    eojb->funcNames[index] = strdup(funcName);
    return eboj;
}

int initEObj(void)
{
    oop sym = NULL;
    oop func = NULL;
    oop eObj = NULL;
    
    sym = newIntern('broadcast');
    eObj = newEObj(sym,CHAT_EOBJ);
    func = newEventH(RECEIVED_EH, EventTable[RECEIVED_EH]);//FIXME
    putFuncEObj(func,'received', eObj, 0);// using real number is stupid
    func = newFunc(SEND_FUNC, FuncTable[SEND_FUNC]);//FIXME
    putFunEOBJ(func, 'send', eObj, 1);
    return 0;    
}

 /*
 var chat = broadcast(channel, password);
 broadcast(channel, password).received(int x){
 }
 var sec = timer.sec;
 */

struct Instance{
    type_t _type;
    oop eobj;
    char scope;//global || state
    int index;// >= 0
};
```

```c: \Entity\entity.c
struct e{
    ent eh;
}
```