echo "creating $1.hex"
./run-avrdude -v -U flash:r:$1.hex:r
sudo chown dan:dan $1.hex
