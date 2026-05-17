#include "standby_mode.h"
#define PWR_MODE_STANDBY (PWR_CR_PDDS)
#define WK_PIN (1U << 0)

static void set_power_mode(uint32_t pwr_mode);

void wakeup_pin_init(void) {
    // Enable clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Set PA0 as input pin
    GPIOA->MODER &= ~(1U << 0);
    GPIOA->MODER &= ~(1U << 1);

    /* Configure PA0 with no internal pull-up or pull-down resistors (Floating).
     * The PUPDR register uses 2 bits per pin. Bits [1:0] control PA0.
     * Clearing both bits to '00' disables internal resistors, preventing
     * unnecessary current leakage since the pin is driven externally. */
    GPIOA->PUPDR &= ~(1U << 0);
    GPIOA->PUPDR &= ~(1U << 1);
}

void standby_wakeup_pin_setup(void) {

    /*Wait for wakeup pin to be released*/
    while (get_wakeup_pin_state() == 0) {}

    /*Disable wakeup pin*/
    PWR->CSR &= ~PWR_CSR_EWUP;

    /*Clear all wakeup flags*/
    PWR->CR |= (1U << 2);

    /* In STM32 hardware, the Wakeup circuitry is hardwired directly to PA0. Setting this specific
     * bit is what fundamentally activates PA0's special ability to boot the microcontroller out of
     * deep sleep!*/
    PWR->CSR |= PWR_CSR_EWUP;

    /*Enter StandBy mode*/
    set_power_mode(PWR_MODE_STANDBY);

    /*Set SLEEPDEEP bit in the CortexM System Control Register*/
    SCB->SCR |= (1U << 2);

    /* Frozen on this line,
     * The only way out is a Wakeup event on PA0, which will cause the microcontroller to reboot
     * from the very beginning of main().*/
    __WFI();
}

uint32_t get_wakeup_pin_state(void) { return ((GPIOA->IDR & WK_PIN) == WK_PIN); }

static void set_power_mode(uint32_t pwr_mode) {
    MODIFY_REG(PWR->CR, (PWR_CR_PDDS | PWR_CR_LPDS | PWR_CR_FPDS | PWR_CR_LPLVDS | PWR_CR_MRLVDS),
               pwr_mode);
}