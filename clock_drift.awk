#! /usr/bin/awk -f
function time_min_sec(value1, value2){
	if (value1 < value2){
		return ((60 - value2) + value1);
	}
	else{
		return (value2 - value1);
	}
}

function time_millisec(value1, value2){
	if (value1 < value2){
		return ((1000 - value2) + value1);
	}
	else{
		return (value2 - value1);
	}
}

BEGIN {
	FS = "\t";

	REC1_hour = 0;
	REC1_min = 0;
	REC1_sec = 0;
	REC1_millisec = 0;

	SND1_hour = 0;
	SND1_min = 0;
	SND1_sec = 0;
	SND1_millisec = 0;

	REC2_hour = 0;
	REC2_min = 0;
	REC2_sec = 0;
	REC2_millisec = 0;


	SND2_hour = 0;
	SND2_min = 0;
	SND2_sec = 0;
	SND2_millisec = 0;
	
	Exp_start_hour = 0;
	Exp_start_min = 0;
	Exp_start_sec = 0;
	Exp_start_millisec = 0;
}

{
	if (NR == 1){

		REC_time1 = $1;
		SND_time1 = $2;

		split(REC_time1, array1, ":");
		split(SND_time1, array2, ":");

		REC1_hour = array1[1];
		REC1_min = array1[2];
		REC1_sec = array1[3];
		REC1_millisec = array1[4];

		SND1_hour = array2[1];
		SND1_min = array2[2];
		SND1_sec = array2[3];
		SND1_millisec = array2[4];
		
		Exp_start_hour = array2[1];
		Exp_start_min = array2[2];
		Exp_start_sec = array2[3];
		Exp_start_millisec = array2[4];

		next;
	}

	print "recieved 1 time: " REC1_hour "\t" REC1_min "\t" REC1_sec "\t" REC1_millisec;

	print "sent 1 time: " SND1_hour "\t" SND1_min "\t" SND1_sec "\t" SND1_millisec;

	REC_time2 = $1;
	SND_time2 = $2;
	split(REC_time2, array3, ":");
	split(SND_time2, array4, ":");

	REC2_hour = array3[1];
	REC2_min = array3[2];
	REC2_sec = array3[3];
	REC2_millisec = array3[4];

	SND2_hour = array4[1];
	SND2_min = array4[2];
	SND2_sec = array4[3];
	SND2_millisec = array4[4];

	print "recieved 2 time: " REC2_hour "\t" REC2_min "\t" REC2_sec "\t" REC2_millisec;

	print "sent 2 time: " SND2_hour "\t" SND2_min "\t" SND2_sec "\t" SND2_millisec;

	hours_det = SND2_hour - SND1_hour;
	minutes_det = time_min_sec(SND2_min, SND1_min);
	sec_det = time_min_sec(SND2_sec, SND1_sec);
	millisec_det = time_millisec(SND2_millisec, SND1_millisec);
	
	determinant = (hours_det*3600) + (minutes_det * 60) + (sec_det) + (millisec_det / 1000);
	
	print "determinant elements: " hours_det "\t" minutes_det "\t" sec_det "\t" millisec_det;

	print "determinant offset is: " determinant;
	
	REC1_sub1_hour = REC1_hour - Exp_start_hour;
	REC1_sub1_min = time_min_sec(REC1_min, Exp_start_min);
	REC1_sub1_sec = time_min_sec(REC1_sec, Exp_start_sec);
	REC1_sub1_millisec = time_millisec(REC1_millisec, Exp_start_millisec);
	
	REC1_sub1 = (REC1_sub1_hour * 3600) + (REC1_sub1_min * 60) + (REC1_sub1_sec) + (REC1_sub1_millisec / 1000);
	print "reciever 1: " REC1_sub1_hour "\t" REC1_sub1_min "\t" REC1_sub1_sec "\t" REC1_sub1_millisec;
	print "reciever 1 formated time: " REC1_sub1;
	
	SND1_sub1_hour = SND1_hour - Exp_start_hour;
	SND1_sub1_min = time_min_sec(SND1_min, Exp_start_min);
	SND1_sub1_sec = time_min_sec(SND1_sec, Exp_start_sec);
	SND1_sub1_millisec = time_millisec(SND1_millisec, Exp_start_millisec);
	
	SND1_sub1 = (SND1_sub1_hour * 3600) + (SND1_sub1_min * 60) + (SND1_sub1_sec) + (SND1_sub1_millisec / 1000);
	print "sender 1: " SND1_sub1_hour "\t" SND1_sub1_min "\t" SND1_sub1_sec "\t" SND1_sub1_millisec;
	print "sender 1 formated time: " SND1_sub1;
	
	REC2_sub1_hour = REC2_hour - Exp_start_hour;
	REC2_sub1_min = time_min_sec(REC2_min, Exp_start_min);
	REC2_sub1_sec = time_min_sec(REC2_sec, Exp_start_sec);
	REC2_sub1_millisec = time_millisec(REC2_millisec, Exp_start_millisec);
	
	REC2_sub1 = (REC2_sub1_hour * 3600) + (REC2_sub1_min * 60) + (REC2_sub1_sec) + (REC2_sub1_millisec / 1000);
	print "reciever 2: " REC2_sub1_hour "\t" REC2_sub1_min "\t" REC2_sub1_sec "\t" REC2_sub1_millisec;
	print "reciever 2 formated time: " REC2_sub1;
	
	SND2_sub1_hour = SND2_hour - Exp_start_hour;
	SND2_sub1_min = time_min_sec(SND2_min, Exp_start_min);
	SND2_sub1_sec = time_min_sec(SND2_sec, Exp_start_sec);
	SND2_sub1_millisec = time_millisec(SND2_millisec, Exp_start_millisec);
	
	SND2_sub1 = (SND2_sub1_hour * 3600) + (SND2_sub1_min * 60) + (SND2_sub1_sec) + (SND2_sub1_millisec / 1000);
	print "sender 2: " SND2_sub1_hour "\t" SND2_sub1_min "\t" SND2_sub1_sec "\t" SND2_sub1_millisec;
	print "sender 2 formated time: " SND2_sub1;
	
	A = ((REC1_sub1 * SND2_sub1) - (REC2_sub1 * SND1_sub1)) / (determinant);
	print "A is: " A;
	
	B_pt1_hour = REC2_hour - REC1_hour;
	B_pt1_min = time_min_sec(REC2_min, REC1_min);
	B_pt1_sec = time_min_sec(REC2_sec, REC1_sec);
	B_pt1_millisec = time_millisec(REC2_millisec, REC1_millisec);
	
	B_sub1 = (B_pt1_hour * 3600) + (B_pt1_min * 60) + (B_pt1_sec) + (B_pt1_millisec / 1000);

	print "calculation of B: " B_pt1_hour "\t" B_pt1_min "\t" B_pt1_sec "\t" B_pt1_millisec;
	print "B_sub1 is " B_sub1;
	
	B = B_sub1 / determinant;
	print "B is: " B;
	

	REC1_hour = REC2_hour;
	REC1_min = REC2_min;
	REC1_sec = REC2_sec;
	REC1_millisec = REC2_millisec;

	SND1_hour = SND2_hour;
	SND1_min = SND2_min;
	SND1_sec = SND2_sec;
	SND1_millisec = SND2_millisec;

	print "-----------------end of one record------------------";


}

END{

	print "end of processing.....";

}
