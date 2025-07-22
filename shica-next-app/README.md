# LAB Page 

## How to build web app

Please following this steps.

- Open your docker app.
- run `docker-compose up --build`
- access `http://localhost:3000`

## Web shica page directory

Please read file structure before using comp and adding file.

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
|
|- public: you can add your static file like html, logo.png etc...
|  |- shicada: static page, or plane code (html, js)
|  |- kunii:  static page, or plane code (html, js)
| 
```



## How to integrate Web assemb in Next.js

### Reference files

If you integrate your wasm file into your page, Please following files (shica base). 

- `hooks/shikada/useVM.tsx`: loading `vm/wasm` and `vm.js`
- `public/shikada/js`: include wasm file these are genarated by wasm
    - `/vm.js`: js code of vm
    - `/vm.wasm`: wasm code of vm
- `app/shikada/shica/page.tsx`: main page using `useVM` and `vm.js`

### How to compile your web assemcode

Please add `ENVIRONMENT=web` option when you compile your C code by wasm.
- hooksを利用してModuleと引っ付ける必要がある。（webassemblyと結合している）
- コンパイル時には、`ENVIRONMENT=web`が必要になる。ないと`ts`なんちゃらとエラーが出力される。

### cacheを消す
なかなか、jsの変更などが反映されないときは、開発者ツールnetwork->Disable casheを選択！
site[https://qiita.com/kondo0602/items/7cc6e0e7783b3533ce2f]