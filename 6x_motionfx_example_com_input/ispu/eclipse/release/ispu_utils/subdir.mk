################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
C:/Users/edringtj/OneDrive\ -\ STMicroelectronics/Documents/Github/Bose-Project/6x_motionfx_example_com_input/ispu/ispu_utils/crt0.S \
C:/Users/edringtj/OneDrive\ -\ STMicroelectronics/Documents/Github/Bose-Project/6x_motionfx_example_com_input/ispu/ispu_utils/global.S 

OBJS += \
./ispu_utils/crt0.o \
./ispu_utils/global.o 

S_UPPER_DEPS += \
./ispu_utils/crt0.d \
./ispu_utils/global.d 


# Each subdirectory must supply rules for building sources it contributes
ispu_utils/crt0.o: C:/Users/edringtj/OneDrive\ -\ STMicroelectronics/Documents/Github/Bose-Project/6x_motionfx_example_com_input/ispu/ispu_utils/crt0.S ispu_utils/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU STRED Cross Assembler'
	reisc-gcc -mcpu=reiscl -mfp32-format=ieee -Os -ffunction-sections -fdata-sections -Wall -Wextra -Wdouble-promotion -fno-strict-aliasing -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ispu_utils/global.o: C:/Users/edringtj/OneDrive\ -\ STMicroelectronics/Documents/Github/Bose-Project/6x_motionfx_example_com_input/ispu/ispu_utils/global.S ispu_utils/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU STRED Cross Assembler'
	reisc-gcc -mcpu=reiscl -mfp32-format=ieee -Os -ffunction-sections -fdata-sections -Wall -Wextra -Wdouble-promotion -fno-strict-aliasing -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-ispu_utils

clean-ispu_utils:
	-$(RM) ./ispu_utils/crt0.d ./ispu_utils/crt0.o ./ispu_utils/global.d ./ispu_utils/global.o

.PHONY: clean-ispu_utils

