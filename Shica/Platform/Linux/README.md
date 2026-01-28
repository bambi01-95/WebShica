# Linux function

<!-- -------------------------------------------- -->
## Event Object

<!-- ------------------------- -->
### broadcastEO((int)channel_id, (str)password);

```js:Shica
var channel = broadcastEO("room1", "oops");
state msgManeger(){
    channel.received(int id:id!=channel.self(), str msg){
        channel.send(msg);
    }
}
```
#### (broadcastEO).received(int id, str msg)

<!-- #### (broadcastEO).send(int id, str msg) -->
<!-- send msg to id -->
#### (broadcastEO).send((str) msg)

<!-- #### (broadcastEO).post((str)msg) -->
<!-- send msg to ALL -->
#### (broadcastEO).self()
Get own group id function.
***Why this is function: cause id is dinamicaly change own id.***

<!-- ------------------------- -->
### timerEO((int)interval);

```js:Shica
var t3 = timerEO(3);
state logging(){
    t3.secEH(int sec){
        print(sec);
        if(sec==12)t3.reset();
    }
}
//output: 3 3 3 3 ...
```
#### (timerEO).secEH(int sec){...}

trigger this function when every iterval. time passed.

#### (timerEO).reset();

reset secound to 0;


<!-- -------------------------------------------- -->
## Event Function
<!-- ------------------------- -->
### timerEH(int sec){...}

<!-- -------------------------------------------- -->
## Standard Function
<!-- ------------------------- -->
### print((any)...);


