#include "uart.h"

#include <stdio.h>

int main(void) {
  /*Initialize debug UART*/
  uart_init();

  while (1) {
    printf("Hello from the other side ai...\r\n");
  }
}