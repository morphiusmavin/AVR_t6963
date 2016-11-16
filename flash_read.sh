#! /bin/bash
if [ $# -eq 0 ]
then
    echo "Usage: flash_read.sh arg"
    echo "where arg is file to create with ext: hex"
    echo "to create a file called dan.hex: flash_read.sh dan"
    exit 1
else
    echo "creating $1.hex"
    ./run-avrdude -v -U flash:r:$1.hex:r
fi
