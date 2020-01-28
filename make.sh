
make -f Makefile.mk ROLE=0; avrdude -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:main.hex:i
make -f Makefile.mk ROLE=1; avrdude -v -patmega328p -carduino -P/dev/ttyUSB1 -b57600 -D -Uflash:w:main.hex:i


