// #include "spi.h"

// #include <stdio.h>

// #define SR_TXE (1U << 1)
// #define SR_RXNE (1U << 0)

// #define SR_BSY (1U << 7)

// // PA5 -> CLK
// // PA6 -> MISO
// // PA7 -> MOSI

// // PA9 -> Slave Select

// void spi_gpio_init(void) {
//   /*Enable clock access to GPIOA*/
//   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

//   /*Set PA5,PA6,PA7 mode to alternate function*/
//   /*PA5*/
//   GPIOA->MODER &= ~(1 << 10);
//   GPIOA->MODER |= (1 << 11);

//   /*PA6*/
//   GPIOA->MODER &= ~(1 << 12);
//   GPIOA->MODER |= (1 << 13);

//   /*PA7*/
//   GPIOA->MODER &= ~(1 << 14);
//   GPIOA->MODER |= (1 << 15);

//   /*Set PA9 as output pin*/
//   GPIOA->MODER |= (1 << 18);
//   GPIOA->MODER &= ~(1 << 19);

//   /*Set PA5,PA6,PA7 alternate function type to SPI1*/
//   GPIOA->AFR[0] |= (0x5 << 20); /*PA5*/
//   GPIOA->AFR[0] |= (0x5 << 24); /*PA6*/
//   GPIOA->AFR[0] |= (0x5 << 28); /*PA7*/
// }

// void spi1_config(void) {
//   /*Enable clock access to SPI1 module*/
//   RCC->APB1ENR |= RCC_APB2ENR_SPI1EN;

//   /*Set clock to fPCLK/4*/
//   SPI1->CR1 |= (1 << 3);
//   SPI1->CR1 &= ~(1 << 4);
//   SPI1->CR1 &= ~(1 << 5);

//   /*Set CPOL to 1 and CPHA to 1*/
//   SPI1->CR1 |= (1 << 0);
//   SPI1->CR1 |= (1 << 1);

//   /*Enable full duplex*/
//   SPI1->CR1 &= ~(1 << 10);

//   /*Set MSB first*/
//   SPI1->CR1 &= ~(1 << 7);

//   /*Set mode to MASTER*/
//   SPI1->CR1 |= (1 << 2);

//   /*Set 8 bit data mode*/
//   SPI1->CR1 &= ~(1 << 11);

//   /*Select software slave management by
//    * setting SSM=1 and SSI=1*/
//   SPI1->CR1 |= (1 << 9);
//   SPI1->CR1 |= (1 << 8);

//   /*Enable SPI module*/
//   SPI1->CR1 |= (1 << 6);
// }

// void spi1_transmit(uint8_t *data, uint32_t size) {
//   for (uint32_t i = 0; i < size; i++) {
//     // Wait until transmit buffer is empty (TXE = 1) and not busy (BSY = 0)
//     printf("Start waiting for SPI to be not busy\r\n");
//     while (!(SPI1->SR & SR_TXE))
//       ;
//     SPI1->DR = data[i];
//   }

//   // Wait until last byte is transmitted (TXE = 1) and SPI is not busy (BSY =
//   0) printf("Finish waiting for SPI to be not busy\r\n"); while ((SPI1->SR &
//   SR_BSY))
//     ;

//   /*
//     After the master transmits data through the Data reg, data reg captures
//     the data received from the slave. To ensure incoming data is properly
//     processed, we read the Data Reg, even if we don’t need the value. This
//     read operation automatically clears the OVR flag. It’s also advisable to
//     read the Status Register as part of this process.
//   */
//   uint8_t temp;
//   temp = SPI1->DR;
//   temp = SPI1->SR;
// }

// void spi1_receive(uint8_t *data, uint32_t size) {
//   for (int i = 0; i < size; ++i) {
//     SPI1->DR = 0x0;
//     // Wait until receive buffer not empty
//     while (!(SPI1->SR & SPI_SR_RXNE))
//       ;
//     data[i] = SPI1->DR;
//   }
// }

// void cs_enable(void) {
//   // Pull low pin 9 to enable slave select
//   GPIOA->ODR &= ~GPIO_ODR_OD9;
// }

// void cs_disable(void) {
//   // Pull high pin9 to disable slave select
//   GPIOA->ODR |= GPIO_ODR_OD9;
// }