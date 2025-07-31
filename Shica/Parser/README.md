<!-- Please write the following content in English -->
# Shica Parser

Parser module for the Shica programming language. Implemented using the PEG/Leg parser generator.

## Overview

This parser analyzes Shica language source code and generates an Abstract Syntax Tree (AST). It uses Ian Piumarta's PEG/Leg parser generator to automatically generate `parser.c` from the `parser.leg` file.

## File Structure

- `parser.h` - Parser header file with public API definitions
- `parser.leg` - PEG grammar definition file (parser specification)
- `parser.c` - Auto-generated parser implementation (do not edit manually)

## API

### Basic Functions

- `int yyparse()` - Parse input and store result in the `result` global variable
- `void yyinit()` - Initialize parser and release resources

### Memory Management (when MSGC is enabled)

When MSGC is enabled, additional memory management features are available:

- `void yycollect()` - Mark processing for garbage collection

## Compile Options

### MSGC

```bash
gcc -DMSGC parser.c ../gc/msgc/msgc.c
```

Enables MSGC garbage collection. When enabled:

- `malloc`, `calloc`, `realloc`, `strdup` are replaced with GC versions
- `yycollect()` function becomes available

## Supported Language Features

Supports the following Shica language syntax:

- **State Definitions**: State definitions using the `stt` keyword
- **Events**: Event handling within states
- **Expressions**: Arithmetic, comparison, and logical expressions
- **Control Structures**: `if`, `else`, `while`, `for`
- **Functions**: Function definitions using the `fn` keyword
- **Variable Assignment**: Assignment using the `=` operator

## Usage Example

```c
#include "parser.h"

int main() {
    yyinit();

    // Execute parsing
    if (yyparse()) {
        // Parse successful, result is stored in result
        // result is of type oop (Object *)
    } else {
        // Parse failed
    }

    return 0;
}
```

## Build

Parser regeneration:

```bash
leg -o parser.c parser.leg
```

Regular compilation:

```bash
gcc parser.c -o parser
```

When using MSGC:

```bash
gcc -DMSGC parser.c ../gc/msgc/msgc.c -o parser
```

## Grammar

See the `parser.leg` file for detailed grammar specifications.

### Basic Tokens

- Identifiers: `[a-zA-Z_][a-zA-Z_0-9]*`
- Numbers: Integer literals
- Operators: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `<=`, `>`, `>=`
- Delimiters: `(`, `)`, `{`, `}`, `[`, `]`, `,`, `;`

### Comments

```
// Single-line comments
```

## Notes

- `parser.c` is an auto-generated file, do not edit manually
- Make grammar changes in the `parser.leg` file and regenerate with the leg command
- When using MSGC, proper garbage collection calls are required

## References

- [PEG/Leg: Ian Piumarta](https://www.piumarta.com/software/peg/)
- [Mark & Sweep Garbage Collection (MSGC)](../gc/msgc/README.md)
