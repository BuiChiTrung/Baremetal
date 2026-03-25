#include "gpio.h"

int main(void) {
  led_init();
  btn_init();

  while (1) {
    if (is_btn_pressed()) {
      led_on();
    } else {
      led_off();
    }
  }
}
