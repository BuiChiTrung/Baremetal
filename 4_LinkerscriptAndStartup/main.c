
#define PERIPH_BASE 0x40000000UL
#define AHB1PERIPH_OFFSET 0x20000UL
#define AHB1PERIPH_BASE (PERIPH_BASE + AHB1PERIPH_OFFSET)

#define GPIOA_OFFSET 0x0UL
#define GPIOA_BASE (AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET 0x3800UL
#define RCC_BASE (AHB1PERIPH_BASE + RCC_OFFSET)

#define RCC_AHB1EN_R_OFFSET 0x30UL
#define RCC_AHB1EN_R                                                           \
  (*(volatile unsigned int *)(RCC_BASE + RCC_AHB1EN_R_OFFSET))

#define GPIOA_MODE_R_OFFSET 0x0UL
#define GPIOA_MODE_R                                                           \
  (*(volatile unsigned int *)(GPIOA_BASE + GPIOA_MODE_R_OFFSET))

#define GPIOA_OD_R_OFFSET 0x14UL
#define GPIOA_OD_R (*(volatile unsigned int *)(GPIOA_BASE + GPIOA_OD_R_OFFSET))

#define GPIOA_EN (1U << 0)
#define PIN5 (1U << 5)
#define LED_PIN PIN5

int main() {
  // Enable clock access to GPIOA
  RCC_AHB1EN_R |= GPIOA_EN;
  // Set pin 5 as output by setting bit 10 = 0 and 11 = 1
  GPIOA_MODE_R |= (1 << 10);
  GPIOA_MODE_R &= ~(1U << 11);

  while (1) {
    // GPIOA_OD_R |= LED_PIN;
    GPIOA_OD_R ^= LED_PIN;
    for (int i = 0; i < 5000000; i++) {
    } // Delay loop for visible blinking
  }
}
