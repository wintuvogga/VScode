TARGET = program


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# source path


# firmware library path
PERIFLIB_PATH =

# Build path 
BUILD_DIR = build


###################################### 
# source
######################################
# C sources
C_SOURCES =  \
Src/main.c \
Src/system_stm32f4xx.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
Src/RTOS/timers.c \
Src/RTOS/tasks.c \
Src/RTOS/queue.c \
Src/RTOS/port.c \
Src/RTOS/list.c \
Src/RTOS/heap_1.c \
Src/RTOS/event_groups.c \
Src/Drivers/Bluetooth/BT121Driver.c \
Src/Drivers/LCD/SSD1306.c \
Src/Drivers/LCD/glcdfont.c \
Src/Drivers/LCD/Graphics.c \


# ASM sources
ASM_SOURCES =  \
startup_stm32f405xx.s


######################################
# firmware library
######################################
PERIFLIB_SOURCES =


#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc -x assembler-with-cpp
CP = arm-none-eabi-objcopy
AR = arm-none-eabi-ar
SZ = arm-none-eabi-size
HEX = arm-none-eabi-objcopy -O ihex
BIN = arm-none-eabi-objcopy -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DSTM32F405xx \
-D__FPU_PRESENT

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-ISrc/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
-ISrc/Drivers/STM32F4xx_HAL_Driver/Inc \
-ISrc/Drivers/Inc \
-ISrc/Drivers/Bluetooth \
-ISrc/Drivers/LCD \
-ISrc/Inc \
-ISrc/Drivers/CMSIS/Include \
-ISrc/RTOS/Inc \

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information 
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F405RGTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
