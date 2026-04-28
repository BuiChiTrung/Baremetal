#include "rtc.h"

#include "stm32f411xe.h"
#include "stm32f4xx.h"

#define RTC_WRITE_PROTECTION_KEY_1 ((uint8_t)0xCAU)
#define RTC_WRITE_PROTECTION_KEY_2 ((uint8_t)0x53U)
#define RTC_INIT_MASK 0xFFFFFFFFU
#define ISR_INITF (1U << 6)

#define WEEKDAY_MONDAY ((uint8_t)0x01U)
#define MONTH_APRIL ((uint8_t)0x04U)
#define TIME_FORMAT_PM (1U << 22)
#define CR_FMT (1U << 6)
#define ISR_RSF (1U << 5)

#define RTC_ASYNCH_PREDIV ((uint32_t)0x7F)
#define RTC_SYNCH_PREDIV ((uint32_t)0x00F9)

static uint8_t rtc_init_seq(void);
static void rtc_date_config(uint32_t WeekDay, uint32_t Day, uint32_t Month,
                            uint32_t Year);
static void rtc_time_config(uint32_t Format12_24, uint32_t Hours,
                            uint32_t Minutes, uint32_t Seconds);
static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler);
static void rtc_set_synch_prescaler(uint32_t SynchPrescaler);
static uint8_t exit_init_seq(void);

void rtc_init(void) {
  /*
  Enable clock access to Power controller.
  It's an on-chip hardware module responsible for managing the device's
  power supplies, voltage regulation, and energy-saving features
  */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  /*
  Disable the backup domain write protection.
  By default, the RTC and backup registers are write-protected to prevent
  accidental corruption. Setting the DBP (Disable Backup Domain write
  Protection) bit allows us to configure the RTC and write to the backup
  registers.
  */
  PWR->CR |= PWR_CR_DBP;

  /*
  Enable the Low Speed Internal (LSI) oscillator
  The RTC needs a low-speed clock source to keep time even in low power modes.
  The LSI is an internal RC oscillator typically running at ~32kHz.
  */
  RCC->CSR |= RCC_CSR_LSION;

  /*
  Wait for the LSI oscillator to stabilize and become ready.
  The hardware sets the LSIRDY flag once the oscillator output is stable
  enough to be used as a clock source. LSIRDY means LSI ready
  */
  while ((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY) {
  }

  /*
  Reset the backup domain
  This clears the RTC registers and backup registers to their default
  states, ensuring we start from a clean slate before configuring the RTC. First
  we assert the reset (set the bit), then we release the reset (clear the bit).
  */
  RCC->BDCR |= RCC_BDCR_BDRST;
  RCC->BDCR &= ~RCC_BDCR_BDRST;

  /*
  Select the LSI as the clock source for the RTC.
  The RTCSEL bits in the Backup Domain Control Register (BDCR) determine the RTC
  clock. Here, we set RTCSEL to 0b10 (bit 9 is 1, bit 8 is 0) to choose the LSI.
  */
  RCC->BDCR &= ~(1U << 8);
  RCC->BDCR |= (1U << 9);

  /*
  Enable the RTC clock.
  This powers up the RTC core, allowing it to start keeping time
  using the clock source we just selected (the LSI).
  */
  RCC->BDCR |= RCC_BDCR_RTCEN;

  /*
  Disable the RTC register write protection.
  Even after disabling the backup domain write protection, most RTC registers
  are still locked by a secondary protection mechanism to prevent spurious
  writes. Writing this specific key sequence (0xCA followed by 0x53) unlocks the
  RTC registers for configuration.
  */
  RTC->WPR = RTC_WRITE_PROTECTION_KEY_1;
  RTC->WPR = RTC_WRITE_PROTECTION_KEY_2;

  /*Enter the initialization mode*/
  if (rtc_init_seq() != 1) {
    // Do something...
  }

  rtc_date_config(WEEKDAY_MONDAY, 0x27, MONTH_APRIL, 0x26);

  /*
  Set desired time :  08:02:55 PM
  In reality, we get correct time using one of following methods:
  - User Input: The device has a screen and buttons (or a mobile app) where the
  user manually sets the clock once.
  - NTP (Network Time Protocol): If the device has WiFi or Ethernet, it connects
  to an internet server to fetch the exact time.
  - GPS: If the device has a GPS module, it receives an extremely accurate time
  signal from satellites.
  */
  rtc_time_config(TIME_FORMAT_PM, 0x08, 0x02, 0x55);

  // Set hour format
  RTC->CR |= CR_FMT;

  /*Set Asynch prescaler*/
  rtc_set_asynch_prescaler(RTC_ASYNCH_PREDIV);

  /*Set Sync prescaler*/
  rtc_set_synch_prescaler(RTC_SYNCH_PREDIV);

  /*Exit the initialization mode*/
  exit_init_seq();

  /*Enable RTC registers write protection*/
  RTC->WPR = 0xFF;
}

uint8_t rtc_convert_dec2bcd(uint8_t value) {
  return (uint8_t)((((value) / 10U) << 4U) | ((value) % 10U));
}

uint8_t rtc_convert_bcd2dec(uint8_t value) {
  return (uint8_t)(((uint8_t)((value) & (uint8_t)0xF0U) >> (uint8_t)0x4U) *
                       10U +
                   ((value) & (uint8_t)0x0FU));
}

uint32_t rtc_date_get_day(void) {
  return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_DT | RTC_DR_DU))) >>
                    RTC_DR_DU_Pos);
}

