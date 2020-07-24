################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../pushover/pushover.cpp

OBJS += \
./pushover/pushover.o

CPP_DEPS += \
./pushover/pushover.d


# Each subdirectory must supply rules for building sources it contributes
pushover/%.o: ../pushover/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: C++ Compiler'
	g++ -I../pushover -I../snap7 -I../main -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


