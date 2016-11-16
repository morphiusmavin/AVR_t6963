#! /bin/bash
if [ $# -eq 0 ]
then
    echo "usage: flash_write.sh arg"
    echo "where arg is hex file to flash without ext"
    echo "the ext will be *.hex"
    echo "to flash the file: dan.hex ->flash_write.sh dan"
else
    ./run-avrdude -v -U flash:w:$1.hex > $1.hex_out
    echo "results in file: $1.hex_out"
    cat $1.hex_out
fi
