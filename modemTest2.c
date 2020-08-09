/****************************************************************************
*
*   Copyright (c) 2006 Dave Hylands     <dhylands@gmail.com>
*   Last Edited By: Samuel Youssef
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
*   See README and COPYING for more details.
*
****************************************************************************/
/**
*
*  sertest.c
*
*  PURPOSE:
*
*   This implements a sample program for accessing the serial port.
*
*****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <pthread.h>
#include <signal.h>
#include <getopt.h>
#include <termios.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/timeb.h>
#define SIZE 1024

/* ---- Public Variables ------------------------------------------------- */

int gFd = -1;

int gVal;

/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

struct option gLongOption[] =
{
    // option       A  Flag   V  (has_arg, flag, val)
    // -----------  -  ----  ---
    { "baud",       1, NULL, 'b' },
    { "debug",      0, NULL, 'd' },
    { "help",       0, NULL, 'h' },
    { "port",       1, NULL, 'p' },
    { "verbose",    0, NULL, 'v' },
    { 0 },

};

struct
{
    speed_t     speed;
    unsigned    baudRate;
} gBaudTable[] =
{
    { B50,          50 },
    { B75,          75 },
    { B110,        110 },
    { B134,        134 },
    { B150,        150 },
    { B200,        200 },
    { B300,        300 },
    { B600,        600 },
    { B1200,      1200 },
    { B1800,      1800 },
    { B2400,      2400 },
    { B4800,      4800 },
    { B9600,      9600 },
    { B19200,    19200 },
    { B38400,    38400 },
    { B57600,    57600 },
    { B115200,  115200 },
    { B230400,  230400 }
};

#define ARRAY_LEN(x)    ( sizeof( x ) / sizeof( x[ 0 ]))

int gVerbose = 0;
int gDebug = 0;

int gPortFd = -1;

/* ---- Private Function Prototypes -------------------------------------- */

void *ReadSerialThread( void *param );
void *ReadStdinThread( void *param );
char *StrMaxCpy( char *dst, const char *src, size_t maxLen );
char *StrMaxCat( char *dst, const char *src, size_t maxLen );
static char* get_Filename();
static char* get_timestamp(char* buff);
static char* get_one_time_date();
void Process_setting(FILE* str);
static char* toHex(char* str);
static char* toStr(char* str);
void  Usage( void );


/* ---- Functions -------------------------------------------------------- */

#if defined(__CYGWIN__)
// Cygwin seems to be missing cfmakeraw, so we provide a copy here.
static void cfmakeraw(struct termios *termios_p)
{
    termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    termios_p->c_oflag &= ~OPOST;
    termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    termios_p->c_cflag &= ~(CSIZE|PARENB);
    termios_p->c_cflag |= CS8;
}
#endif /* defined(__CYGWIN__) */

//structure to hold the data extracted from the configuration file
typedef struct{
	int DstID;
	int TxMode;
	int PWLevel;
	int LOMsg;
	int edited_LOMsg;
	int NBMsg;
	int IMDelay;
	int len_of_command;
	int fixed_format_len;
	char Dstc[SIZE];
        char TxMc[SIZE];
        char PWLc[SIZE];
        char NoMc[SIZE];
        char LoMc[SIZE];
        char IMDc[SIZE];
	char target_command[SIZE];
	char fixed_NoMc[SIZE];
}modem;

modem modem1;


typedef struct{
	FILE* inputFile1;
	FILE* inputFile2;
	FILE* outputFile;
}FilePaths;

FilePaths Files;

/***************************************************************************
*
*  main
*
****************************************************************************/

