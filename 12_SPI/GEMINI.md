# Project Overview: 12_SPI (STM32F411 SPI with ADXL345)

This project implements a bare-metal SPI driver for an STM32F411 microcontroller to interface with an ADXL345 3-axis accelerometer. It reads raw acceleration data (X, Y, and Z axes) and converts it to g values, then outputs the results via UART.

## Main Technologies
- **Microcontroller:** STM32F411RET6 (ARM Cortex-M4)
- **Communication:**
  - **SPI1:** High-speed serial interface for the ADXL345 sensor.
  - **UART2:** Serial output for debugging and data logging (115200 baud).
- **Sensor:** ADXL345 Accelerometer.
- **Development Style:** CMSIS-based bare-metal C with direct register access.

## Architecture
- `Src/main.c`: Core application logic, including the main execution loop for reading and processing sensor data.
- `Src/spi.c` / `Inc/spi.h`: Low-level SPI1 peripheral configuration and data transfer functions (transmit/receive).
- `Src/adxl345.c` / `Inc/adxl345.h`: Driver implementation for the ADXL345, handling initialization, register-level read/write, and data formatting.
- `Src/uart.c` / `Inc/uart.h`: UART2 configuration for serial communication and `printf` redirection.

## Building and Running
### Build System
The project is managed by STM32CubeIDE.
- **Toolchain:** GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`).
- **Build Commands:**
  - Use STM32CubeIDE's build button.
  - Alternatively, use `make -C Debug all` (if the makefile is generated).
- **Linker Scripts:** `STM32F411RETX_FLASH.ld` (Flash-based execution) and `STM32F411RETX_RAM.ld` (RAM-based execution).

### Flashing and Debugging
- **Hardware:** ST-LINK/V2 (integrated on Nucleo-F411RE boards).
- **Configuration:** `12_SPI Debug.launch` defines the debug environment for STM32CubeIDE.

## Development Conventions
### Register Access
The project uses CMSIS-style direct register access (e.g., `RCC->AHB1ENR |= GPIOAEN`) for peripheral configuration, avoiding the standard ST HAL libraries for efficiency and direct hardware control.

### Pin Mapping
- **SPI1:**
  - `PA5`: SCK (Clock)
  - `PA6`: MISO (Master In Slave Out)
  - `PA7`: MOSI (Master Out Slave In)
  - `PA9`: CS (Chip Select - Active Low)
- **UART2:**
  - `PA2`: TX (Transmit)

### Standards
- **Data Types:** Uses `stdint.h` for portable integer types (`uint8_t`, `int16_t`, etc.).
- **Formatting:** Clean, modular C code with bitwise operations for hardware-level control.
