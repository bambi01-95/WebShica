
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
const task22Sample: string = `// Particle Sample with improved collision handling
stt moving(){
  int vx = 5;
  int vy = 5;
  entryEH(){
    setVX(vx);
    setVY(vy);
  }
  collisionEH(int xDir, int yDir){
   if(xDir == 1) setVX(-vx); // left collision
   if(xDir == -1) setVX(vx); // right collision
   if(yDir == 1) setVY(vy); // top collision
   if(yDir == -1) setVY(-vy); // bottom collision
  }
  clickEH(int x,int y){
    print("moving stop state");
  }
}
`;

const task31Sample: string = `// copporate sample
var chat = broadcast("shica","pwd");

stt idle(){
  entryEH(){
    setColor(200, 200, 200);
  }
  clickEH(int x,int y){
    str msg = str(x) + "," + str(y);
    chat.send(msg,0);
    stt working;
  }
}

stt counting(){
  int time = 0;
  entryEH(){
    time = now();
   }
    chat.received(str from, str msg:msg=="finished"){
      int elapsed = now() - time;
      print("Task finished in ", elapsed, " ms");
      stt idle;
   }
}`;

const task32Sample: string = `// copporate sample
var chat = broadcast("shica","pwd");
int xTarget = 0;
int yTarget = 0;
stt waiting(){
  chat.received(str from, str msg){
    // parse msg "x,y"
    int x = atoi(substring(msg, 0, indexOf(msg, ",")));
    int y = atoi(substring(msg, indexOf(msg, ",")+1, length(msg)));
    xTarget = x;
    yTarget = y;
    stt moving;
  }
}
stt moving(){
  int vx = 0;
  int vy = 0;
  entryEH(){
    int x = getX();
    int y = getY();
  }
  loopEH(getX() != xTarget || getY() != yTarget){
    chat.send("finished",0);
    stt waiting;
  }
}

`;

const sampleCodes: string[] = [
  task22Sample,
task2Sample,
timerEOSample,
task1Sample,
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