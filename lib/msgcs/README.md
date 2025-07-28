# MSGCS (multiple agent)

The global variable `gc_ctx` is the main memory context that you initialize and that holds the entire memory.
You can separate the memory held by `gc_ctx` using `gc_separateContext(nProcess, nActive)`.
This function divides it into nProcess contexts, and nActive specifies which process you want to activate first.
You can obtain an unused process slot using `gc_getContextSlot()` until you reach nProcess.

Common functions related to GC, such as `gc_alloc()` and `gc_collect()`, work with `ctx`.
`ctx` is a global variable that holds the current context. 
Therefore, every time you switch processes, you need to use the `set_ctx(I)` macro, where `I` is the process index.

> [!WARNING]
> This is __DESIGNED__ for __SINGLE_THREAD__.
>
> If you want to implement multiple threads, change `SET_CTX(I)` and using `ctx`.

## Reference

- `msgc.c`: Ian Piumarta Minimal mark-sweep garbage collector to illustrate the concepts, 2024, [Mozilla Public License v2.0](https://mozilla.org/MPL/2.0/)