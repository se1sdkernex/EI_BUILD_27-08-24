################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BitManip.c \
../Core/Src/Config.c \
../Core/Src/Diagnosis.c \
../Core/Src/Hitachi.c \
../Core/Src/Last_Reset_Reason.c \
../Core/Src/Medha.c \
../Core/Src/Process_Cmd.c \
../Core/Src/RTC.c \
../Core/Src/Siemens.c \
../Core/Src/Ticks.c \
../Core/Src/User_UDP.c \
../Core/Src/can.c \
../Core/Src/crc.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/iwdg.c \
../Core/Src/kyosan.c \
../Core/Src/main.c \
../Core/Src/rng.c \
../Core/Src/spi.c \
../Core/Src/stm32f7xx_hal_msp.c \
../Core/Src/stm32f7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f7xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/BitManip.o \
./Core/Src/Config.o \
./Core/Src/Diagnosis.o \
./Core/Src/Hitachi.o \
./Core/Src/Last_Reset_Reason.o \
./Core/Src/Medha.o \
./Core/Src/Process_Cmd.o \
./Core/Src/RTC.o \
./Core/Src/Siemens.o \
./Core/Src/Ticks.o \
./Core/Src/User_UDP.o \
./Core/Src/can.o \
./Core/Src/crc.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/iwdg.o \
./Core/Src/kyosan.o \
./Core/Src/main.o \
./Core/Src/rng.o \
./Core/Src/spi.o \
./Core/Src/stm32f7xx_hal_msp.o \
./Core/Src/stm32f7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f7xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/BitManip.d \
./Core/Src/Config.d \
./Core/Src/Diagnosis.d \
./Core/Src/Hitachi.d \
./Core/Src/Last_Reset_Reason.d \
./Core/Src/Medha.d \
./Core/Src/Process_Cmd.d \
./Core/Src/RTC.d \
./Core/Src/Siemens.d \
./Core/Src/Ticks.d \
./Core/Src/User_UDP.d \
./Core/Src/can.d \
./Core/Src/crc.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/iwdg.d \
./Core/Src/kyosan.d \
./Core/Src/main.d \
./Core/Src/rng.d \
./Core/Src/spi.d \
./Core/Src/stm32f7xx_hal_msp.d \
./Core/Src/stm32f7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f7xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM_32=1 -DEI_1=1 -c -I../Core/Inc -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/Components/dp83848 -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/BitManip.d ./Core/Src/BitManip.o ./Core/Src/BitManip.su ./Core/Src/Config.d ./Core/Src/Config.o ./Core/Src/Config.su ./Core/Src/Diagnosis.d ./Core/Src/Diagnosis.o ./Core/Src/Diagnosis.su ./Core/Src/Hitachi.d ./Core/Src/Hitachi.o ./Core/Src/Hitachi.su ./Core/Src/Last_Reset_Reason.d ./Core/Src/Last_Reset_Reason.o ./Core/Src/Last_Reset_Reason.su ./Core/Src/Medha.d ./Core/Src/Medha.o ./Core/Src/Medha.su ./Core/Src/Process_Cmd.d ./Core/Src/Process_Cmd.o ./Core/Src/Process_Cmd.su ./Core/Src/RTC.d ./Core/Src/RTC.o ./Core/Src/RTC.su ./Core/Src/Siemens.d ./Core/Src/Siemens.o ./Core/Src/Siemens.su ./Core/Src/Ticks.d ./Core/Src/Ticks.o ./Core/Src/Ticks.su ./Core/Src/User_UDP.d ./Core/Src/User_UDP.o ./Core/Src/User_UDP.su ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/can.su ./Core/Src/crc.d ./Core/Src/crc.o ./Core/Src/crc.su ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/iwdg.su ./Core/Src/kyosan.d ./Core/Src/kyosan.o ./Core/Src/kyosan.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/rng.d ./Core/Src/rng.o ./Core/Src/rng.su ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32f7xx_hal_msp.d ./Core/Src/stm32f7xx_hal_msp.o ./Core/Src/stm32f7xx_hal_msp.su ./Core/Src/stm32f7xx_it.d ./Core/Src/stm32f7xx_it.o ./Core/Src/stm32f7xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f7xx.d ./Core/Src/system_stm32f7xx.o ./Core/Src/system_stm32f7xx.su ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

