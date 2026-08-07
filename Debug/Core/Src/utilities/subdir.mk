################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/utilities/lpf_butterworth.c \
../Core/Src/utilities/mc_callback.c \
../Core/Src/utilities/sv_transformation.c 

OBJS += \
./Core/Src/utilities/lpf_butterworth.o \
./Core/Src/utilities/mc_callback.o \
./Core/Src/utilities/sv_transformation.o 

C_DEPS += \
./Core/Src/utilities/lpf_butterworth.d \
./Core/Src/utilities/mc_callback.d \
./Core/Src/utilities/sv_transformation.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/utilities/%.o Core/Src/utilities/%.su Core/Src/utilities/%.cyclo: ../Core/Src/utilities/%.c Core/Src/utilities/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G491xx -c -I"C:/Development/VenomESC/Core/Inc/motor_control" -I"C:/Development/VenomESC/Core/Inc/measurement" -I"C:/Development/VenomESC/Core/Inc/utilities" -I"C:/Development/VenomESC/Core/Inc/periphs" -I"C:/Development/VenomESC/Core/Inc/comm_protocols" -I"C:/Development/VenomESC/Core/Inc/mc" -I"C:/Development/VenomESC/Core/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"C:/Development/VenomESC/Drivers/CMSIS/Include" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -include"C:/Development/VenomESC/Core/Inc/config/hardware.h" -include"C:/Development/VenomESC/Core/Inc/utilities/numeric_constants.h" -include"C:/Development/VenomESC/Core/Inc/config/motor.h" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-utilities

clean-Core-2f-Src-2f-utilities:
	-$(RM) ./Core/Src/utilities/lpf_butterworth.cyclo ./Core/Src/utilities/lpf_butterworth.d ./Core/Src/utilities/lpf_butterworth.o ./Core/Src/utilities/lpf_butterworth.su ./Core/Src/utilities/mc_callback.cyclo ./Core/Src/utilities/mc_callback.d ./Core/Src/utilities/mc_callback.o ./Core/Src/utilities/mc_callback.su ./Core/Src/utilities/sv_transformation.cyclo ./Core/Src/utilities/sv_transformation.d ./Core/Src/utilities/sv_transformation.o ./Core/Src/utilities/sv_transformation.su

.PHONY: clean-Core-2f-Src-2f-utilities

