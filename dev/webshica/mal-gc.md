# Multiple GC

Multiple garbage collection system for managing compilation and execution contexts.

## Variables

```c
MAX_BINARY_CODE    // Maximum number of binary codes
binaryCodes[]      // Array of compiled binary codes
nBinaryCodes       // Current number of binary codes

MAX_AGENT_CODE     // Maximum number of execution agents
agents[]           // Array of execution agents
nAgents[]          // Number of agents per context
```

## Flow

```
com_ctx = gen(ctx) -> malloc()  // Compilation context
exe_ctx = global_ctx            // Execution context
```

### Compiler

#### Initialization

- `initBinaryCode(number)` → `MAX_BINARY_CODE = number`

#### Compilation

- `compileWebCodes(bool, index, code)`
  - If `bool` is true: clean whole memory
  - If `index < MAX`: increment `nBinaryCodes`, set `binaryCodes[index] = compile(code)`
  - Else: throw error `noteError("reach maximum compile code")`

#### Deletion

- `deleteWebCode(index)`
  - Move `binaryCodes[index]` to -1
  - Decrement index
  - Assert `index < 0`

### Executor

#### Initialization

- `initAgents(number)` → `MAX_AGENT_CODE = number`

#### Execution

- `executeWebCodes(bool, number)`
  - If `bool` is true: clean `exe_ctx`, set `nAgents = number`, call `separateCtx(number)`
  - Execute the code

#### Stop

- Stop execution and cleanup
