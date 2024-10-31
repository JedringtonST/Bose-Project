################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/edringtj/OneDrive\ -\ STMicroelectronics/Documents/Github/Bose-Project/6x_motionfx_example_com_input/ispu/src/main.c 

C_DEPS += \
./src/main.d 

OBJS += \
./src/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/main.o: C:/Users/edringtj/OneDrive\ -\ STMicroelectronics/Documents/Github/Bose-Project/6x_motionfx_example_com_input/ispu/src/main.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU STRED Cross C Compiler'
	reisc-gcc -mcpu=reiscl -mfp32-format=ieee -Os -ffunction-sections -fdata-sections -Wall -Wextra -Wdouble-promotion -fno-strict-aliasing -I../../ispu_utils -I../../lib -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/main.d ./src/main.o

.PHONY: clean-src

