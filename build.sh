

REMOTEPORT=/dev/ttyUSB0
DOZERPORT=/dev/ttyUSB1





if [ $# -eq 0 ]
    then
        if make -f Makefile.mk ROLE=1; then
            echo Compilation Good 
        else
            echo Compilation Bad
        fi
    else

        if [ $1 -eq 1 ]
            then
                if make -f Makefile.mk ROLE=1; then
                    avrdude -v -patmega328p -carduino -P$DOZERPORT -b57600 -D -Uflash:w:main.hex:i
                else
                    echo Compilation failed, skipping flash
            fi
        elif [ $1 -eq 0 ]
            then
                if make -f Makefile.mk ROLE=0; then
                    avrdude -v -patmega328p -carduino -P$REMOTEPORT -b57600 -D -Uflash:w:main.hex:i
                else
                    echo Compilation failed, skipping flash
            fi
        fi
    fi



