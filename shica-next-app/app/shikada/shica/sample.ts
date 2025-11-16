
const clickSample: string = `
stt state(){
  clickEH(int x,int y){
    print("clicked at ", x, ":", y);
  }
}
`;

const webRtcReceiverSample: string = `
var chat = broadcast("shica","pwd");
  stt state(){
    chat.received(str from, str msg){
      print("msg from ", from, ": ", msg);
    }
}
`;

const webRtcSenderSample: string = `
var chat = broadcast("shica","pwd");
  stt state(){
    clickEH(int x,int y){
      chat.send("Hello World",0);
    }
}
`;

const stateChangeSample: string = `
stt state1(){
  clickEH(int x,int y){
    print("clicked at ", x, ":", y);
    setVX(-5);
    setColor(225, 255, 255);
    stt state2;
  }
}

stt state2(){
  clickEH(int x,int y){
    print("clicked at ", x, ":", y);
    setVX(5);
    setColor(0, 0, 0);
    stt state1;
  }
}

`; 


const sampleCodes: string[] = [
    stateChangeSample,
    clickSample,
    webRtcReceiverSample,
    webRtcSenderSample
];


export { sampleCodes };