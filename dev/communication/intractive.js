let layer = [];
let sublayer = [];


mergeInto(LibraryManager.library, {
  create_layer: function () {
    layer = [];
    layer.push(sublayer);
  },
  add_sublayer: function (layerIndex) {
    sublayer = [];
    layer[layerIndex].push(sublayer);
  },
  create_div: function (idPtr, childTextPtr, layerIndex, sublayerIndex) {
    const id = UTF8ToString(idPtr);
    const childText = UTF8ToString(childTextPtr);

    const div = document.createElement('div');
    div.id = id;
    // とりあえずテキストを入れておく
    div.textContent = childText;

    layer[layerIndex][sublayerIndex].appendChild(div);
  },

  create_h1: function (idPtr, textPtr, layerIndex, sublayerIndex) {
    const id = UTF8ToString(idPtr);
    const text = UTF8ToString(textPtr);

    const h1 = document.createElement('h1');
    h1.id = id;
    h1.textContent = text;

    layer[layerIndex][sublayerIndex].appendChild(h1);
  },

  create_h1: function (idPtr, textPtr, layerIndex, sublayerIndex) {
    const id = UTF8ToString(idPtr);
    const text = UTF8ToString(textPtr);

    const h1 = document.createElement('h1');
    h1.id = id;
    h1.textContent = text;

    layer[layerIndex][sublayerIndex].appendChild(h1);
  },

  create_h2: function (idPtr, textPtr, layerIndex, sublayerIndex) {
    const id = UTF8ToString(idPtr);
    const text = UTF8ToString(textPtr);

    const h2 = document.createElement('h2');
    h2.id = id;
    h2.textContent = text;

    layer[layerIndex][sublayerIndex].appendChild(h2);
  }
});