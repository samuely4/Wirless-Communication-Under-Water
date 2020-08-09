#! /bin/bash 

function usage(){
	echo "---------------------------------------------------------------"
	echo "File layer3.sh must be in the same directory"
	echo "The directory you provided for config file might be wrong"
	echo "The directory you provided for the data file might be wrong"
	echo "---------------------------------------------------------------"
}

Pck_1=608
Pck_2=1280
Pck_3=1952
Pck_4=2624
Pck_5=3968

Power=0
mode=$1
EXEFName=$2
num_trials_per_mode=10
Data_name="data1.txt"
config_name="configuration1.txt"
device_name="/dev/ttyUSB0"
d_option="-d"
b_option="-b"
Baud_rate="115200"


Pck_arr=($Pck_1 $Pck_2 $Pck_3 $Pck_4 $Pck_5)

echo "------This is layer 2------"

echo "The elements of Packet array length is ${Pck_arr[*]}"

echo "This mode is $1"

echo "the name of the executable is: $EXEFName"

if [ -f $config_name ]
	then
		echo "ok.....configuration file exists"
	else
		echo "Something went wrong."
		usage
		exit 1
fi

if [ -f $Data_name ]
	then
		echo "ok.....data file exists"
	else
		echo "Something went wrong."
		usage
		exit 1
fi

if [ -f layer3.sh ]
	then
		echo "....preparing to execute layer3...."
		for ((i=0; i<$num_trials_per_mode; i++))
		do
			let "Power = Power + 10"
			sed -i 's/#Power Level:.*/#Power Level: '$Power'/g' $config_name
			sed -i 's/PWL:.*/PWL:'$Power'/g' $config_name
			./layer3.sh $EXEFName  $config_name  $Data_name  $device_name  $d_option  $b_option  $Baud_rate  $mode  $Power  ${Pck_arr[$(( mode - 1 ))]}
		done
	else
		echo "Something went wrong\n"
		usage
		exit 1
fi
