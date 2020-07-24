################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../snap7/snap7.cpp

OBJS += \
./snap7/snap7.o

CPP_DEPS += \
./snap7/snap7.d


# Each subdirectory must supply rules for building sources it contributes
snap7/%.o: ../snap7/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: C++ Compiler'
	g++ -I../snap7 -I../pushover -I../jzon -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


