cd ..
rmwhitesp.sh
cd test_write_data
make clean
tar cf test_write.tar test_write_data.c  makefile ../main.h ../menus.c ../t6963.* ../USART.* ../sfr_helper.h
scp test_write.tar 192.168.0.15:/home/dan/dev/AVR/AVR_t6963/test_write_data
scp test_write.tar 192.168.0.10:/home/dan/dev/AVR/AVR_t6963/test_write_data
rm *.tar
