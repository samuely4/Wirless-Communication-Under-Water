	   #include <stdio.h>
       #include <stdlib.h>
	   #include <string.h>
	   #include <time.h>
	   #include<unistd.h>
	   #include<sys/timeb.h>
	   #define SIZE 1024

int Randoms(int lower, int upper)
{
 
        int num = (rand() %(upper - lower + 1)) + lower;
        printf("the random number is: %d\n", num);
		return num;
}

static char* get_Filename(){
	
		time_t timer;
        struct tm* tm_info;
        char day[3];
        char month[3];
        char year[5];
		char hour[3];
		char minute[3];
		char second[3];
		char AM_PM[3];
		char *buffer;
        time(&timer);
        tm_info = localtime(&timer);
        strftime(day, 3, "%d", tm_info);
        strftime(month, 3, "%m", tm_info);
        strftime(year, 5, "%Y", tm_info);
		strftime(hour, 3, "%I", tm_info);
		strftime(minute, 3, "%M", tm_info);
		strftime(second, 3, "%S", tm_info); 
		strftime(AM_PM, 3, "%p", tm_info);
		buffer = (char *)malloc(SIZE * sizeof(char));
        printf("%s-%s-%s_%s:%s:%s%s\n", month, day, year,hour, minute, second, AM_PM);
		sprintf(buffer, "%s_%s-%s-%s_%s:%s:%s%s.%s", "log", month, day, year,hour, minute, second, AM_PM,"txt");
		printf("the name of the output file is: %s\n", buffer);
		return buffer;
}

static char* get_timestamp(char* buff){
	struct timeb start;
	if (buff){
		ftime(&start);
		strftime(buff,100, "%H:%M:%S", localtime(&start.time));
		sprintf(buff + 8, ":%03u", start.millitm);
	}
return buff;
}

static char* get_one_time_date(){
	
	time_t current_time = time(NULL);
    struct tm *tm = localtime(&current_time);
    return (asctime(tm));
	
}

