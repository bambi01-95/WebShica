# LAB Page

## How to build web app

Please following this steps.

1. Open your docker app.
2. run `docker-compose up --build`
3. access `http://localhost:3000`

### Web shica page directory

```text
|- components: U can use page, code and ui components. but not costom it!!!
|  |            You can custom or make your comp. in your directory. like comp./shikada...
|  |- page: for instruction / home page componets
|  |- code: for editor page components
|  |- edit: for web user, not developer
|  |- ui: for some page
|  |- kunii: for inprimentation page of sub-c/im-c
|  |- shikada: for imprimentation page of shica
|
|- hooks: you can add your hooks
|  |- shikada
|
|- app
|  |- kunii: instructino and introduction of your reseach
|  |  |- sub-C: sub-c implemantation page
|  |  |- im-C: im-c implemetation page
|  |
|  |- shikada: instructtion and introduction of my research
|  |  |- web-shica: web-shica implemetation
|  |  |- shica-compiler: compiler of shica that works in C enviroment
|. |
|. |- global.css: don't edit exsiting color palette. but you can add>?
|
|- public: you can add your static file like html, logo.png etc...
|  |- shicada: static page, or plane code (html, js)
|  |- kunii:  static page, or plane code (html, js)
|
```

> [!IMPORTANT]
> You can make own custom component and page making your name directory.
> (Don't change shared component directory!!!)

### How to change Theme (dark/light)

Firstly, you have to look `./app/global.css`. We define light and dark color palette in that css file.
It looks like,

```CSS
:root {
  /* define light mode color */
}

html.dark {
  /* define dark mode color */
}
```

In your page.tsx file or your.css, you can adapt theme palette defineing like
`style={{ backgroundColor: "var(--color-background-primary)" }}`. And also
if you want to use hover, it is only defined at `your.css`.

### How to use font

You can use font provided from google.

```tsx
import { Roboto } from "next/font/google";

const roboto = Roboto({
  subsets: ["latin", "latin-ext"],
  weight: ["400", "600", "500", "700"],
});

//...
 <div className={`weight:400 ${roboto.className}`}>
 Hello world!
 </div>
//...
```
>[!TIP]
>You can serach font at [Google Fonts](https://fonts.google.com/)
>`weight` coresppoinding to Google Fonts option. 
---

## How to integrate Web assemb in Next.js

If you integrate your wasm file into your page, Please following files (shica base).

- `hooks/shikada/useVM.tsx`: loading `vm/wasm` and `vm.js`
- `public/shikada/js`: include wasm file these are genarated by wasm
  - `/vm.js`: js code of vm
  - `/vm.wasm`: wasm code of vm
- `app/shikada/shica/page.tsx`: main page using `useVM` and `vm.js`

### How to compile your web assemcode

> [!Caution]
> Please add `ENVIRONMENT=web` option when you compile your C code by wasm.
> hooks を利用して Module と引っ付ける必要がある。（webassembly と結合している）
> コンパイル時には、`ENVIRONMENT=web`が必要になる。ないと`ts`なんちゃらとエラーが出力される。

> [!TIP]
> cache を消す。
> なかなか、js の変更などが反映されないときは、開発者ツール network->Disable cashe を選択！
> site[https://qiita.com/kondo0602/items/7cc6e0e7783b3533ce2f]
