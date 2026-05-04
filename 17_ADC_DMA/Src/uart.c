#include "uart.h"

#include <stdint.h>

#include "stm32f411xe.h"

#define SYS_CLK 16000000U
#define APB1_CLK SYS_CLK
#define UART_BAUD_RATE 115200

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
