<h1>Project to run 6963 graphics LCD from a ATmega328p</h1>

uses buspirate with avrdude to program a Pro Mini atmega328p<br />
see ProMiniPinout.txt and t6963_notes.txt for more info.<br />
run-avrdude will run avr with the appropriate parameters for my setup. I'm using a buspirate on the first<br />
USB port. Use getinfo to test the USB port. There are some files with the extention *.cx which are<br />
other *.c files to compile which have to be renamed because the Makefile will compile and try to<br /> 
link all *.c files and there will be an error trying to link multiple files with main(). So to burn the<br />
eeprom, rename eeprom_burn.cx to eeprom_burn.c and rename main.c to main.cx. Visa versa to run the main<br />
program. Before burning the eeprom, run "make set_eeprom_save_fuse" to set the EESAVE fuse. Then burn<br />
the eeprom. To clear the EESAVE fuse, run "make clear_eeprom_save_fuse" or "make set_default_fuses".<br />
To view what's in eeprom, go: "./run-avrdude -t". The -t does the terminal and then<br />
type "dump eeprom 0 0x03ff" to view the entire eeprom or use "0 512" to view what's currently in<br />
use. (very end of eeprom is used to store some variable "0x03f0 16").<br />
The flash is rated for 10,000 writes and the eeprom is rated for 100,000 writes.<br />
I'm willing to bet these are conservative estimates. i.e. if it was made on a Monday or a Friday.<br />

in the Makefile swap pound signs on lines 62 & 64 to disable/enable the TTY_DISPLAY define.