int main( int argc, char **argv )
{
	
    int         sig;
    int         rc;
    int         opt;
    char        devName[ 40 ];
    const char *baudStr = NULL;
    const char *portStr = "ttyUSB0";
    speed_t     baudRate;
    sigset_t    termSig;
    pthread_t   readSerialThreadId;
    pthread_t   readStdinThreadId;
    struct termios stdin_tio;
    struct termios stdin_tio_org;

    struct termios attr;

    Files.inputFile1 = fopen(argv[1], "r");
    if (Files.inputFile1 == NULL){
	   printf("error opening the file");
	   perror("fopen");
	   exit(EXIT_FAILURE);
    }

    Files.inputFile2 = fopen(argv[2], "r");
    if (Files.inputFile2 == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
    }

    Files.outputFile = fopen(get_Filename(), "ab+");
    if (Files.outputFile == NULL){
	   printf("error opening the file");
	   perror("fopen");
	   exit(EXIT_FAILURE);
    }

    fprintf(Files.outputFile, "%s		%s		        %s\n" ,"Send/Receive", "Time Stamp", "Data");


    // Parse the command line options

    while (( opt = getopt_long( argc, argv, "b:dhp:v", gLongOption, NULL )) > 0 )
    {
        switch ( opt )
        {
            case 'b':
            {
                baudStr = optarg;
                break;
            }

            case 'd':
            {
                gDebug = 1;
                break;
            }

            case 'p':
            {
                portStr = optarg;
                break;
            }

            case 'v':
            {
                gVerbose = 1;
                break;
            }
            case '?':
            case 'h':
            {
                Usage();
                return 1;
            }
        }
    }
	


    Process_setting(Files.inputFile1);


    devName[ 0 ] = '\0';
    if ( portStr[ 0 ] != '/' )
    {
        StrMaxCpy( devName, "/dev/", sizeof( devName ));
    }
    StrMaxCat( devName, portStr, sizeof( devName ));


    baudRate = B0;
    if ( baudStr == NULL )
    {
        baudRate = B38400;
    }
    else
    {
        int baudIdx;
        int testBaud = atoi( baudStr );

        for ( baudIdx = 0; baudIdx < ARRAY_LEN( gBaudTable ); baudIdx++ ) 
        {
            if ( gBaudTable[ baudIdx ].baudRate == testBaud )
            {
                baudRate = gBaudTable[ baudIdx ].speed;
                break;
            }
        }

        if ( baudRate == B0 )
        {
            fprintf( stderr, "Unrecognized baud rate: '%s'\n", baudStr );
            exit( 1 );
        }
    }

    // Open the serial port initially using O_NONBLOCK so that we won't block waiting for
    // carrier detect.

    if (( gPortFd = open( devName, O_RDWR | O_EXCL | O_NONBLOCK )) < 0 )
    {
        fprintf( stderr, "Unable to open serial port '%s': %s\n", devName, strerror( errno ));
        exit( 2 );
    }

    // Now that the serial port is open, we can turn off the non-blocking behaviour (for us we want
    // the reads to have blocking semantics).

    fcntl( gPortFd, F_SETFL, fcntl( gPortFd, F_GETFL ) & ~O_NONBLOCK );

    if ( tcgetattr( gPortFd, &attr ) < 0 )
    {
        fprintf( stderr, "Call to tcgetattr failed: %s\n", strerror( errno ));
        exit( 3 );
    }

    cfmakeraw( &attr );

    // CLOCAL - Disable modem control lines
    // CREAD  - Enable Receiver

    attr.c_cflag |= ( CLOCAL | CREAD );

    cfsetispeed( &attr, baudRate );
    cfsetospeed( &attr, baudRate );

    if ( tcsetattr( gPortFd, TCSAFLUSH, &attr ) < 0 )
    {
        fprintf( stderr, "Call to tcsetattr failed: %s\n", strerror( errno ));
        exit( 4 );
    }

    // Put stdin & stdout in unbuffered mode.

    setbuf( stdin, NULL );
    setbuf( stdout, NULL );

    sigemptyset( &termSig );
    sigaddset( &termSig, SIGINT );
    sigaddset( &termSig, SIGTERM );

    pthread_sigmask( SIG_BLOCK, &termSig, NULL );

    // Put stdin in raw mode (i.e. turn off canonical mode). Canonical mode
    // causes the driver to wait for the RETURN character so that line editing
    // can take place. We also want to turn off ECHO.

    if ( tcgetattr( fileno( stdin ), &stdin_tio_org ) < 0 )
    {
        fprintf( stderr, "Unable to retrieve terminal settings: %s\n", strerror( errno ));
        exit( 5 );
    }

    stdin_tio = stdin_tio_org;
    stdin_tio.c_lflag &= ~( ICANON | ECHO );
    stdin_tio.c_cc[VTIME] = 0;
    stdin_tio.c_cc[VMIN] = 1;

    if ( tcsetattr( fileno( stdin ), TCSANOW, &stdin_tio ) < 0 )
    {
        fprintf( stderr, "Unable to update terminal settings: %s\n", strerror( errno ));
        exit( 6 );
    }

    // Kick off the serial port reader thread.

    rc = pthread_create( &readSerialThreadId, NULL, ReadSerialThread, NULL );
    if ( rc != 0 )
    {
        fprintf( stderr, "Error creating ReadSerialThread: %s\n", strerror( rc ));
        exit( 7 );
    }

    // Kick off the stdin reader thread

    rc = pthread_create( &readStdinThreadId, NULL, ReadStdinThread, NULL );
    if ( rc != 0 )
    {
        fprintf( stderr, "Error creating ReadStdinThread: %s\n", strerror( rc ));
        exit( 7 );
    }

    // Wait for a termmination signal

    if (( rc = sigwait( &termSig, &sig )) != 0 )
    {
        fprintf( stderr, "sigwait failed\n" );
    }
    else
    {
        fprintf( stderr, "Exiting...\n" );
    }

    pthread_cancel( readSerialThreadId );
    pthread_cancel( readStdinThreadId );

    // Restore stdin back to the way it was when we started

    if ( tcsetattr( fileno( stdin ), TCSANOW, &stdin_tio_org ) < 0 )
    {
        fprintf( stderr, "Unable to update terminal settings: %s\n", strerror( errno ));
        exit( 6 );
    }

    // Unblock the termination signals so the user can kill us if we hang up
    // waiting for the reader threads to exit.

    pthread_sigmask( SIG_UNBLOCK, &termSig, NULL );

    pthread_join( readSerialThreadId, NULL );
    pthread_join( readStdinThreadId, NULL );

    close( gPortFd );

    if ( gVerbose )
    {
        fprintf( stderr, "Done\n" );
    }

    fclose(Files.inputFile1);
    fclose(Files.inputFile2);
    fclose(Files.outputFile);
    exit( 0 );
    return 0;   // Get rid of warning about not returning anything
}