int main(int argc, char *argv[]){
	
			
		   printf("\nCurrent Date and Time of this trial is: %s\n", get_one_time_date());
		   
           FILE *stream;
		   FILE *Outstream;
           char *line = NULL;
           size_t len = 0;
           size_t nread;
		   unsigned int Dst;
		   unsigned int TxM;
		   unsigned int PWL; 
		   unsigned int NoM;
		   unsigned int LoM;
		   unsigned int IMD;

           if (argc != 3) {
               fprintf(stderr, "Usage: %s <file>\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           stream = fopen(argv[1], "r");
           if (stream == NULL) {
               perror("fopen");
               exit(EXIT_FAILURE);
           }

           while ((nread = getline(&line, &len, stream)) != -1) {
			   if (line[0] == '#'){
				   continue;
			   }
               printf("Retrieved line of length %zu:\n", nread);
			   char ch = ':';
			   char *array = NULL;
			   char array1[3];
			   array = strrchr(line, ch);
			   int value = atoi(array+1);
			   printf("the value of line retrieved is: %d\n", value);
			   printf("the gotton array after (:) is: %s\n", array+1);
               fwrite(line, nread, 1, stdout);
			   strncpy(array1, line, 3);
			   array1[3] = '\0';
			   printf("\nthe identifying array is: %s\n", array1);
			   
			   if (strcmp(array1, "Dst") == 0){
				   Dst = value;
			   }
			   
			   else if (strcmp(array1, "TxM") == 0){
				   TxM = value;
			   }
			   
			   else if (strcmp(array1, "PWL") == 0){
				   PWL = value;
			   }
			   
			   else if (strcmp(array1, "NoM") == 0){
				   NoM = value;
			   }
			   
			   else if (strcmp(array1, "LoM") == 0){
				   LoM = value;
			   }
			   
			   else if (strcmp(array1, "IMD") == 0){
				   IMD = value;
			   }
			   printf("\n");
			   printf("\n");
			   
           }
		   
		   printf("Dst is: %d\n", Dst);
		   printf("TxM is: %d\n", TxM);
		   printf("PWL is: %d\n", PWL);
		   printf("LoM is: %d\n", LoM);
		   printf("NoM is: %d\n", NoM);
		   printf("IMD is: %d\n", IMD);
		   
		   printf("\n");
		   printf("\n");
		   printf("\n");
		   printf("\n");
		   
		   
           fclose(stream);
		   
		   int count = 0;
		   int size_read = LoM;
		   char ch;
		   char command[] = "$HHTXD";
		   char comma[] = ",";
		   char Dstc[SIZE];
		   char TxMc[SIZE];
		   char PWLc[SIZE];
		   char NoMc[SIZE];
		   char LoMc[SIZE];
		   char IMDc[SIZE];
		   char toHex[SIZE];
		   char toStr[SIZE];
		   char target[SIZE];
		   char COMM[SIZE];
		   char tmp[SIZE];
		   char packet_totalNO[SIZE];
		   char packet_counter_str[SIZE];
		   int len_of_command;
		   int len_of_data;
		   int total_len;
		   int packet_counter = 0;
		   int total_without_COMM;
		 
		   srand(time(0));
		   
		   
		   snprintf(Dstc, sizeof(Dstc), "%u",Dst);
		   snprintf(TxMc, sizeof(TxMc), "%u",TxM);
		   snprintf(NoMc, sizeof(NoMc), "%u",NoM);
		   snprintf(PWLc, sizeof(PWLc), "%u",PWL);
		   snprintf(LoMc, sizeof(LoMc), "%u",LoM);
		   snprintf(IMDc, sizeof(IMDc), "%u",IMD);
		   
		   
		   snprintf(packet_totalNO, sizeof(packet_totalNO), "%05d", NoM);
		   printf("the number of messages string is: %s\n", packet_totalNO);
		   
		   snprintf(packet_counter_str, sizeof(packet_counter_str), "%05d", packet_counter);
		   printf("The packet counter string is: %s\n", packet_counter_str);
		   
		   stream = fopen(argv[2], "r");
		   if (stream == NULL){
			   printf("error opening the file");
			   perror("fopen");
               exit(EXIT_FAILURE);
		   }
		   
		   
		   Outstream = fopen(get_Filename(), "ab+");
		   if (Outstream == NULL){
			   printf("error opening the file");
			   perror("fopen");
               exit(EXIT_FAILURE);
		   }
		   
			snprintf(target, sizeof(target), "%s%s%s%s%s%s%s%s", command, comma, Dstc, comma, TxMc, comma, PWLc, comma);
			len_of_command = strlen(target);
			printf("length of command is: %d\n", len_of_command);
			printf("the targetCommand is: %s\n", target);
		   
		   
		   printf("\n");
		   printf("\n");
		   printf("\n");
		   printf("\n");
		   
		   
		   printf("destination string is: %s\n", Dstc);
		   printf("mode string is: %s\n", TxMc);
		   printf("power level string is: %s\n", PWLc);
		   printf("number of messages string is: %s\n", NoMc);
		   printf("length of messages is: %s\n", LoMc);
		   printf("inter-message delay is: %s\n", IMDc);
		   printf("comma string is: %s\n", comma);
		   printf("command string is: %s\n", command);
		   
		   printf("\n");
		   printf("\n");
		   printf("\n");
		   printf("\n");
		   
		   total_without_COMM = strlen(get_timestamp(tmp)) + strlen(packet_counter_str) + strlen(packet_totalNO);
		   printf("total number of characters not include in the command itself is: %d\n", total_without_COMM);
		   
		   fprintf(Outstream, "%s\n", get_one_time_date());
		   fprintf(Outstream, "the composing command is: %s\n\n\n", target);
		   
		   //this commented part is to read from file line by line (not taking into consideration \n \r)
		   /*int i,j;
		   while ((nread = getline(&line, &len, stream)) != -1) {
			   printf("Retrieved line of length %zu:\n", nread);
			   fwrite(line, nread, 1, stdout);
			   for(i=0,j=0;i<strlen(line);i++,j+=2){
					sprintf((char*)toHex+j,"%02X",line[i]);
				}
				toHex[j] = '\0';
				len_of_data = strlen(toHex);
				total_len = len_of_command + len_of_data;
				printf("the total length of data is: %d\n", len_of_data);
				printf("the hex data is: %s\n", toHex);
				printf("the total size of the command is: %d\n", total_len);
				snprintf(COMM, sizeof(COMM), "%s%s", target, toHex);
				printf("the complete command is: %s\n", COMM);
				printf("\n");
				printf("\n");
				printf("\n");
				fprintf(Outstream, "%s        %s\n", get_timestamp(tmp) ,COMM);
				
				for(i= 0,j= 0; j < (strlen(toHex) / 2); i+= 2,j++){
					toStr[j] = (toHex[i] % 32 + 9) % 25 * 16 + (toHex[i+1] % 32 + 9) % 25;
				}
				toStr[strlen(toHex)/2] = '\0';
				
				fprintf(Outstream, "%s        %s%s\n\n", get_timestamp(tmp), "data recieved is: ", toStr);
				
				int randomNUM = Randoms(1,5);
				sleep(randomNUM);
		   }
		   fclose(stream);
		   fclose(Outstream);
		   printf("\n");*/
		   
		int i, j;
		while(!feof(stream)){
			line = (char *)malloc(SIZE * sizeof(char));
			while(1){
				if (count == size_read || feof(stream)){
					break;
				}
				ch = fgetc(stream);
				if(ch == '\r' || ch == '\n' || ch == EOF){
					continue;
				}
				line[count++] = ch;
				printf("new character is: %c\n", ch);
			}
			line[count] = '\0';
			printf("the count is: %d\n", count);
			printf("the buffer is: %s\n", line);
			for(i=0,j=0;i<strlen(line);i++,j+=2){
				sprintf((char*)toHex+j,"%02X",line[i]);
			}
			toHex[j] = '\0';
			len_of_data = strlen(toHex);
			total_len = len_of_command + len_of_data;
			printf("the total length of data is: %d\n", len_of_data);
			printf("the hex data is: %s\n", toHex);
			printf("the total size of the command is: %d\n", total_len);
			snprintf(COMM, sizeof(COMM), "%s%s", target, toHex);
			printf("the complete command is: %s\n", COMM);
			
			fprintf(Outstream, "%s        %s\n", get_timestamp(tmp) ,COMM);
				
			for(i= 0,j= 0; j < (strlen(toHex) / 2); i+= 2,j++){
				toStr[j] = (toHex[i] % 32 + 9) % 25 * 16 + (toHex[i+1] % 32 + 9) % 25;
			}
			toStr[strlen(toHex)/2] = '\0';
			
			
			packet_counter = packet_counter + 1;
			snprintf(packet_counter_str, sizeof(packet_counter_str), "%05d", packet_counter);
			printf("The packet counter string is: %s\n", packet_counter_str);
			
			printf("\n");
			printf("\n");
			printf("\n");
			
			fprintf(Outstream, "%s        %s%s\n\n", get_timestamp(tmp), "data recieved is: ", toStr);
			sleep(IMD);
			count = 0;
			
			free(line);
		}
		   fclose(stream);
		   fclose(Outstream);
		   printf("\n");
           return 0;
       }
