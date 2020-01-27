# This is the name of the file that shall be created. (It is also the name of my primary source file, without the file extension.)
TARGET = main

# create a variable that only pertains to this project
#MY_OWN_LIBRARY_DIR = /usr/home/MJ/Arduino/libraries/mj_midi

base = /home/mrh/Downloads/arduino-1.8.8-linux64/arduino-1.8.8

# "EXTRAINCDIRS" is a variable used by the base makefile. The base makefile creates a -I compiler flag for every item in the "EXTRAINCDIRS" list.
EXTRAINCDIRS = \
	${base}/hardware/arduino/avr/libraries/SPI/src \
	${base}/hardware/arduino/avr/cores/arduino    \
	${base}/hardware/arduino/avr/variants/standard \
	${base}/libraries/RF24                         \

# specify *.c source files pertaining to my project
SRC = 

# specify *.cpp source files pertaining to my project
PSRC = main.cpp 

# specify additional (non-core) Arduino libraries to include
ARDLIBS = RF24 

# include my base makefile
include ./Makefile.base
