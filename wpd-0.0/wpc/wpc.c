
#include <config.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "../src/wpd_commands.h"

int e[1];

void print_help()
{
	printf("WPClient - Use: wpc -sfarpF\n");
	printf("                    -s: set cpu clock to minimum\n");
	printf("                    -f: set cpu clock to maximum\n");
	printf("                    -a: set cpu clock to ondemand\n");
	printf("                    -o: set cpu clock to conservative\n");
	printf("                    -r: battery remaining capacity\n");
	printf("                    -p: battery present rate\n");
	printf("                    -F: current cpu frequency\n");
	printf("                    -g: current cpu frenq governor\n");
	printf("                    -A: cpu available frequencies\n");
	printf("                    -C: maximum battery capacity\n");
	printf("                    -t: remainig minutes\n");
	printf("                    -S: time remaining string formatted\n");
	printf("                    -T: get temperature in �C\n");
	printf("                    -l: switch LCD on\n");
	printf("                    -b: switch both lcd and display on\n");
	printf("                    -d: switch external display on\n");
	printf("                    -c: get ac state\n");
	printf("                    -B: check if battery is present\n");
	printf("\n");
}

int main( int argc, char *argv[] )
{
	char str[200];
	int array[100];
	int n;
	static struct option long_options[21] = {
		{ "help", 0, 0, 'h' },
		{ "slow", 0, 0, 's' },
		{ "fast", 0, 0, 'f' },
		{ "auto", 0, 0, 'a' },
		{ "conservative", 0, 0, 'o' },
		{ "remaining-capacity", 0, 0, 'r' },
		{ "present-rate", 0, 0, 'p' },
		{ "frequency", 0, 0, 'F' },
		{ "governor", 0, 0, 'g' },
		{ "availfreq", 0, 0, 'A' },
		{ "maxcapacity", 0, 0, 'C' },
		{ "time", 0, 0, 't' },
		{ "temp", 0, 0, 'T' },
		{ "timestring", 0, 0, 'S' },
		{ "lcdon", 0, 0, 'l' },
		{ "bothon", 0, 0, 'b' },
		{ "displayon", 0, 0, 'd' },
		{ "acstate", 0, 0, 'c' },
		{ "battery", 0, 0, 'B' },
		{ 0, 0, 0, 0 },
						};
	
	int ch,option_index = 0;

	if ( argc < 2 ) 
	{
		print_help();
		exit(0);
	}

	while ((ch = getopt_long(argc, argv, "hsfaorpFgACtTSdlbcB", long_options, 
			         &option_index)) != -1) 
	{
		switch (ch)
		{
		case 's':
			printf("%d\n", send_command( WPD_CMD( MODE_COMMAND, SYS_CPU, CPU_SLOW ), 0 ,-1,e ));
			break;
			
		case 'f':
			printf("%d\n", send_command( WPD_CMD( MODE_COMMAND, SYS_CPU,CPU_FAST),0 ,-1,e ));
			break;
			
		case 'a':
			printf("%d\n", send_command( WPD_CMD( MODE_COMMAND, SYS_CPU, CPU_AUTO),0 ,-1,e ));
			break;
			
		case 'o':
			printf("%d\n", send_command( WPD_CMD( MODE_COMMAND, SYS_CPU, CPU_CONSERVATIVE),0 ,-1,e ));
			break;
			
		case 'r':
			printf("%d\n", send_request( WPD_CMD( MODE_REQUEST, SYS_BATTERY,
													REMAINING_CAPACITY), -1,e));
			break;
			
		case 'p':
			printf("%d\n", send_request( WPD_CMD( MODE_REQUEST, SYS_BATTERY,
													PRESENT_RATE), -1,e));
			break;
			
		case 'F':
			printf("%d\n", send_request( WPD_CMD( MODE_REQUEST, SYS_CPU,FREQUENCY), -1,e ));
			break;
		
		case 'g':
			send_request_str( WPD_CMD( MODE_REQUEST, SYS_CPU,GOVERNOR), str, -1 );
			printf("%s\n", str);
			break;
			
		case 'A':
			n = send_request_array( WPD_CMD( MODE_REQUEST, SYS_CPU,
											AVAIL_FREQUENCIES), array, -1 ) / sizeof(int);
			for ( ; n > 0; n--)
				printf("%d\n", array[n-1] );
			break;

		case 'C':
			printf("%d\n", send_request( WPD_CMD( MODE_REQUEST, SYS_BATTERY,MAX_CAPACITY), -1,e));
			break;
			
		case 't':
			printf("%d\n", send_request( WPD_CMD( MODE_REQUEST,SYS_SYSTEM,TIMEREMAINING), -1,e));
			break;
			
		case 'T':
			printf("%d\n", send_request( WPD_CMD( MODE_REQUEST, SYS_CPU, CPU_TEMP), -1,e));
			break;
			
		case 'S':
			send_request_str( WPD_CMD( MODE_REQUEST,SYS_SYSTEM,TIME_FORMATTED), str, -1 );
			printf("%s\n", str);
			break;
			
		case 'l':
			printf("%d\n", send_command( WPD_CMD(MODE_COMMAND,SYS_SYSTEM,LCD_ON),0,-1,e ));
			break;

		case 'd':
			printf("%d\n", send_command( WPD_CMD(MODE_COMMAND,SYS_SYSTEM,DISPLAY_ON),0,-1,e ));
			break;

		case 'b':
			printf("%d\n", send_command( WPD_CMD(MODE_COMMAND,SYS_SYSTEM,LCD_DISPLAY_ON),0,-1,e ));
			break;
			
		case 'c':
			send_request_str( WPD_CMD(MODE_REQUEST,SYS_BATTERY,AC_STATE), str, -1 );
			printf("%s\n", str);
			break;
			
		case 'B':
			send_request_str( WPD_CMD(MODE_REQUEST,SYS_BATTERY,BATTERY_PRESENT), str, -1 );
			printf("%s\n", str);
			break;

		default:
			print_help();
			exit(0);
		
		}
	}

	exit(0);
}
