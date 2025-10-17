# MSGCS

`gc_roots`? / `cxt->roots[0]` is used for **Agent** data.


## web msgc

```
compiler and executor have each heap memory that gen by native malloc().

compiler:
|ctx|h.                       |

executor:
|ctx|h.                       |
or 
|ctx|ctx|h.      |ctx|h.      |
 |.   |- roots[0] have agent str.
 |- roots have each ctx loc.

```