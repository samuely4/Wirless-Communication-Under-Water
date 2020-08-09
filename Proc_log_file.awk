
#! /usr/bin/awk -f
BEGIN {
	FS = "\t\t\t";
}

{
	if ((NF == 4) && ($3 == "REC")){
		if ($4 ~ /^\$MMRXA/){
			REC_time1 = $2;
			SND_time_str1 = $4;
			subs = substr(SND_time_str1, 26, 12);
			print REC_time1 "\t" subs >> "clocks.txt";
		}
	}
}

END{

	print "....This is the end of Proc_log_file program";
	print "....preparing to calculate clock drift between two machines";
}
