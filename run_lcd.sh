mv test_ports1.c test_ports1.cx
mv main.cx main.c
mv t6963.cx t6963.c
make clean
make upload
avr-objdump -h testLCD.out > lcd_stats.txt
avr-size -C -d --mcu=atmega328p testLCD.out >> lcd_stats.txt
#./flash_lcdw.sh
