project to run 6963 graphics LCD from a ATmega328p

uses buspirate with avrdude to program a Pro Mini atmega328p
see ProMiniPinout.txt for description of pins used.
run-avrdude will run avr with the appropriate parameters for my setup. I'm using a buspirate on the first USB port.
Use getinfo to test the USB port. 
run-test.sh <arg> will do a make clean;make upload; and create a file called <arg>.stats.txt and then called
flash_write.sh <arg> which will flash the chip and then do a make clean
flash_write.sh put the results in <arg>.hex_out
flash_read.sh <arg> will read the flash and put the results in <arg>.hex
there are some files with the extention *.cx which are other *.c files to compile which have to be renamed
because the Makefile will compile and try to link all *.c files and there will be an error trying to link
multiple files with main()