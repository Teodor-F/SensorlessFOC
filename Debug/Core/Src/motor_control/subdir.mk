################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/motor_control/motor_control.c \
../Core/Src/motor_control/pi_cntrl.c \
../Core/Src/motor_control/pi_controller.c \
../Core/Src/motor_control/sv_modulation.c 

OBJS += \
./Core/Src/motor_control/motor_control.o \
./Core/Src/motor_control/pi_cntrl.o \
./Core/Src/motor_control/pi_controller.o \
./Core/Src/motor_control/sv_modulation.o 

C_DEPS += \
./Core/Src/motor_control/motor_control.d \
./Core/Src/motor_control/pi_cntrl.d \
./Core/Src/motor_control/pi_controller.d \
./Core/Src/motor_control/sv_modulation.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/motor_control/%.o Core/Src/motor_control/%.su Core/Src/motor_control/%.cyclo: ../Core/Src/motor_control/%.c Core/Src/motor_control/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G491xx -c -I"C:/Development/VenomESC/Core/Inc/motor_control" -I"C:/Development/VenomESC/Core/Inc/measurement" -I"C:/Development/VenomESC/Core/Inc/utilities" -I"C:/Development/VenomESC/Core/Inc/periphs" -I"C:/Development/VenomESC/Core/Inc/comm_protocols" -I"C:/Development/VenomESC/Core/Inc/mc" -I"C:/Development/VenomESC/Core/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"C:/Development/VenomESC/Drivers/CMSIS/Include" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -include"C:/Development/VenomESC/Core/Inc/config/hardware.h" -include"C:/Development/VenomESC/Core/Inc/utilities/numeric_constants.h" -include"C:/Development/VenomESC/Core/Inc/config/motor.h" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-motor_control

clean-Core-2f-Src-2f-motor_control:
	-$(RM) ./Core/Src/motor_control/motor_control.cyclo ./Core/Src/motor_control/motor_control.d ./Core/Src/motor_control/motor_control.o ./Core/Src/motor_control/motor_control.su ./Core/Src/motor_control/pi_cntrl.cyclo ./Core/Src/motor_control/pi_cntrl.d ./Core/Src/motor_control/pi_cntrl.o ./Core/Src/motor_control/pi_cntrl.su ./Core/Src/motor_control/pi_controller.cyclo ./Core/Src/motor_control/pi_controller.d ./Core/Src/motor_control/pi_controller.o ./Core/Src/motor_control/pi_controller.su ./Core/Src/motor_control/sv_modulation.cyclo ./Core/Src/motor_control/sv_modulation.d ./Core/Src/motor_control/sv_modulation.o ./Core/Src/motor_control/sv_modulation.su

.PHONY: clean-Core-2f-Src-2f-motor_control

