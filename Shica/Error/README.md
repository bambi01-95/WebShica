# Error

This is used in the compiler for collecting error messages.

## Error Types

The following error types are supported:

- `ERROR_WARNING` (0): Warning messages
- `ERROR_ERROR` (1): Error messages
- `ERROR_UNSUPPORTED` (2): Unsupported operations

## How to use

### Basic Usage

1. **Initialize the error list:**

```c
Error *ErrorHeader = NULL;
initErrorList(&ErrorHeader);
```

2. **Define a convenient macro (optional):**

```c
#define errorMSG(T,L,M) reportError(&ErrorHeader,T,L,M)
```

3. **Report errors:**

```c
// Using the function directly
reportError(&ErrorHeader, ERROR_ERROR, 42, "Syntax error");

// Using the macro (if defined)
errorMSG(ERROR_WARNING, 15, "Unused variable");
```

4. **Clean up when done:**

```c
freeErrorList(&ErrorHeader);
```

### Complete Example

```c
#include "Error.h"

int main() {
    Error *ErrorHeader = NULL;
    initErrorList(&ErrorHeader);

    #define errorMSG(T,L,M) reportError(&ErrorHeader,T,L,M)

    // Report some errors
    errorMSG(ERROR_WARNING, 10, "Variable 'x' is unused");
    errorMSG(ERROR_ERROR, 25, "Undefined function 'foo'");

    // Process errors here...

    // Clean up
    freeErrorList(&ErrorHeader);
    return 0;
}
```