/***************************************************************************/
/**
*   Thread which processes the incoming serial data.
*/

void *ReadSerialThread( void *param )
{
   
    
    char* line;	
    char* STR;
    char* tmp;
    char  ch;
    int   bytesRead;
    int counter = 0;
    int len_of_data;

    line = (char *)malloc(SIZE * sizeof(char));
    tmp = (char *)malloc(SIZE * sizeof(char));

    
    
    while ( 1 )
    {

	get_timestamp(tmp);
        if (( bytesRead  = read( gPortFd, &ch, 1 )) < 0 )
        {
            fprintf( stderr, "Serial port read failed: %s\n", strerror( errno ));
            exit( 1 );
        }
	
	if(ch == '\n' || ch == '\r'){
		read( gPortFd, &ch, 1 );
		if ( gDebug )
    		{
			
          		fprintf(Files.outputFile, ">>			%s			%s\n", tmp ,line);  
    		}
		len_of_data = strlen(line);
        	printf("the length of data recieved is: %d\n", len_of_data);
		counter = 0;
		free(line);
   		free(tmp);
		line = calloc(1, SIZE);
		tmp = calloc(1, SIZE);
		continue;
	}
	printf("the character read is: %c\n", ch);
	line[counter++] = ch;
    }

   

/*while ( 1 )
    {
        char    ch;
        int     bytesRead;

        if (( bytesRead  = read( gPortFd, &ch, 1 )) < 0 )
        {
            fprintf( stderr, "Serial port read failed: %s\n", strerror( errno ));
            exit( 1 );
        }

        if ( gDebug )
        {
            if (( ch < ' ' ) || ( ch > '~' ))
            {
                fprintf( stderr, "Serial Read: 0x%02x '.'\n", ch );
            }
            else
            {
                fprintf( stderr, "Serial Read: 0x%02x '%c'\n", ch, ch );
            }
        }

        putc( ch, stdout );
    }

    return NULL;*/

  return NULL;

} // ReadSerialThread

/***************************************************************************/
/**
*   Thread which processes the incoming serial from stdin.
*/

