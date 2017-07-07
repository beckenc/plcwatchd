################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jzon/Jzon.cpp 

OBJS += \
./jzon/Jzon.o 

CPP_DEPS += \
./jzon/Jzon.d 


# Each subdirectory must supply rules for building sources it contributes
jzon/%.o: ../jzon/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -I../pushover -I../jzon -I../snap7 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


