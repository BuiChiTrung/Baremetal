#include "uart.h"

#include <stdint.h>

#include "stm32f411xe.h"

#define SYS_CLK 16000000U
#define APB1_CLK SYS_CLK
#define UART_BAUD_RATE 115200

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

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate) {
  return ((periph_clk + (baudrate / 2U)) / baudrate);
}

void uart_write(int ch) {
  // Wait until transmit data reg is empty
  while (!(USART2->SR & USART_SR_TXE)) {
  }
  // Write data to transmit date reg
  USART2->DR = (ch & 0xFF);
}

void uart_init(void) {
  /*Enable clock access to GPIOA*/
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /*Set the mode of PA2 to alternate function mode*/
  GPIOA->MODER |= (1 << 5);
  GPIOA->MODER &= ~(1 << 4);

  /*Use PA2 as transmit line TX by setting afternate mode AF7*/
  GPIOA->AFR[0] |= (1 << 8);
  GPIOA->AFR[0] |= (1 << 9);
  GPIOA->AFR[0] |= (1 << 10);
  GPIOA->AFR[0] &= ~(1 << 11);

  /*Enable clock access to UART2*/
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  /*Configure uart baudrate*/
  USART2->BRR = compute_uart_bd(APB1_CLK, UART_BAUD_RATE);

  /*Configure transfer direction: USART2 is transmitter*/
  USART2->CR1 |= (USART_CR1_TE);

  /*Enable UART*/
  USART2->CR1 |= (USART_CR1_UE);
}
