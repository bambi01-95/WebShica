
const clickSample: string = `// Click Event Sample
stt state(){
  clickEH(int x,int y){
    print("clicked at ", x, ":", y);
  }
}`;

const touchSample: string = `// Touch Event Sample
stt state(){
  touchEH(int count){
    print("touched with id ", count);
  }
}`;

const collisionSample: string = `// Collision Event Sample
stt state(){
  collisionEH(){
    print("collision detected!");
  }
}`;

const webRtcReceiverSample: string = `// Receiver
var chat = broadcast("shica","pwd");
stt state(){
    chat.received(str from, str msg){
      print("msg from ", from, ": ", msg);
    }
}`;

const WebRtcReceiverSample_v2: string = `// Receiver v2
// Receiver
var chat = broadcast("shica","pwd");
stt state1(){
    chat.received(str from, str msg){
      setColor(0,0,0);
      stt state2;
    }
}

stt state2(){
    chat.received(str from, str msg){
      setColor(255,255,255);
      stt state1;
    }
}`;

const webRtcSenderSample: string = `// Sender
var chat = broadcast("shica","pwd");
  stt state(){
    clickEH(int x,int y){
      chat.send("Hello World",0);
    }
}`;

const stateChangeSample: string = `// State Change Sample
stt state1(){
  entryEH(){
    setVX(5);
    setColor(225, 255, 255);
  }
  clickEH(int x,int y){stt state2;}
}

stt state2(){
  entryEH(){
    setVX(-5);
    setColor(255, 0, 0);
  }
  clickEH(int x,int y){stt state1;}
}`; 

const radioButtonGroupSample: string = `// Radio Button Group Sample
var chat = broadcast("shica","pwd");

stt off(){
  entryEH(){
    setColor(255, 0, 0);
  }
  clickEH(int x,int y){
    chat.send("Button OFF clicked",0);
    stt on;
  }
}
  
stt on{
  entryEH(){
    setColor(0, 255, 0);
  }
  clickEH(int x,int y){
    chat.send("Button ON clicked",0);
    stt off;
  }
  chat.received(str from, str msg){
    print("msg from ", from, ": ", msg);
    stt off;
  }
}`;



const sampleCodes: string[] = [
    touchSample,
    collisionSample,
    radioButtonGroupSample,
    WebRtcReceiverSample_v2,
    webRtcSenderSample,
    webRtcReceiverSample,
    stateChangeSample,
    clickSample,
];


export { sampleCodes };