project to run 6963 graphics LCD from a ATmega328p

uses buspirate with avrdude to program a Pro Mini atmega328p
see ProMiniPinout.txt for description of pins used.
To compile for test_ports1.c which just tests the output of each port, do the bash script: ./run_testports.sh
To compile for the main program, run this file: ./run_lcd.sh
The first one will rename main.c to main.cx and t6963.c to t6963.cx. The other will rename these back and rename
test_ports1.cx to test_ports1.c. This is so I can use the same Makefile. 
run-avrdude will run avr with the appropriate parameters for my setup. I'm using a buspirate on the first USB port.
Use getinfo to test the USB port. 
