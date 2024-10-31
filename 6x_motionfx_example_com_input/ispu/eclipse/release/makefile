################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include ispu_utils/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ispu_sensor_fusion_6x
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
ispu_sensor_fusion_6x.srec \

SECONDARY_SIZE += \
ispu_sensor_fusion_6x.siz \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: ispu_sensor_fusion_6x.elf secondary-outputs

# Tool invocations
ispu_sensor_fusion_6x.elf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU STRED Cross C Linker'
	reisc-gcc -mcpu=reiscl -mfp32-format=ieee -Os -ffunction-sections -fdata-sections -Wall -Wextra -Wdouble-promotion -fno-strict-aliasing -T ../../ispu_utils/boot.ld -Xlinker --gc-sections -L../../lib -Wl,-Map,"ispu_sensor_fusion_6x.map" --specs=nano.specs -o "ispu_sensor_fusion_6x.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lMotionFX
	@echo 'Finished building target: $@'
	@echo ' '

ispu_sensor_fusion_6x.srec: ispu_sensor_fusion_6x.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU STRED Cross Create Flash Image'
	reisc-objcopy -O srec "ispu_sensor_fusion_6x.elf"  "ispu_sensor_fusion_6x.srec"
	@echo 'Finished building: $@'
	@echo ' '

ispu_sensor_fusion_6x.siz: ispu_sensor_fusion_6x.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU STRED Cross Print Size'
	reisc-size --format=berkeley "ispu_sensor_fusion_6x.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ispu_sensor_fusion_6x.elf ispu_sensor_fusion_6x.siz ispu_sensor_fusion_6x.srec
	-@echo ' '

post-build:
	-ispu_gen -b -c -s ../../conf.txt -d imu_22 -n ispu_conf ispu_sensor_fusion_6x.srec > ispu.h && ispu_gen -b -s ../../conf.txt -d imu_22 -n ispu_conf ispu_sensor_fusion_6x.srec > ispu.ucf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
