################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/planificacion/subdir.mk
-include src/dtb/subdir.mk
-include src/cpu/subdir.mk
-include src/consola/subdir.mk
-include src/config/subdir.mk
-include src/commons/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SAFA

# Tool invocations
SAFA: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -Wl,-rpath='$$ORIGIN/../../qcommons/Qcommons' -L../../qcommons/Qcommons -o "SAFA" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) SAFA
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
