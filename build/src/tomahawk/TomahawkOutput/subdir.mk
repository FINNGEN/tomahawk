################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tomahawk/TomahawkOutput/TomahawkOutputFilterController.cpp \
../src/tomahawk/TomahawkOutput/TomahawkOutputReader.cpp \
../src/tomahawk/TomahawkOutput/output_entry_support.cpp 

OBJS += \
./src/tomahawk/TomahawkOutput/TomahawkOutputFilterController.o \
./src/tomahawk/TomahawkOutput/TomahawkOutputReader.o \
./src/tomahawk/TomahawkOutput/output_entry_support.o 

CPP_DEPS += \
./src/tomahawk/TomahawkOutput/TomahawkOutputFilterController.d \
./src/tomahawk/TomahawkOutput/TomahawkOutputReader.d \
./src/tomahawk/TomahawkOutput/output_entry_support.d 


# Each subdirectory must supply rules for building sources it contributes
src/tomahawk/TomahawkOutput/%.o: ../src/tomahawk/TomahawkOutput/%.cpp
	g++ -std=c++0x -O3 -march=native -mtune=native -ftree-vectorize -pipe -frename-registers -funroll-loops -g -Wall -c -fmessage-length=0  -DVERSION=\"$(GIT_VERSION)\" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"


