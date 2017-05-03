#copy this to 192.168.0.15:/home/dan/dev/test_write_data
#and copy ../main.h and ../sfr_helper.h to /home/dan/dev
#scp test_write.tar 192.168.0.15:/home/dan/dev/AVR/AVR_t6963/test_write_data
tar xvf test_write.tar
make clean
mv main.c menus.c parse_P24.c  main.h t6963.* USART.* sfr_helper.h ..
mv eeprom/eeprom_burn.c ../eeprom
rmdir -r eeprom
make
rm *.o
rm *.tar
# if running as simulating PIC24:
#test_data.sh 600 0 0 100
# if running as simulationg AVR:
#test_data.sh r
