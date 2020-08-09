#! /bin/bash

function usage(){
	echo "-----------------------------------------------------------------------------------------------------"
	echo "Device name should be -p /dev/name/"
	echo "For the rest of options to execute the C program, Please refer to the usage function of the program"
	echo "-----------------------------------------------------------------------------------------------------"
}

echo "------This is layer 3------"
echo "The argument 1 is: $1"
echo "The argument 2 is: $2"
echo "The argument 3 is: $3"
echo "The argument 4 is: $4"
echo "The argument 5 is: $5"
echo "The argument 6 is: $6"
echo "The argument 7 is: $7"
echo "The argument 8 is: $8"
echo "The argument 9 is: $9"
echo "The argument 10 is: ${10}"

exe=$1
config_name=$2
Data_name=$3
device_name=$4
d_option=$5
b_option=$6
Buad_rate=$7
mode=$8
power=$9
pck_length=${10}

out_files=$(ls log_*)

echo "${out_files[0]}"
echo "${out_files[1]}"


#mv -i ${out_files[0]} ${out_files[1]}  /home/samuel/modem/results/

#awk 'BEGIN { FS = "\t\t\t"; } $3=="REC" {count++; print $1 "   " $2 "   " $3 "   " $4} END {print count}' log_08-19-2018_06\:27\:25PM.txt