uint32_t rtc_date_get_year(void) {
  return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_YT | RTC_DR_YU))) >>
                    RTC_DR_YU_Pos);
}

uint32_t rtc_date_get_month(void) {
  return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_MT | RTC_DR_MU))) >>
                    RTC_DR_MU_Pos);
}

uint32_t rtc_time_get_second(void) {
  return (uint32_t)(READ_BIT(RTC->TR, (RTC_TR_ST | RTC_TR_SU)) >>
                    RTC_TR_SU_Pos);
}

uint32_t rtc_time_get_minute(void) {
  return (uint32_t)(READ_BIT(RTC->TR, (RTC_TR_MNT | RTC_TR_MNU)) >>
                    RTC_TR_MNU_Pos);
}

uint32_t rtc_time_get_hour(void) {
  return (uint32_t)((READ_BIT(RTC->TR, (RTC_TR_HT | RTC_TR_HU))) >>
                    RTC_TR_HU_Pos);
}

static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler) {
  MODIFY_REG(RTC->PRER, RTC_PRER_PREDIV_A,
             AsynchPrescaler << RTC_PRER_PREDIV_A_Pos);
}

static void rtc_set_synch_prescaler(uint32_t SynchPrescaler) {
  MODIFY_REG(RTC->PRER, RTC_PRER_PREDIV_S, SynchPrescaler);
}

static void rtc_date_config(uint32_t WeekDay, uint32_t Day, uint32_t Month,
                            uint32_t Year) {
  register uint32_t temp = 0U;

  temp = (WeekDay << RTC_DR_WDU_Pos) |
         (((Year & 0xF0U) << (RTC_DR_YT_Pos - 4U)) |
          ((Year & 0x0FU) << RTC_DR_YU_Pos)) |
         (((Month & 0xF0U) << (RTC_DR_MT_Pos - 4U)) |
          ((Month & 0x0FU) << RTC_DR_MU_Pos)) |
         (((Day & 0xF0U) << (RTC_DR_DT_Pos - 4U)) |
          ((Day & 0x0FU) << RTC_DR_DU_Pos));

  MODIFY_REG(RTC->DR,
             (RTC_DR_WDU | RTC_DR_MT | RTC_DR_MU | RTC_DR_DT | RTC_DR_DU |
              RTC_DR_YT | RTC_DR_YU),
             temp);
}

static void rtc_time_config(uint32_t Format12_24, uint32_t Hours,
                            uint32_t Minutes, uint32_t Seconds) {
  register uint32_t temp = 0U;

  temp = Format12_24 |
         (((Hours & 0xF0U) << (RTC_TR_HT_Pos - 4U)) |
          ((Hours & 0x0FU) << RTC_TR_HU_Pos)) |
         (((Minutes & 0xF0U) << (RTC_TR_MNT_Pos - 4U)) |
          ((Minutes & 0x0FU) << RTC_TR_MNU_Pos)) |
         (((Seconds & 0xF0U) << (RTC_TR_ST_Pos - 4U)) |
          ((Seconds & 0x0FU) << RTC_TR_SU_Pos));
  MODIFY_REG(RTC->TR,
             (RTC_TR_PM | RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU |
              RTC_TR_ST | RTC_TR_SU),
             temp);
}

void _rtc_enable_init_mode(void) { RTC->ISR = RTC_INIT_MASK; }

void _rtc_disable_init_mode(void) { RTC->ISR = ~RTC_INIT_MASK; }

uint8_t _rtc_isActiveflag_init(void) {

  return ((RTC->ISR & ISR_INITF) == ISR_INITF);
}

uint8_t _rtc_isActiveflag_rs(void) { return ((RTC->ISR & ISR_RSF) == ISR_RSF); }

static uint8_t rtc_init_seq(void) {
  /*Start init mode*/
  _rtc_enable_init_mode();

  /*Wait till we are in init mode*/
  while (_rtc_isActiveflag_init() != 1) {
  }

  return 1;
}

static uint8_t wait_for_synchro(void) {
  /*Clear RSF*/
  RTC->ISR &= ~ISR_RSF;

  /*Wait for registers synch*/
  while (_rtc_isActiveflag_rs() != 1) {
  }

  return 1;
}
static uint8_t exit_init_seq(void) {
  /*Stop init mode*/
  _rtc_disable_init_mode();

  /*Wait for registers to synchronise*/
  return (wait_for_synchro());
}
