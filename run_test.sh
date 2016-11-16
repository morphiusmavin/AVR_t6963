#! /bin/bash
if [ $# -eq 0 ]
then
	echo "Usage: run_test.sh arg"
	echo "where arg is file to create without ext"
    echo "this will run flash_write.sh"
	exit 1
else
	make clean
	make upload
	avr-objdump -h $1.out > $1.stats.txt
	avr-size -C -d --mcu=atmega328p $1.out >> $1.stats.txt
	./flash_write.sh $1
	make clean
fi
