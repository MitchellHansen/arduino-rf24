avr-gcc -Os -DF_CPU=16000000L -DBAUD=9600UL -mmcu=atmega328p main.c -o main.o
avr-objcopy -O ihex main.o main.hex
avrdude -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:main.hex:i
 rm main.o main.hex
