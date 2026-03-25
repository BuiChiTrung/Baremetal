// This header'll include stm32f411xe.h as we specified using STM32F411xE
// microcontroller in SMT32CubeIDE
#include "stm32f4xx.h"

#define GPIOAEN (1U << 0)
#define PIN5 (1U << 5)
#define LED_PIN PIN5

int main(void) {
  // We can access register like with C struct
  RCC->AHB1ENR |= GPIOAEN;
  GPIOA->MODER |= (1U << 10);  // 10: Set bit 10 to 1
  GPIOA->MODER &= ~(1U << 11); // 11: Set bit 11 to 0

  while (1) {
    GPIOA->ODR ^= LED_PIN;
    for (int i = 0; i < 5000000; i++) {
    }
  }
}
