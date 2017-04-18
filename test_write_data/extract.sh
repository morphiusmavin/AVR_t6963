#copy this to 192.168.0.15:/home/dan/dev/test_write_data
#and copy ../main.h and ../sfr_helper.h to /home/dan/dev
tar xvf test_write.tar
make clean
mv menus.c parse_P24.c main.h t6963.* USART.* sfr_helper.h ..
make
rm *.o
rm *.tar
#if running as read (AVR)
#sudo ./test_data r
#if running as write (PIC24)
#sudo ./test_data w 400 0 0 200
