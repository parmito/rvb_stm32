################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/TaskAppCAN.c \
../Core/Src/TaskAppSerial.c \
../Core/Src/TaskIO.c \
../Core/Src/can.c \
../Core/Src/eeprom.c \
../Core/Src/freertos.c \
../Core/Src/gpio.c \
../Core/Src/main.c \
../Core/Src/state.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/TaskAppCAN.o \
./Core/Src/TaskAppSerial.o \
./Core/Src/TaskIO.o \
./Core/Src/can.o \
./Core/Src/eeprom.o \
./Core/Src/freertos.o \
./Core/Src/gpio.o \
./Core/Src/main.o \
./Core/Src/state.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/TaskAppCAN.d \
./Core/Src/TaskAppSerial.d \
./Core/Src/TaskIO.d \
./Core/Src/can.d \
./Core/Src/eeprom.d \
./Core/Src/freertos.d \
./Core/Src/gpio.d \
./Core/Src/main.d \
./Core/Src/state.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/STM32F1xx_HAL_Driver/Inc -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Middlewares/Third_Party/FreeRTOS/Source/include -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I/home/volkswagen2/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.6/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/TaskAppCAN.cyclo ./Core/Src/TaskAppCAN.d ./Core/Src/TaskAppCAN.o ./Core/Src/TaskAppCAN.su ./Core/Src/TaskAppSerial.cyclo ./Core/Src/TaskAppSerial.d ./Core/Src/TaskAppSerial.o ./Core/Src/TaskAppSerial.su ./Core/Src/TaskIO.cyclo ./Core/Src/TaskIO.d ./Core/Src/TaskIO.o ./Core/Src/TaskIO.su ./Core/Src/can.cyclo ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/can.su ./Core/Src/eeprom.cyclo ./Core/Src/eeprom.d ./Core/Src/eeprom.o ./Core/Src/eeprom.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/state.cyclo ./Core/Src/state.d ./Core/Src/state.o ./Core/Src/state.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