void *ReadStdinThread( void *param ) 
{
	char ch;
	char	nl = '\r';
	char* line;
	char* fixed_line_without_data;
	char* fixed_pattern_line;
	char* HEX;
	char* STR;
	char* Cmd_send;
	char* tmp ;
	char* packet_counter_str;
	int count = 0;
	int packet_counter = 0;
	int len_of_Comm;
	
    
	
	line = (char *)malloc(SIZE * sizeof(char));
	fixed_line_without_data = (char *)malloc(SIZE * sizeof(char));
	fixed_pattern_line = (char *)malloc(SIZE * sizeof(char));
	HEX = (char *)malloc(SIZE * sizeof(char));
	Cmd_send = (char *)malloc(SIZE * sizeof(char));
	tmp = (char *)malloc(SIZE * sizeof(char));
	STR = (char *)malloc(SIZE * sizeof(char));
	packet_counter_str = (char *)malloc(SIZE * sizeof(char));

	while(!feof(Files.inputFile2) && packet_counter <= (modem1.NBMsg - 1)){
	    packet_counter = packet_counter + 1;
		sprintf(packet_counter_str, "%05d", packet_counter);
		
		get_timestamp(tmp);


		printf("the time stamp needed is: %s\n",tmp);
		printf("the packet counter string is: %s\n", packet_counter_str);
		printf("the modem fixed packet count is: %s\n", modem1.fixed_NoMc);

		sprintf(fixed_line_without_data, "<%s/%s>_%s_", packet_counter_str, modem1.fixed_NoMc, tmp);
		printf("fixed line without data is %s\n", fixed_line_without_data);
		modem1.fixed_format_len = strlen(fixed_line_without_data);
		modem1.edited_LOMsg = modem1.LOMsg - modem1.fixed_format_len;
		printf("modem fixed format length is: %d\n", modem1.fixed_format_len);
		printf("modem edited format length is: %d\n", modem1.edited_LOMsg);
	
		while(1){
			if (count == modem1.edited_LOMsg || feof(Files.inputFile2) || modem1.edited_LOMsg < 0){
				break;
			}
			ch = fgetc(Files.inputFile2);
			if(ch == '\r' || ch == '\n' || ch == EOF){
				continue;
			}
			line[count++] = ch;
			printf("new character is: %c\n", ch);
		}
		
		if(modem1.edited_LOMsg < 0){
			break;
		}

		line[count] = '\0';
		printf("the count is: %d\n", count);
		printf("the buffer is: %s\n", line);


		sprintf(fixed_pattern_line, "<%s/%s>_%s_%s", packet_counter_str, modem1.fixed_NoMc, tmp, line);
		HEX = toHex(fixed_pattern_line);
		STR = toStr(HEX);
		
		
		printf("the hex data is: %s\n", HEX);
		printf("the string data is: %s\n", STR);
		
		sprintf(Cmd_send, "%s%s", modem1.target_command, HEX);
		printf("the complete command to be sent is: %s\n", Cmd_send);
		
		
		if ( gDebug )
        	{
				fprintf(Files.outputFile, "<<			%s			%s\n", tmp ,Cmd_send);
				fprintf(Files.outputFile, "<<			%s			%s\n", tmp ,STR);
        	}
		
		len_of_Comm = strlen(Cmd_send);
		Cmd_send[len_of_Comm] = '\n';
		
		for (int x=0; x < strlen(Cmd_send); ++x){
			ch = Cmd_send[x];
			if (ch == '\n') {write( gPortFd, &nl, 1 );}
			if ( write( gPortFd, &ch, 1) != 1 )
				{
					fprintf( stderr, "write to serial port failed: %s\n", strerror( errno ));
					break;
				}
		}
		
		
		printf("\n\n\n");
		
	   sleep(modem1.IMDelay);
	   count = 0;
	   free(line);
	   free(fixed_line_without_data);
	   free(fixed_pattern_line);
	   free(HEX);
	   free(Cmd_send);
	   free(tmp);
       free(STR);
	   free(packet_counter_str);
	   if (packet_counter < modem1.NBMsg){
		printf("allocating memory.....\n");
		line = calloc (1, SIZE);
			fixed_line_without_data = calloc (1, SIZE);
			fixed_pattern_line = calloc (1, SIZE);
			HEX = calloc (1, SIZE);
			Cmd_send = calloc (1, SIZE);
			tmp = calloc (1, SIZE);
			STR = calloc (1, SIZE);
			packet_counter_str = calloc (1, SIZE);
	   }
	   
	}
	   /*free(line);
	   free(fixed_line_without_data);
	   free(fixed_pattern_line);
	   free(HEX);
	   free(Cmd_send);
	   free(tmp);
           free(STR);
	   free(packet_counter_str);*/
	   printf("\n");
	
	
	
    /*while ( 1 )
    {
        char    ch;
        int     chInt = fgetc( stdin );
        if ( chInt < 0 )
        {
            fprintf( stderr, "Error reading stdin...\n" );
            break;
        }
        ch = (char)chInt;

        if ( gDebug )
        {
            if (( ch < ' ' ) || ( ch > '~' ))
            {
                fprintf( stderr, "stdin Read: 0x%02x '.'\n", ch );
            }
            else
            {
                fprintf( stderr, "stdin Read: 0x%02x '%c'\n", ch, ch );
            }

        }
		//if debug mode is enabled, write data into the text file. seperate file than before data time colon new line then data
        if ( write( gPortFd, &ch, 1 ) != 1 )// change to calculate the size of the command plus the data
        {
            fprintf( stderr, "write to serial port failed: %s\n", strerror( errno ));
            break;
        }
    }*/

    return NULL;

} // ReadStdinThread

