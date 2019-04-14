
if [ $# -eq 0 ]
    then
        file=main.c
    else
        file=$1
fi

ofile=$(echo $file | sed "s/\.c/\.o/")
hexfile=$(echo $file | sed "s/\.c/\.hex/")

avr-gcc -Os -DF_CPU=16000000L -DBAUD=9600UL -mmcu=atmega328p $file -o $ofile 
avr-objcopy -O ihex $ofile $hexfile
avrdude -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:$hexfile:i
rm $ofile $hexfile
