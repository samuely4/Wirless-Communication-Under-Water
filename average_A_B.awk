#! /bin/awk -f
BEGIN {
	total_A = 0;
	total_B = 0;
	average_A = 0;
	average_B = 0;
	num_A = 0;
	num_B = 0;
}

{

	total_A = total_A + $1
	total_B = total_B + $2
	num_A += 1;
	num_B += 1;
}

END{
	average_A = total_A / num_A;
	average_B = total_B / num_B;
	print "total A  : " total_A;
	print "total B  : " total_B; 
	print "num_A    : " num_A;
	print "num_B    : " num_B;
	print "average A: " average_A;
	print "average B: " average_B;
}