/***************************************************************************/
/**
*  Concatenates source to the destination, but makes sure that the 
*  destination string (including terminating null), doesn't exceed maxLen.
*
*  @param   dst      (mod) String to concatnate onto.
*  @param   src      (in)  String to being added to the end of @a dst.
*  @param   maxLen   (in)  Maximum length that @a dst is allowed to be.
*
*  @return  A pointer to the destination string.
*/

char *StrMaxCat( char *dst, const char *src, size_t maxLen )
{
	size_t	dstLen = strlen( dst );

	if ( dstLen < maxLen )
	{
		StrMaxCpy( &dst[ dstLen ], src, maxLen - dstLen );
	}

	return dst;

} /* StrMaxCat */

/***************************************************************************/
/**
*   Copies the source to the destination, but makes sure that the 
*   destination string (including terminating null), doesn't exceed 
*   maxLen.
*
*   @param  dst     (out) Place to store the string copy.
*   @param  src     (in)  String to copy.
*   @param  maxLen  (in)  Maximum number of characters to copy into @a dst.
*
*   @return A pointer to the destination string.
*/

char *StrMaxCpy( char *dst, const char *src, size_t maxLen )
{
	if ( maxLen < 1 )
	{
		/*
		 * There's no room in the buffer?
		 */

		return "";
	}

	if ( maxLen == 1 )
	{
		/*
		 * There's only room for the terminating null character
		 */

		dst[ 0 ] = '\0';
		return dst;
	}

	/*
	 * The Visual C++ version of strncpy writes to every single character
	 * of the destination buffer, so we use a length one character smaller
	 * and write in our own null (if required).
     *
     * This allows the caller to store a sentinel in the last byte of the
     * buffer to detect overflows (if desired).
	 */

	strncpy( dst, src, maxLen - 1 );
	if (( strlen( src ) + 1 ) >= maxLen )
	{
		/*
		 * The string exactly fits, or probably overflows the buffer.
		 * Write in the terminating null character since strncpy doesn't in
		 * this particular case.
		 *
		 * We don't do this arbitrarily so that the caller can use a sentinel 
		 * in the very end of the buffer to detect buffer overflows.
		 */

		dst[ maxLen - 1 ] = '\0';
	}

	return dst;

} /* StrMaxCpy */


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

