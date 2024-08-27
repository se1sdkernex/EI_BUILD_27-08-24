################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/_ei/EI_FINAL_Tested_10Peers_17-06-24_SeqMod_Diagnosis/EI_1/Drivers/BSP/Components/dp83848/dp83848.c 

OBJS += \
./Drivers/BSP/Components/dp83848/dp83848.o 

C_DEPS += \
./Drivers/BSP/Components/dp83848/dp83848.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/dp83848/dp83848.o: D:/_ei/EI_FINAL_Tested_10Peers_17-06-24_SeqMod_Diagnosis/EI_1/Drivers/BSP/Components/dp83848/dp83848.c Drivers/BSP/Components/dp83848/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM_32=1 -DEI_2=1 -c -I../../EI_1/Core/Inc -I../../EI_1/LWIP/App -I../../EI_1/LWIP/Target -I../../EI_1/Middlewares/Third_Party/LwIP/src/include -I../../EI_1/Middlewares/Third_Party/LwIP/system -I../../EI_1/Drivers/STM32F7xx_HAL_Driver/Inc -I../../EI_1/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../../EI_1/Drivers/BSP/Components/dp83848 -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../EI_1/Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/lwip -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/netif -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/compat/posix -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../../EI_1/Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../../EI_1/Middlewares/Third_Party/LwIP/system/arch -I../../EI_1/Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-dp83848

clean-Drivers-2f-BSP-2f-Components-2f-dp83848:
	-$(RM) ./Drivers/BSP/Components/dp83848/dp83848.d ./Drivers/BSP/Components/dp83848/dp83848.o ./Drivers/BSP/Components/dp83848/dp83848.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-dp83848

