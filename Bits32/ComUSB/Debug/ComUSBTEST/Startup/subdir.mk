################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../ComUSBTEST/Startup/startup_stm32f103c8tx.s 

OBJS += \
./ComUSBTEST/Startup/startup_stm32f103c8tx.o 

S_DEPS += \
./ComUSBTEST/Startup/startup_stm32f103c8tx.d 


# Each subdirectory must supply rules for building sources it contributes
ComUSBTEST/Startup/%.o: ../ComUSBTEST/Startup/%.s ComUSBTEST/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-ComUSBTEST-2f-Startup

clean-ComUSBTEST-2f-Startup:
	-$(RM) ./ComUSBTEST/Startup/startup_stm32f103c8tx.d ./ComUSBTEST/Startup/startup_stm32f103c8tx.o

.PHONY: clean-ComUSBTEST-2f-Startup

