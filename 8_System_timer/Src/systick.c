#include "systick.h"

#include "stm32f411xe.h"

#define SYSTICK_CTRL_ENABLE (1 << 0)
#define SYSTICK_CTRL_CLK_SRC_PROCESSOR (1 << 2)
#define SYSTICK_CTRL_COUNTFLAG (1 << 16)
#define CYCLES_PER_MS 16000

void systick_delay_ms(uint32_t delay_ms) {
  // Clear current value
  SysTick->VAL = 0;

  // Set load value to count down each ms
  SysTick->LOAD = (CYCLES_PER_MS - 1);

  // Enable the clock & set clock source
  SysTick->CTRL |= SYSTICK_CTRL_ENABLE;
  SysTick->CTRL |= SYSTICK_CTRL_CLK_SRC_PROCESSOR;

  // While loop to wait for delay_ms
  for (int i = 0; i < delay_ms; ++i) {
    // Wait until counter val be 0
    while ((SysTick->CTRL & SYSTICK_CTRL_COUNTFLAG) == 0) {
    }
  }
}