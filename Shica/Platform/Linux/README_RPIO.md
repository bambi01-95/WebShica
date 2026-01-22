# GPIO LIB DESCRIPTION

## Shica GPIO Docs

### Event Object

#### gpioEO((int)pin,(int)mode, (int)pud);

```js:Shica
var button = gpioEO(17,0,0);//pin, mode, pud | init Write
var led = gpioEO(13, 1, 0);
state monitor(){
    button.readEH(int v){
        print("pressed");
        led.Write(1);
    }
}
```
バックエンドでは、
```c: gpioEO()
int try_i = 0
for(;try_i<3;try_i++)
    if(gpioInitialise(void))break;
if(try_i){
    reportError(ERROR,0000,"msg");
}
if(mode){//input
    gpioSetMode(pin, PI_INPUT);
    gpioSetPullUpDown(pin, pud);
    instance.field[0] = newIntval(pin);
    instance.field[1] = newIntval(mode);
}else{
    gpioSetMode(pin, PI_OUTPUT);
    gpioWrite(pin, mode);
    instance.field[0] = newIntval(pin);
    instance.field[1] = newIntval(mode);
}

```
```c:button.readEH(int v){...}
const int pin = IntVal_value(instance.field[0]);
if(IntVal_value(instance.field[1])!=0)return 1;//error message
const int val = gpioRead(pin);
if(val){
    enqueue();
}
```
```c:led.write((int)v);
const int pin = IntVal_value(instance.field[0]);
if(IntVal_value(instance.field[1])!=1)return 1;//error message
const int val = intVal_value(popStack);
pigpioWrite(pin,v);
```

Cの対応する関数詳細
```c
int gpioInitialise(void);
gpioSetMode(gpio, PI_INPUT);
int gpioRead(unsigned gpio);
```


### Event Function

#### gpioRead31EH(int pin, int vol){...}

```js:Shica
gpioRead31EH(int vol){
    for(int i=0;i<32;i++){
        print(vol||(1<<i));
    }
}
```
バックエンドでは、`gpioRead_Bits_0_31();`で呼び出し、イベントをチェックする。

Cの対応する関数詳細
```c
int gpio_status = gpioRead_Bits_0_31();
```


### Common Function

#### gpioWrite((int)pin,(int)vol);

```js:Shica
gpioWrite(1);//write voltage 1
```
バックエンドでは、`gpioWrite(pin,vol);`で呼び出す。

Cの対応する関数詳細
```c
int gpioWrite(unsigned gpio, unsigned level);
```

#### gpioSetMode((int)pin);

```js:Shica
gpioSetOutPut(Pin);
```

バックエンドでは、`gpioSetMode(pin,PI_OUTPUT);`で呼び出す。

Cの対応する関数詳細
```c
int gpioSetMode(unsigned gpio, unsigned mode);
// mode	意味
// PI_INPUT	入力
// PI_OUTPUT	出力
```


```var:RBG
var button = gpioEO(17,1);
state init(){
    entryEH(){
        gpioSetOutPut(13);
    }
}
state off(){
    entryEH(){
        gpioWrite(13,0);
    }
    button.readEH(int val){
        state on;
    }
}
state on(){
    button.readEH(int val){
        state off;
    }
}
```



## C native function
```c
int gpioInitialise(void);
<!-- pigpio を初期化（成功 ≥0、失敗 <0） -->
void gpioTerminate(void);

gpioSetMode(17, PI_INPUT);
int gpioSetMode(unsigned gpio, unsigned mode);
// mode	意味
// PI_INPUT	入力
// PI_OUTPUT	出力
gpioSetPullUpDown(17, PI_PUD_UP);
// pud	意味
// PI_PUD_OFF	無効
// PI_PUD_DOWN	プルダウン
// PI_PUD_UP	プルアップ


int gpioRead(unsigned gpio);
int gpioWrite(unsigned gpio, unsigned level);

// Event Object
int gpioInitialise(void);は、bool値で初期化したのかを確認
```