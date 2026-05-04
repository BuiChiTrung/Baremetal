import json
import os

workspace = "/Users/straw/STM32CubeIDE/workspace_1.19.0"
json_path = os.path.join(workspace, "compile_commands.json")

with open(json_path, 'r') as f:
    data = json.load(f)

proj = "16_IWDG"
src_files = ["main.c", "syscalls.c", "sysmem.c", "uart.c", "exti.c", "gpio.c", "iwdg.c"]
startup_file = "startup_stm32f411retx.s"

new_entries = []

startup_entry = {
    "directory": f"{workspace}/{proj}/Debug",
    "command": f"arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MFStartup/startup_stm32f411retx.d -MTStartup/startup_stm32f411retx.o --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o Startup/startup_stm32f411retx.o {workspace}/{proj}/Startup/startup_stm32f411retx.s",
    "file": f"{workspace}/{proj}/Startup/startup_stm32f411retx.s"
}
new_entries.append(startup_entry)

for src in src_files:
    basename = src.replace('.c', '')
    cmd = f"arm-none-eabi-gcc {workspace}/{proj}/Src/{src} -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I{workspace}/{proj}/Inc -I{workspace}/{proj}/chip_headers/CMSIS/Include -I{workspace}/{proj}/chip_headers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MFSrc/{basename}.d -MTSrc/{basename}.o --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o Src/{basename}.o"
    entry = {
        "directory": f"{workspace}/{proj}/Debug",
        "command": cmd,
        "file": f"{workspace}/{proj}/Src/{src}"
    }
    new_entries.append(entry)

data = [d for d in data if f"/{proj}/" not in d['file']]
data.extend(new_entries)

with open(json_path, 'w') as f:
    json.dump(data, f, indent=2)

print("Updated compile_commands.json")
