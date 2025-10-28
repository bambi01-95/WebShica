


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




const sampleCodes: string[] = [
    webRtcReceiverSample,
    webRtcSenderSample
];


export { sampleCodes };