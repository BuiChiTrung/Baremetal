#include "stm32f411xe.h"

#include "tim2.h"

#define APB1ENR_TIM2EN (1U << 0)
#define TIM2_CEN (1U << 0)

void init_tim2() {
  // Enable clock access
  RCC->APB1ENR |= APB1ENR_TIM2EN;

  // Set prescale
  TIM2->PSC = 1600 - 1;

  // Set auto-reload val
  TIM2->ARR = 10000 - 1;

  // Reset counter
  TIM2->CNT = 0;

  // Enable timer
  TIM2->CR1 |= TIM2_CEN;
}