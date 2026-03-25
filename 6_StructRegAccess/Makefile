# Variables
CC = arm-none-eabi-gcc 
CFLAGS = -mcpu=cortex-m4 -mthumb
LDFLAGS = -nostdlib -T stm32_ls.ld -Wl,-Map=5_Makefile.map

# Execute in chain to build final: "startup.o", "main.o" => "5_Makefile.elf" => "final"
final: 5_Makefile.elf

# Create elf using all obj files and .map file named shows where each part of 
# the code and data is loaded in memory, useful for debugging.
5_Makefile.elf: main.o startup.o
	$(CC) $(LDFLAGS) $^ -o $@ 

main.o: main.c
	$(CC) $(CFLAGS) -c $^ -o $@

startup.o: stm32f411_startup.c
	$(CC) $(CFLAGS) -c $^ -o $@

# executes OpenOCD, using a configuration file tailored for the 
# STM32 Nucleo F4 board, specifically st_nucleo_f4.cfg
load: 
	openocd -f /Users/straw/xpack-openocd-0.12.0-6/openocd/scripts/board/st_nucleo_f4.cfg

clean:
	rm -f *.o *.elf *.map

