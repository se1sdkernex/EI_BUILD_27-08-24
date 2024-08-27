################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
D:/_ei/EI_FINAL_Tested_10Peers_17-06-24_SeqMod_Diagnosis/EI_1/Core/Startup/startup_stm32f769bitx.s 

OBJS += \
./Core/Startup/startup_stm32f769bitx.o 

S_DEPS += \
./Core/Startup/startup_stm32f769bitx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/startup_stm32f769bitx.o: D:/_ei/EI_FINAL_Tested_10Peers_17-06-24_SeqMod_Diagnosis/EI_1/Core/Startup/startup_stm32f769bitx.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -DSTM_32=1 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f769bitx.d ./Core/Startup/startup_stm32f769bitx.o

.PHONY: clean-Core-2f-Startup

