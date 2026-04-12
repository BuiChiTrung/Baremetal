#include "stm32f411xe.h"

#include "exti.h"
#include "gpio.h"
#include "uart.h"

#include <stdio.h>

int __io_putchar(int ch) {
  uart_write(ch);
  return ch;
}

// The handler name must be exactly the same as the one defined in the startup
// file startup_stm32f411retx.s
void EXTI15_10_IRQHandler(void) {
  if (EXTI->PR & LINE13) {
    // W1C technique: write 1 to clear pending bit
    EXTI->PR |= LINE13;

    printf("Button pressed!\n");
    led_toggle();
  }
}

int main(void) {
  uart_init();
  led_init();
  pc13_exti_init();

  while (1)
    ;
}