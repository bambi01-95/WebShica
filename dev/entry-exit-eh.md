# entry() and exit() event handler

```
---- state definition ----
+---+
entry stt
+---+
setup state
op(trans-relpc): push a relpc to global variable. 
+---+
exit stt
+---+
op(): pop a value from global variable and set it to next pc.
----
```