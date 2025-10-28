# Web Assembler





## Cross funciton

### Call C function from Next.js

- Add that function name into the `WEB_SHICA_FUNC` list in the `Shica/makefile`.
- Add `import { useVM } from "@/hooks/shikada/useShica";` and define `const [Module, isReady] = useVM();`.
- Call your C function like `Module.ccall("FUNCTION_NAME", "retType", ["arg1Type",...],[arg1Name,...])`.

***If something does not work, look the `shica-next-app/hooks/shikada/useShica.tsx`.

### Call Next.js function from C

- decare that function into `C` like, `export int funcion(int arg);`
- define that function into `Platform/WebShica/shica-lib.js`
- when you use/call function that define at next.js (page.ts) from `shica-lib.js`, 
  you should define that funciton as global function.
```js: page.tsx > ShicaPage (comp)
  const JSFUNCITON = ()=>{...};
  // JSCALL
  useEffect(() => {
    if (isReady) {
      (globalThis as any).JSFUNCTION = JSFUNCTION;
    }
  }, [isReady]);
```