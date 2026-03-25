#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

void led_init();
void led_on();
void led_off();
void btn_init();
bool is_btn_pressed();

#endif /* GPIO_H_ */
