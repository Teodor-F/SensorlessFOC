################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/measurement/current_measure.c \
../Core/Src/measurement/measurement_layer_initializer.c \
../Core/Src/measurement/pll.c \
../Core/Src/measurement/sliding_mode_observer.c \
../Core/Src/measurement/smo.c \
../Core/Src/measurement/velocity_measure.c 

OBJS += \
./Core/Src/measurement/current_measure.o \
./Core/Src/measurement/measurement_layer_initializer.o \
./Core/Src/measurement/pll.o \
./Core/Src/measurement/sliding_mode_observer.o \
./Core/Src/measurement/smo.o \
./Core/Src/measurement/velocity_measure.o 

C_DEPS += \
./Core/Src/measurement/current_measure.d \
./Core/Src/measurement/measurement_layer_initializer.d \
./Core/Src/measurement/pll.d \
./Core/Src/measurement/sliding_mode_observer.d \
./Core/Src/measurement/smo.d \
./Core/Src/measurement/velocity_measure.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/measurement/%.o Core/Src/measurement/%.su Core/Src/measurement/%.cyclo: ../Core/Src/measurement/%.c Core/Src/measurement/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G491xx -c -I"C:/Development/VenomESC/Core/Inc/config" -I"C:/Development/VenomESC/Core/Inc/control" -I"C:/Development/VenomESC/Core/Inc/motor_control" -I"C:/Development/VenomESC/Core/Inc/measurement" -I"C:/Development/VenomESC/Core/Inc/utilities" -I"C:/Development/VenomESC/Core/Inc/periphs" -I"C:/Development/VenomESC/Core/Inc/comm_protocols" -I"C:/Development/VenomESC/Core/Inc/mc" -I"C:/Development/VenomESC/Core/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"C:/Development/VenomESC/Drivers/CMSIS/Include" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -include"C:/Development/VenomESC/Core/Inc/config/hardware.h" -include"C:/Development/VenomESC/Core/Inc/utilities/numeric_constants.h" -include"C:/Development/VenomESC/Core/Inc/config/motor.h" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-measurement

clean-Core-2f-Src-2f-measurement:
	-$(RM) ./Core/Src/measurement/current_measure.cyclo ./Core/Src/measurement/current_measure.d ./Core/Src/measurement/current_measure.o ./Core/Src/measurement/current_measure.su ./Core/Src/measurement/measurement_layer_initializer.cyclo ./Core/Src/measurement/measurement_layer_initializer.d ./Core/Src/measurement/measurement_layer_initializer.o ./Core/Src/measurement/measurement_layer_initializer.su ./Core/Src/measurement/pll.cyclo ./Core/Src/measurement/pll.d ./Core/Src/measurement/pll.o ./Core/Src/measurement/pll.su ./Core/Src/measurement/sliding_mode_observer.cyclo ./Core/Src/measurement/sliding_mode_observer.d ./Core/Src/measurement/sliding_mode_observer.o ./Core/Src/measurement/sliding_mode_observer.su ./Core/Src/measurement/smo.cyclo ./Core/Src/measurement/smo.d ./Core/Src/measurement/smo.o ./Core/Src/measurement/smo.su ./Core/Src/measurement/velocity_measure.cyclo ./Core/Src/measurement/velocity_measure.d ./Core/Src/measurement/velocity_measure.o ./Core/Src/measurement/velocity_measure.su

.PHONY: clean-Core-2f-Src-2f-measurement

