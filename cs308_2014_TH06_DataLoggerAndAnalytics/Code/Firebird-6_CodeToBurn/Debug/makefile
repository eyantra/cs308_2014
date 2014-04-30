################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: I2C_ADC_DEMO.axf

# Tool invocations
I2C_ADC_DEMO.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\girraj\Documents\LPCXpresso_4.1.5_219\workspace\CMSISv2_LPC17xx\Debug" -L"C:\Users\girraj\Documents\LPCXpresso_4.1.5_219\workspace\FB6Lib_v_01_00\Debug" -Xlinker -Map="I2C_ADC_DEMO.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "I2C_ADC_DEMO_Debug.ld" -o"I2C_ADC_DEMO.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) I2C_ADC_DEMO.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "I2C_ADC_DEMO.axf"; arm-none-eabi-objcopy -O binary "I2C_ADC_DEMO.axf" "I2C_ADC_DEMO.bin" ; checksum -p LPC1769 -d "I2C_ADC_DEMO.bin"; arm-none-eabi-objcopy -O ihex I2C_ADC_DEMO.axf I2C_ADC_DEMO.hex;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
