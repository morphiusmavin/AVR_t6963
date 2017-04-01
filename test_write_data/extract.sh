#copy this to 192.168.0.15:/home/dan/dev/test_write_data
#and copy ../main.h and ../sfr_helper.h to /home/dan/dev
tar xvf test_write.tar
make clean
mv menus.c main.h t6963.* USART.* sfr_helper.h ..
make
rm *.o
rm *.tar
