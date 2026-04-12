#include "gpio.h"

#include "stm32f411xe.h"

#define GPIOCEN (1U << 2)
#define PIN13_STATE (1U << 13)
#define GPIOA_ODR_PIN5 (1U << 5)

void led_init() {
  // Enable clock to port A
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // Set PA5 mode as output
  GPIOA->MODER |= (1U << 10);  // Set bit
  GPIOA->MODER &= ~(1U << 11); // Clear bit
}

void led_toggle() { GPIOA->ODR ^= GPIOA_ODR_PIN5; }

void btn_init() {
  // Enable clock to port C
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  // Set PC13 mode as input
  GPIOC->MODER &= ~(1U << 26);
  GPIOC->MODER &= ~(1U << 27);
}
