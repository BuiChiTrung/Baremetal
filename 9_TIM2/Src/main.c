#include "stm32f411xe.h"

#include "gpio.h"
#include "tim2.h"

int main(void) {
  led_init();
  init_tim2();

  while (1) {
    toggle_led();
    // Wait for UIF: this flag set to 1 when counter reach auto-reload val
    while (!(TIM2->SR & TIM2_SR_UIF)) {
    }
    // Clear UIF flag
    TIM2->SR &= ~TIM2_SR_UIF;
  }
}
