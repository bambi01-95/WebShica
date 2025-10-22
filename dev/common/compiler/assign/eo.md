# Assigngment Event Object

ここでは、イベントオブジェクトの代入に関してメモをする。


```c: Assginment of Event Object
var chat = broadcast('channel','password');
```

上記のように、イベントオブジェクトの初期化には、複製（インスタンス化）が必要な場合がある。
その際、変数の型定義は、`var`となる。そして、その型は、Event Objectとする。

処理は、普通の代入(`int x = 10`)と同様の処理手順で行われる。まずは、そこをおさらいする。

```opcode
iload x // push intger value x to top of stack
put   i // get value from top of stack and then put it onto i on the stack
```

まず、全体のスタックメモリーに一度値を保持させる。そして、それをGlobal, State or Localの
保持したいメモリーに移すという手順だ。

これを考えると、Event Objectも同じように処理することで実行が可能となる。

```opcode
eload x // initialize x event handler and then put it to top of the stack
put i // same
```



## eo.func

`case GetField`をみろ笑
注意点は、`field->Call.function->GetVar.id`を直接変更している点だ。
将来的に、
```c
chat.send().value();
chat.send().x;
```
のような奇妙な文法がないことを前提としている。