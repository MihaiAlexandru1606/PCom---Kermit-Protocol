#!/bin/bash

SPEED=10
DELAY=10
LOSS=5
CORRUPT=20
FILES=(files/file1.bin files/file2.bin files/file3.bin)

killall link
killall kreceiver
killall ksender

mv files src/files

cd ./src
cd ./link_emulator
make clean
make
cd ../
make clean
make

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT &> /dev/null &
sleep 1
./kreceiver &
sleep 1

./ksender "${FILES[@]}"

echo "==========================="
for file in "${FILES[@]}"
do
	DIFF=$(diff $file recv_$file) 
	if [ "$DIFF" != "" ] 
	then
		echo "Files $file and recv_$file are not the same!"
	fi
done
echo "==========================="

rm -fr files

cd ./link_emulator
make clean
cd ../
make clean
rm -fr recv_*
killall link
