#stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

stty -F /dev/ttyUSB0 9600 -parenb -parodd cs8 -hupcl \
-cstopb cread clocal -crtscts -iuclc -ixany -imaxbel \
-iutf8 -opost -olcuc -ocrnl -onlcr -onocr -onlret -ofill \
 -ofdel nl0 cr0 tab0 bs0 vt0 ff0 -isig -icanon -iexten \
-echo -echoe -echok -echonl -noflsh -xcase -tostop -echoprt \
-echoctl -echoke


tail -f /dev/ttyUSB0
