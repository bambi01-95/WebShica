# gossipSub

## support lang.

- rust
- go
- js

## reference



## memo 
```ts
event topic: gossipSub;
event button = button(id1);
event transButton = button(id2)
event input  = input(id3);
comp msg = 
function(title, subtitle){
    <div>
        <h1>title</h1>
        <h2>subtitle</h2>
    <div> 
}

stt msg(){
    gossipSub(topic topic='/xxx/',str msg){
        display(msg)
    }
    button(int click){
        gossipSub(topic,msg);
    }
    input(int i){
        append(msg,e);
    }
    input(int i){

    }
    transButton(int click){
        stt onlyView;
    }
}

stt onlyView(){
    gossipSub(topic topic='/xxx/',str msg){
        display(msg)
    }
}
```

```js
let parents = [];
let sublayer
mergeInto(LibraryManager.library, {

  create_div: function (idPtr, childTextPtr, layer, ) {
    const id = UTF8ToString(idPtr);
    const childText = UTF8ToString(childTextPtr);

    const div = document.createElement('div');
    div.id = id;
    // とりあえずテキストを入れておく
    div.textContent = childText;

    document.body.appendChild(div);
  },

  create_h1: function (idPtr, textPtr) {
    const id = UTF8ToString(idPtr);
    const text = UTF8ToString(textPtr);

    const h1 = document.createElement('h1');
    h1.id = id;
    h1.textContent = text;

    document.body.appendChild(h1);
  },

  create_h2: function (idPtr, textPtr) {
    const id = UTF8ToString(idPtr);
    const text = UTF8ToString(textPtr);

    const h2 = document.createElement('h2');
    h2.id = id;
    h2.textContent = text;

    document.body.appendChild(h2);
  }
});
```

```html
<div>
    <div>
        <div>
            <h1>
                title
            </h1>
            <h2>
                subtite
            </h2>
        </div>
    </div>
    <div>
        <p>description</p>
    </div>
</div>

```