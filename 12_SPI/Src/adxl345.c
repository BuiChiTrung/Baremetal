#include "adxl345.h"

/********PINOUT********/

/*   STM32    ------ ADXL345*/

/* PA6(MISO)  ------ SDO
 * PA7(MOSI)  ------ SDA
 * PA5(SCK)   ------ SCL
 * PA9(SS)    ------ CS
 * GND        ------ GND
 * 5V+        ------ VCC
 * */
void adxl_read(uint8_t address, uint8_t *rxdata) {
  // Set the read operation bit in the address byte
  address |= ADXL345_READ_OPERATION;

  // Set the multi-byte read enable bit to read multiple consecutive registers
  address |= ADXL345_MULTI_BYTE_ENABLE;

  // Pull CS (chip select) line low to activate the slave device
  cs_enable();

  // This transmits the address (with read and multi-byte bits set) to the
  // ADXL345
  // ???? how adxl can handle when receiving this
  // Send pointer as spi1_transmit expects a pointer to read an array
  spi1_transmit(&address, 1);

  // Read 6 bytes of data from the accelerometer (X, Y, Z axes - 2 bytes each)
  spi1_receive(rxdata, 6);

  // Pull CS line high to deactivate the slave device and end the transaction
  cs_disable();
}

void adxl_write(uint8_t address, uint8_t value) {
  uint8_t data[2];

  /*Enable multi-byte, place address into buffer*/
  data[0] = address | ADXL345_MULTI_BYTE_ENABLE;

  /*Place data into buffer*/
  data[1] = value;

  /*Pull cs line low to enable slave*/
  cs_enable();

  /*Transmit data and address*/
  spi1_transmit(data, 2);

  /*Pull cs line high to disable slave*/
  cs_disable();
}

void adxl_init(void) {
  /*Enable SPI gpio*/
  spi_gpio_init();

  /*Config SPI*/
  spi1_config();

  /*Set data format range to +-4g*/
  adxl_write(ADXL345_REG_DATA_FORMAT, ADXL345_RANGE_4G);

  /*Reset all bits*/
  adxl_write(ADXL345_REG_POWER_CTL, ADXL345_RESET);

  /*Configure power control measure bit*/
  adxl_write(ADXL345_REG_POWER_CTL, ADXL345_MEASURE_BIT);
}
