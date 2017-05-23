################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Jzon.cpp \
../src/plc_state_watchdog.cpp \
../src/snap7.cpp 

OBJS += \
./src/Jzon.o \
./src/plc_state_watchdog.o \
./src/snap7.o 

CPP_DEPS += \
./src/Jzon.d \
./src/plc_state_watchdog.d \
./src/snap7.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


