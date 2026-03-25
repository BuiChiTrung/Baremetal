#include "gpio.h"
#include "systick.h"

int main(void) {
  led_init();

  while (1) {
    toggle_led();
    systick_delay_ms(5000);
  }
}
