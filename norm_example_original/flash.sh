#!/bin/sh

# Flash script for WSL

if [ $# -lt 1 ]
then
	echo "Usage: "$0" <disk_letter>"
else
	echo "Flashing to "$1":"

	disk_u=$1
	disk_l=$(echo $disk_u | tr '[:upper:]' '[:lower:]')

	sudo mkdir -p "/mnt/"$disk_l
	sudo mount -t drvfs $disk_u":" "/mnt/"$disk_l && cp build/nucleo_ispu.bin "/mnt/"$disk_l"/"
fi

