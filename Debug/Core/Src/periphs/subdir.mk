################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/periphs/adc.c \
../Core/Src/periphs/hall.c \
../Core/Src/periphs/mctimer.c \
../Core/Src/periphs/periph_layer_initializer.c \
../Core/Src/periphs/pwm.c 

OBJS += \
./Core/Src/periphs/adc.o \
./Core/Src/periphs/hall.o \
./Core/Src/periphs/mctimer.o \
./Core/Src/periphs/periph_layer_initializer.o \
./Core/Src/periphs/pwm.o 

C_DEPS += \
./Core/Src/periphs/adc.d \
./Core/Src/periphs/hall.d \
./Core/Src/periphs/mctimer.d \
./Core/Src/periphs/periph_layer_initializer.d \
./Core/Src/periphs/pwm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/periphs/%.o Core/Src/periphs/%.su Core/Src/periphs/%.cyclo: ../Core/Src/periphs/%.c Core/Src/periphs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G491xx -c -I"C:/Development/VenomESC/Core/Inc/config" -I"C:/Development/VenomESC/Core/Inc/control" -I"C:/Development/VenomESC/Core/Inc/motor_control" -I"C:/Development/VenomESC/Core/Inc/measurement" -I"C:/Development/VenomESC/Core/Inc/utilities" -I"C:/Development/VenomESC/Core/Inc/periphs" -I"C:/Development/VenomESC/Core/Inc/comm_protocols" -I"C:/Development/VenomESC/Core/Inc/mc" -I"C:/Development/VenomESC/Core/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"C:/Development/VenomESC/Drivers/CMSIS/Include" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -include"C:/Development/VenomESC/Core/Inc/config/hardware.h" -include"C:/Development/VenomESC/Core/Inc/utilities/mc_types.h" -include"C:/Development/VenomESC/Core/Inc/utilities/numeric_constants.h" -include"C:/Development/VenomESC/Core/Inc/config/motor.h" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-periphs

clean-Core-2f-Src-2f-periphs:
	-$(RM) ./Core/Src/periphs/adc.cyclo ./Core/Src/periphs/adc.d ./Core/Src/periphs/adc.o ./Core/Src/periphs/adc.su ./Core/Src/periphs/hall.cyclo ./Core/Src/periphs/hall.d ./Core/Src/periphs/hall.o ./Core/Src/periphs/hall.su ./Core/Src/periphs/mctimer.cyclo ./Core/Src/periphs/mctimer.d ./Core/Src/periphs/mctimer.o ./Core/Src/periphs/mctimer.su ./Core/Src/periphs/periph_layer_initializer.cyclo ./Core/Src/periphs/periph_layer_initializer.d ./Core/Src/periphs/periph_layer_initializer.o ./Core/Src/periphs/periph_layer_initializer.su ./Core/Src/periphs/pwm.cyclo ./Core/Src/periphs/pwm.d ./Core/Src/periphs/pwm.o ./Core/Src/periphs/pwm.su

.PHONY: clean-Core-2f-Src-2f-periphs

