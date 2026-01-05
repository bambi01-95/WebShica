
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
var t = timer();
stt state(){
  t.sec(int sec){
    print("1 second passed", sec);
  }
}`;

const collisionSample: string = `// Collision Event Sample
stt state(){
  entryEH(){
    setVX(5);
  }
  collisionEH(int x,int y){
    print("collision at direction x:", x, " y:", y);
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
    print("state1 entered");
  }
  clickEH(int x,int y){stt state2;}
}

stt state2(){
  entryEH(){
    setVX(-5);
    setColor(255, 0, 0);
    print("state2 entered");
  }
  clickEH(int x,int y){stt state1;}
}`; 

const radioButtonGroupSample: string = `// Radio Button Group Sample
var chat = broadcast("shica","pwd");

stt off(){
  entryEH(){
    setColor(0, 0, 0);
    print("Im OFF");
  }
  touchEH(int c){
    stt on;
  }
}
  
stt on(){
  entryEH(){
    chat.send("Im ON",0);
    setColor(255, 0, 0);
    print("Im ON");
  }
  touchEH(int c){stt off;}
  chat.received(str from, str msg){
    stt off;
  }
}`;

const task1Sample: string = `// Greeting Sample
// 3 states: morning, afternoon, evening
// state changes every 3 seconds
// when clicked, it greets according to the time of day

stt morning(){
  entryEH(){setColor(255, 223, 186); }// light orange
  timerEH(int sec: sec==3){stt afternoon;}
  clickEH(int x, int y){print("Good morning!");}
}

stt afternoon(){
  entryEH(){
    setColor(255, 255, 186); // light yellow
  }
  timerEH(int sec: sec==3){stt evening;}
  clickEH(int x, int y){
    print("Good afternoon!");
  }
}

stt evening(){
  entryEH(){
    setColor(186, 225, 255); // light blue
  }
  timerEH(int sec: sec==3){stt morning;}
  clickEH(int x, int y){
    print("Good evening!");
  }
}`;

// task 2 using multiple collisionEH
const task2Sample: string = `// Particle Sample
stt moving(){
  int vx = 5;
  int vy = 5;
  entryEH(){
    setVX(vx);
    setVY(vy);
  }
  collisionEH(int x, int y){ // left collision
    print(x, y);
    if(x!=0)setVX(-vx);
  }
  collisionEH(int x, int y){ // right collision
    print(1);
    if(x==-1)setVX(-vx);
  }
  collisionEH(int x, int y){ // top collision
    print(2);
    if(y==1)setVY(vy);
  }
  collisionEH(int x, int y){ // bottom collision
    print(3);
    if(y!=0)setVY(-vy);
  }
  clickEH(int x,int y){
    print("moving stop state");
  }
}
`;
// Task 2 using if statement
const task22Sample: string = `// Particle Sample with improved collision handling
// Particle Sample with improved collision handling
stt moving(){
  int vx = 5;
  int vy = 5;
  entryEH(){
    setVX(vx);
    setVY(vy);
  }
  collisionEH(int xDir, int yDir){
   if(xDir == 1) setVX(5); // left collision
   if(xDir == -1) setVX(-5); // right collision
   if(yDir == 1) setVY(5); // top collision
   if(yDir == -1) setVY(-5); // bottom collision
   print(xDir,yDir);
  }
}
`;




const sampleCodes: string[] = [
task2Sample,
task1Sample,
task22Sample,
timerEOSample,
radioButtonGroupSample,
radioButtonGroupSample,
WebRtcReceiverSample_v2,
webRtcSenderSample,
timerSample,
touchSample,
collisionSample,
radioButtonGroupSample,
webRtcReceiverSample,
clickSample,
];


export { sampleCodes };