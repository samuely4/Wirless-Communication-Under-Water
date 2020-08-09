#! /bin/bash

function usage(){
	echo "---------------------------------------------------------------"
	echo "File layer2.sh must be in the same directory"
	echo "The C program directory you provided might be wrong"
	echo "---------------------------------------------------------------"
}

echo "------This is layer 1------"

C_file="serTest.c"
EXEFName="modemexe"
num_modes=5

if [ -f $C_file ]
	then
		echo "ok....serTest.c exists"
		gcc $C_file -lpthread -o $EXEFName
	else

		echo "Something went wrong\n"
		usage
		exit 1
fi


for((i=1; i<=$num_modes; i++))
do
	echo "THE MODE THAT IS BEING TESTED IS: $i"
	if [ -f layer2.sh ]
		then
			./layer2.sh $i $EXEFName
		else
			echo "Something went wrong \n"
			usage
			exit 1
	fi
done

#awk 'BEGIN { FS = "\t\t\t"; } $3=="SND" {count++; print $1 "   " $2 "   " $3 "   " $4} END {print count}' log_08-19-2018_05\:16\:18PM.txt


