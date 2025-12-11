
const clickSample: string = `// Click Event Sample
stt state(){
  clickEH(int x,int y){
    print("clicked at ", x, ":", y);
  }
}`;

const touchSample: string = `// Touch Event Sample
stt state1(){
  entryEH(){
    setColor(0, 0, 0);// black
  }
  touchEH(int c){stt state2;}
}

stt state2(){
  entryEH(){
    setColor(255, 0, 0);// red
  }
  touchEH(int c){stt state1;}
}`;

const timerSample: string = `// Timer Event Sample
stt state(){
  entryEH(){
    setVX(10);
  }
  timerEH(int sec){
    print(sec, " seconds passed");
    setVX(-getVX());
  }
}`;


const timerEOSample: string = `// Timer Event Sample
var t = time();
stt state(){
  t.secondEH(int sec){
    print("1 second passed", sec);
  }
}`;

const collisionSample: string = `// Collision Event Sample
stt state(){
  entryEH(){
    setVX(5);
  }
  collisionEH(){
    print("collision detected!");
    setVX(-getVX());
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
    timerSample,
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