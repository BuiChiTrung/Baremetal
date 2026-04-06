#include "uart.h"

#include <stdio.h>

/*
`__io_putchar` acts as retargeting bridge redirects standard C library output
printf to the STM32's UART.

### How it works:
1.  When you call a function like `printf()`, the standard C library (Newlib)
eventually calls the low-level system call `_write` (found in `Src/syscalls.c`).
2.  Inside `Src/syscalls.c`, the `_write` function is implemented to iterate
through each character in a string and pass it to `__io_putchar`:
    ```c
    // From Src/syscalls.c
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
       __io_putchar(*ptr++);
    }
    ```
3.  **UART Redirection**: By defining `__io_putchar` in your `uart.c` file, you
provide the actual hardware implementation. Your code takes the character `ch`
and passes it to `uart_write(ch)`, which waits for the USART2 transmit register
to be empty and then sends the byte.

### Summary
Without this function, `printf` would do nothing. By implementing it, you enable
"Serial Debugging," allowing you to use standard I/O functions to send data to a
serial terminal via USART2.
*/
int __io_putchar(int ch) {
  uart_write(ch);
  return ch;
}

int main(void) {
  /*Initialize debug UART*/
  uart_init();

  while (1) {
    printf("Hello from the other side ai...\r\n");
  }
}