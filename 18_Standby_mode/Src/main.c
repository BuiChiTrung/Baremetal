#include "gpio_exti.h"
#include <stdio.h>

#include "standby_mode.h"
#include "uart.h"

/*
 * To test the project, start by pressing the blue push-button to enter Standby mode.
 * Remember, PA0 is configured as the wake-up pin and is active low.
 * In normal mode, connect a jumper wire from PA0 to ground.
 * To trigger a wake-up event, pull out the jumper wire and connect it to 3.3V,
 * causing a change in logic that will wake the microcontroller from Standby mode.
 *
 * */

uint8_t g_btn_press;
static void check_reset_source(void);

int main(void) {
    printf("Start firmware");

    /* 1. Initialize the UART so we can print debug messages via printf */
    uart_init();

    /* 2. Configure PA0 as a general-purpose input (our physical wakeup button) */
    wakeup_pin_init();

    /* 3. Check the power status to see if we just woke up from Standby mode
     *    or if this was a normal cold boot / reset. */
    check_reset_source();

    /* 4. Configure PC13 (blue push-button) as an external interrupt (EXTI).
     *    When pressed, it triggers EXTI15_10_IRQHandler. */
    pc13_exti_init();

    /* 5. Infinite loop. The microcontroller stays here during normal operation. */
    while (1) {}
}

static void check_reset_source(void) {
    /* Enable clock access to the Power Control (PWR) peripheral */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* Check the Standby Flag (SBF) in the Power Control Status Register (CSR).
     * If SBF is set, it means the system was in Standby mode and just woke up. */
    if ((PWR->CSR & PWR_CSR_SBF) == (PWR_CSR_SBF)) {

        /* Clear the Standby flag so we don't accidentally read it again on the next normal reset */
        PWR->CR |= PWR_CR_CSBF;

        printf("System resume from Standby.....\n\r");

        /* Wait here as long as the wakeup pin (PA0) is currently being held low.
         * This ensures the system doesn't immediately go back to sleep if the button is held. */
        while (get_wakeup_pin_state() == 0) {}
    }

    /* Check the Wakeup Flag (WUF). This flag is set when a wakeup event occurs on the WKUP pin.
     * We must clear it to ensure power registers are in clean state no matter it boot normally
     * or from standby mode */
    if ((PWR->CSR & PWR_CSR_WUF) == PWR_CSR_WUF) {
        PWR->CR |= PWR_CR_CWUF;
    }
}

/* Interrupt Service Routine for EXTI lines 10 through 15 (which includes PC13) */
void EXTI15_10_IRQHandler(void) {
    /* Check if the interrupt was specifically triggered by Line 13 (our PC13 button) */
    if ((EXTI->PR & LINE13) != 0) {
        /* Clear the Pending Register (PR) flag so the interrupt doesn't constantly re-trigger */
        EXTI->PR |= LINE13;

        /* Setup the Wakeup pin (PA0) hardware feature and immediately put the CPU into deep Standby
         * mode. Execution STOPS here until PA0 receives a wakeup signal. */
        standby_wakeup_pin_setup();
    }
}