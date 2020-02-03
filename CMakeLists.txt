
project(battletank)

cmake_minimum_required(VERSION 2.6)

set(MCU "atmega328p" )
set(CPU_SPEED "16000000" )
set(PORT "/dev/ttyUSB0")
set(PORT_SPEED "57600")
set(PIN_VARIANT "standard")
set(ARDUINO_PATH "arduino")
set(PROGRAMMER "arduino")
set(COMPILE_FLAGS "")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
set(SRC_FILES main.cpp)

# Include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI/src
        ${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino
        ${ARDUINO_PATH}/hardware/arduino/avr/variants/standard
        ${ARDUINO_PATH}/libraries/SoftPWM
        ${ARDUINO_PATH}/libraries/RF24
        ${ARDUINO_PATH}/hardware/tools/avr/avr/include
)

# Find Arduino source files.
file(GLOB_RECURSE ARDUINO_CORE_SRC
        ${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino/*.c
        ${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino/*.cpp
        ${ARDUINO_PATH}/libraries/SoftPWM/*.cpp
        ${ARDUINO_PATH}/libraries/RF24/*.cpp
        ${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI/src/*.cpp
)

# Compiler suite specification
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_OBJCOPY avr-objcopy)
set(CMAKE_OBJDUMP avr-objdump)
set(CMAKE_RANLIB avr-ranlib)
set(CMAKE_LINKER avr-ld)

# Compiler flags
add_definitions(-DBAUD=9600UL -DARDUINO -DROLE=1)
add_definitions(-mmcu=${MCU} -DF_CPU=${CPU_SPEED})
add_definitions(-c -g -Os -Wall)
add_definitions(-fpack-struct -fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fshort-enums)

# Linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "-Os -Wl,--gc-sections -mmcu=${MCU}")

add_executable(${PROJECT_NAME} ${ARDUINO_CORE_SRC} ${SRC_FILES})

add_custom_target(flash
        COMMAND ${CMAKE_OBJCOPY} -j .text -j .data -O ihex ${PROJECT_NAME} main.hex
        COMMAND  avrdude -F -p${MCU} -carduino -P${PORT} -b${PORT_SPEED} -D -Uflash:w:main.hex:i
        DEPENDS ${PROJECT_NAME}
        )
