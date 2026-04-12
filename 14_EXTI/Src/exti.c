#include "exti.h"
#include "stm32f411xe.h"

void pc13_exti_init(void) {
  // Disable IRQ in configure process to avoid race condition
  __disable_irq();

  // Enable clock access for GPIOC
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  /*Set PC13 as input*/
  GPIOC->MODER &= ~(1 << 26);
  GPIOC->MODER &= ~(1 << 27);

  // Enable clock access to SYSCFG
  // System Configuration Controller (SYSCFG) is an on-chip hardware module
  // (specifically a peripheral) in the STM32F411xC/E microcontroller
  // It is primarily responsible for managing low-level system-wide
  // configurations that allow the processor to interact correctly with memory
  // and external pins
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // Connect EXTI line 13 to PC13 by selecting port C for line 13
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

  // Enable IRQ on line 13
  EXTI->IMR |= LINE13;

  // Note: the button pin (PC13) on your board reads high when not pressed.
  // Trigger IRQ on line 13 when the signal is
  // Rising edge: button is released (0V to 3.3V)
  // Falling edge: button is pressed (3.3V to 0V) => choose this
  EXTI->FTSR |= LINE13;

  // Enable handler to handle IRQ from line 10 to line 15 (these lines share the
  // same IRQ handler)
  NVIC_EnableIRQ(EXTI15_10_IRQn);

  // Enable global interrupts
  __enable_irq();
}
