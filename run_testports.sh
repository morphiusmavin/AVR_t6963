mv main.c main.cx
mv t6963.c t6963.cx
mv test_ports1.cx test_ports1.c
make clean
make upload
avr-objdump -h testLCD.out > test_ports_stats.txt
avr-size -C -d --mcu=atmega328p testLCD.out >> test_ports_stats.txt
./flash_lcdw.sh
