################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/hal/stm32g4/hal_adc_st32g4.c \
../Core/Src/hal/stm32g4/hal_hall_stm32g4.c \
../Core/Src/hal/stm32g4/hal_pwm_stm32g4.c 

OBJS += \
./Core/Src/hal/stm32g4/hal_adc_st32g4.o \
./Core/Src/hal/stm32g4/hal_hall_stm32g4.o \
./Core/Src/hal/stm32g4/hal_pwm_stm32g4.o 

C_DEPS += \
./Core/Src/hal/stm32g4/hal_adc_st32g4.d \
./Core/Src/hal/stm32g4/hal_hall_stm32g4.d \
./Core/Src/hal/stm32g4/hal_pwm_stm32g4.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/hal/stm32g4/%.o Core/Src/hal/stm32g4/%.su Core/Src/hal/stm32g4/%.cyclo: ../Core/Src/hal/stm32g4/%.c Core/Src/hal/stm32g4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G491xx -c -I"C:/Development/VenomESC/Core/Inc/hal" -I"C:/Development/VenomESC/Core/Inc/comm_protocols" -I"C:/Development/VenomESC/Core/target" -I"C:/Development/VenomESC/Core/Inc/mc" -I"C:/Development/VenomESC/Core/target/NUCLEO_IHM08M1" -I"C:/Development/VenomESC/Core/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"C:/Development/VenomESC/Drivers/CMSIS/Include" -I"C:/Development/VenomESC/Drivers/STM32G4xx_HAL_Driver/Inc" -I"C:/Development/VenomESC/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-hal-2f-stm32g4

clean-Core-2f-Src-2f-hal-2f-stm32g4:
	-$(RM) ./Core/Src/hal/stm32g4/hal_adc_st32g4.cyclo ./Core/Src/hal/stm32g4/hal_adc_st32g4.d ./Core/Src/hal/stm32g4/hal_adc_st32g4.o ./Core/Src/hal/stm32g4/hal_adc_st32g4.su ./Core/Src/hal/stm32g4/hal_hall_stm32g4.cyclo ./Core/Src/hal/stm32g4/hal_hall_stm32g4.d ./Core/Src/hal/stm32g4/hal_hall_stm32g4.o ./Core/Src/hal/stm32g4/hal_hall_stm32g4.su ./Core/Src/hal/stm32g4/hal_pwm_stm32g4.cyclo ./Core/Src/hal/stm32g4/hal_pwm_stm32g4.d ./Core/Src/hal/stm32g4/hal_pwm_stm32g4.o ./Core/Src/hal/stm32g4/hal_pwm_stm32g4.su

.PHONY: clean-Core-2f-Src-2f-hal-2f-stm32g4

