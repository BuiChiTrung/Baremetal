#include "gpio.h"

#include "stm32f411xe.h"

#define GPIOAEN (1U << 0)
#define LED_BS5 (1U << 5)
#define LED_BR5 (1U << 21)
#define GPIOA_ODR_PIN5 (1U << 5)

void led_init() {
  // Enable clock to port A
  RCC->AHB1ENR |= GPIOAEN;

  // Set PA5 mode as output
  GPIOA->MODER |= (1U << 10);  // Set bit
  GPIOA->MODER &= ~(1U << 11); // Clear bit
}

void led_on() { GPIOA->BSRR |= LED_BS5; }

void led_off() { GPIOA->BSRR |= LED_BR5; }

void toggle_led() { GPIOA->ODR ^= GPIOA_ODR_PIN5; }