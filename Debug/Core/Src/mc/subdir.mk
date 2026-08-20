################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/mc/foc.c \
../Core/Src/mc/foc_math.c \
../Core/Src/mc/mc_svm.c 

OBJS += \
./Core/Src/mc/foc.o \
./Core/Src/mc/foc_math.o \
./Core/Src/mc/mc_svm.o 

C_DEPS += \
./Core/Src/mc/foc.d \
./Core/Src/mc/foc_math.d \
./Core/Src/mc/mc_svm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/mc/%.o Core/Src/mc/%.su Core/Src/mc/%.cyclo: ../Core/Src/mc/%.c Core/Src/mc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G491xx -c -I"C:/Development/VenomESC/Core/Inc/config" -I"C:/Development/VenomESC/Core/Inc/control" -I"C:/Development/VenomESC/Core/Inc/motor_control" -I"C:/Development/VenomESC/Core/Inc/measurement" -I"C:/Development/VenomESC/Core/Inc/utilities" -I"C:/Development/VenomESC/Core/Inc/periphs" -I"C:/Development/VenomESC/Core/Inc/comm_protocols" -I"C:/Development/VenomESC/Core/Inc/mc" -I"C:/Development/VenomESC/Core/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"C:/Development/VenomESC/Drivers/CMSIS/Include" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -include"C:/Development/VenomESC/Core/Inc/config/hardware.h" -include"C:/Development/VenomESC/Core/Inc/utilities/numeric_constants.h" -include"C:/Development/VenomESC/Core/Inc/config/motor.h" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-mc

clean-Core-2f-Src-2f-mc:
	-$(RM) ./Core/Src/mc/foc.cyclo ./Core/Src/mc/foc.d ./Core/Src/mc/foc.o ./Core/Src/mc/foc.su ./Core/Src/mc/foc_math.cyclo ./Core/Src/mc/foc_math.d ./Core/Src/mc/foc_math.o ./Core/Src/mc/foc_math.su ./Core/Src/mc/mc_svm.cyclo ./Core/Src/mc/mc_svm.d ./Core/Src/mc/mc_svm.o ./Core/Src/mc/mc_svm.su

.PHONY: clean-Core-2f-Src-2f-mc

