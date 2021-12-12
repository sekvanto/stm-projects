The module is basically done. There are two tiny pitfalls:

1. `uart_read()` internally uses size of `nbyte *= 2`. This, i suspect, is not fully correct, but this hack works fine.
2. Beware, when you use `uart_write()`, always add `Delay(10)` (10 ms) after it.

Later i might need to fix both hacks. But it suffices for now.
