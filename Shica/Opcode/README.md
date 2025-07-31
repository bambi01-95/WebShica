<!-- Please write the following content in English -->
# Opcode

Virtual machine instruction codes for the Shica language.

## API

- `opcode_t` - Opcode type definition
- `char *getOpcodeName(opcode_t op)` - Returns the name of the given opcode
- `printOP(opcode_t op)` - Prints opcode name with newline

## Instructions

- `iHALT` - Stop execution

## Build Options

- `-DDEBUG` - Enable debug output
