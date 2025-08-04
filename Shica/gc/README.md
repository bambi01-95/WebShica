# Garbage Collection

Garbage collection implementations for the Shica language.

## Structure

- `gc.h` - Common header file for GC modules (don't include specific GC headers directly)
- `MSGC/` - Mark and sweep garbage collector
- `MSGCS/` - Mark and sweep garbage collector for multiple heaps
- `CPGC/` - Copy garbage collector
- `test/` - Test files and benchmarks