void Process_setting(FILE *str){
	
	char *line = NULL;
	char ch = ':';
	char *array = NULL;
	char array1[3];
	char* command = "$HHTXD";
	char* comma = ",";
	size_t len = 0;
    	size_t nread;
	
	while ((nread = getline(&line, &len, str)) != -1) {
	   if (line[0] == '#'){
		   continue;
	   }
	   printf("Retrieved line of length %zu:\n", nread);
	   array = strrchr(line, ch);
	   int value = atoi(array+1);
	   printf("the value of line retrieved is: %d\n", value);
	   printf("the gotton array after (:) is: %s\n", array+1);
	   fwrite(line, nread, 1, stdout);
	   strncpy(array1, line, 3);
	   array1[3] = '\0';
	   printf("\nthe identifying array is: %s\n", array1);
	   
	   if (strcmp(array1, "Dst") == 0){
		   modem1.DstID = value;
	   }
	   
	   else if (strcmp(array1, "TxM") == 0){
		   modem1.TxMode = value;
	   }
	   
	   else if (strcmp(array1, "PWL") == 0){
		   modem1.PWLevel = value;
	   }
	   
	   else if (strcmp(array1, "NoM") == 0){
		   modem1.NBMsg = value;
	   }
	   
	   else if (strcmp(array1, "LoM") == 0){
		   modem1.LOMsg = value;
	   }
	   
	   else if (strcmp(array1, "IMD") == 0){
		   modem1.IMDelay = value;
	   }
   }
   printf("Dst is: %d\n", modem1.DstID);
   printf("TxM is: %d\n", modem1.TxMode);
   printf("PWL is: %d\n", modem1.PWLevel);
   printf("LoM is: %d\n", modem1.LOMsg);
   printf("NoM is: %d\n", modem1.NBMsg);
   printf("IMD is: %d\n", modem1.IMDelay);
   
   snprintf(modem1.Dstc, sizeof(modem1.Dstc), "%u",modem1.DstID);
   snprintf(modem1.TxMc, sizeof(modem1.TxMc), "%u",modem1.TxMode);
   snprintf(modem1.NoMc, sizeof(modem1.NoMc), "%u",modem1.NBMsg);
   snprintf(modem1.PWLc, sizeof(modem1.PWLc), "%u",modem1.PWLevel);
   snprintf(modem1.LoMc, sizeof(modem1.LoMc), "%u",modem1.LOMsg);
   snprintf(modem1.IMDc, sizeof(modem1.IMDc), "%u",modem1.IMDelay);
   
   printf("destination string is: %s\n", modem1.Dstc);
   printf("mode string is: %s\n", modem1.TxMc);
   printf("power level string is: %s\n", modem1.PWLc);
   printf("number of messages string is: %s\n", modem1.NoMc);
   printf("length of messages is: %s\n", modem1.LoMc);
   printf("inter-message delay is: %s\n", modem1.IMDc);
   printf("comma string is: %s\n", comma);
   printf("command string is: %s\n", command);

   snprintf(modem1.target_command, sizeof(modem1.target_command), "%s%s%s%s%s%s%s%s", command, comma, modem1.Dstc, comma, modem1.TxMc, comma, modem1.PWLc, comma);
   modem1.len_of_command = strlen(modem1.target_command);
   printf("length of command is: %d\n", modem1.len_of_command);
   printf("the targetCommand is: %s\n", modem1.target_command);
   
   snprintf(modem1.fixed_NoMc, sizeof(modem1.fixed_NoMc), "%05d", modem1.NBMsg);
   printf("the fixed packet count string is: %s\n", modem1.fixed_NoMc);
   printf("\n\n\n");
	
}


static char* toHex(char* str){
	int i, j;
	char* toHex_arr = malloc(SIZE * sizeof(char));
	for(i=0,j=0;i<strlen(str);i++,j+=2){
		sprintf(toHex_arr+j,"%02X",str[i]);
	}
	toHex_arr[j] = '\0';
	return toHex_arr;
}


static char* toStr(char* str){
	int i, j;
	char* toStr_arr = malloc(SIZE * sizeof(char));
	for(i= 0,j= 0; j < (strlen(str) / 2); i+= 2,j++){
		toStr_arr[j] = (str[i] % 32 + 9) % 25 * 16 + (str[i+1] % 32 + 9) % 25;
	}
	toStr_arr[strlen(str)/2] = '\0';
	return toStr_arr;
}


/***************************************************************************
*
*  Usage
*
****************************************************************************/

void Usage()
{
    fprintf( stderr, "Usage: sertest [option(s)]\n" );
    fprintf( stderr, "  Download a program via serial/i2c\n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "  -b, --baud=baud   Set the baudrate used\n" );
    fprintf( stderr, "  -d, --debug       Turn on debug output\n" );
    fprintf( stderr, "  -h, --help        Display this message\n" );
    fprintf( stderr, "  -p, --port=port   Set the I/O port\n" );
    fprintf( stderr, "  -v, --verbose     Turn on verbose messages\n" );

} // Usage
