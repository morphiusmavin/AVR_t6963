<h1>Project to run 6963 graphics LCD from a ATmega328p</h1>

uses buspirate with avrdude to program a Pro Mini atmega328p<br />
see ProMiniPinout.txt and t6963_notes.txt for more info.<br />
run-avrdude will run avr with the appropriate parameters for my setup. I'm using a buspirate on the first<br />
USB port. Use getinfo to test the USB port. There are some files with the extention *.cx which are<br />
other *.c files to compile which have to be renamed because the Makefile will compile and try to<br /> 
link all *.c files and there will be an error trying to link multiple files with main(). So to burn the<br />
eeprom, rename eeprom_burn.cx to eeprom_burn.c and rename main.c to main.cx. Visa versa to run the main<br />
program. Before burning the eeprom, run "Makefile set_default_fuses" to unset the EESAVE fuse. Then burn<br />
the eeprom, then run "Makefile set_eeprom_save_fuse" to set the EESAVE fuse so you can reflash the chip<br />
without erasing the eeprom.<br /> 
