#copy this to 192.168.0.15:/home/dan/dev/test_write_data
#and copy ../main.h and ../sfr_helper.h to /home/dan/dev
tar xvf test_write.tar
make clean
mv menu.c main.h ..
make
rm *.o
