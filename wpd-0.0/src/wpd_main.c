
#include "wpd.h"
#include "wpd_commands.h"

/* Configuration loading */
struct _conf_data wpd_conf_data;
char config_file[BUFFER_LEN] = WPD_CONF;


/* Socket functions */

/* The socket which is used to listen to the incoming commands port */
static int command_socket;
/* This array holds all the permanent command connections */
static int *keep_socket;
static int n_sockets = 0;

/*
 * Setup the listening socket for incoming commands
 */
static void setup_socket()
{
	struct   sockaddr_in sin;
 
	/* get an internet domain socket */
	if ((command_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(wpd_conf_data.output_file, "Unable to create command socket\n");
		exit(1);
	}

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons( wpd_conf_data.port);

	/* bind the socket to the port number */
	if (bind( command_socket, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
		fprintf(wpd_conf_data.output_file, "Unable to bind to %d address\n", wpd_conf_data.port);
		exit(1);
	}

	/* show that we are willing to listen */
	if (listen(command_socket, 5) == -1) {
		fprintf(wpd_conf_data.output_file, "Listen failed\n");
		exit(1);
	}
}

/* 
 * Send an integer down tmp_socket
 */
static void command_socket_output(int tmp_socket,  int n )
{
	ERROR err = NO_ERROR;
	int len = sizeof(int);
	atom_send( tmp_socket, (char*)&len, sizeof(int), &err );
	atom_send( tmp_socket, (char*)&n, len, &err );
}

/*
 * Send an array of integers down tmpsocket
 */
static void command_socket_output_array(int tmp_socket,  int* arr, int n )
{
	ERROR err = NO_ERROR;
	int len = sizeof(int)*n;
	atom_send( tmp_socket, (char*)&len, sizeof(int), &err );
	atom_send( tmp_socket, (char*)arr, len, &err );
}

/*
 * Send a character string down tmp_socket
 */
static void command_socket_output_str(int tmp_socket,  char* str )
{
	ERROR err = NO_ERROR;
	int len = strlen( str )+1;
	atom_send( tmp_socket, (char*)&len, sizeof(int), &err );
	atom_send( tmp_socket, str, len, &err );
}

/*
 * Read a command (an integer value) from tmp_socket
 */
static int read_command(int tmp_socket, ERROR* err)
{
	int len = 0;
	int com = 0;
	if ( atom_read( tmp_socket, (char*)&len, sizeof(int), err ) == -1 )
		return -1;
	if ( len > sizeof(int) )
	{
		fprintf(wpd_conf_data.output_file, "Received malformed command (size %d)\n", len );
		return -1;
	}
	if ( atom_read( tmp_socket, (char*)&com, len, err ) == -1 )
		return -1;
	return com;
}

/*
 * Add a new socket to the list of sockets which are permanent
 */
static void add_socket( int tmp_socket )
{
	if ( n_sockets < wpd_conf_data.n_sockets )
	{
		keep_socket[n_sockets++] = tmp_socket;
		command_socket_output( tmp_socket, 1 );
		fprintf(wpd_conf_data.output_file, "New socket %d placed in keep list\n", 
							 												keep_socket[n_sockets-1]);
	}
	else
		close( tmp_socket );
}

/* 
 * Remove a socket from the list of permanent sockets
 */
static void remove_socket( int s )
{
	int i = 0;
	int pos = -1;

	for ( i = 0; i < n_sockets; i++ )
		if ( keep_socket[i] == s )
			pos = i;

	if ( pos > -1 )
	{
		close( keep_socket[pos] );
	
		for ( i = pos+1; i < n_sockets; i++ );
			keep_socket[i-1] = keep_socket[i];
		n_sockets--;
		fprintf(wpd_conf_data.output_file, "Socket removed\n");
	}
	else
		fprintf(wpd_conf_data.output_file, "Socket to be removed not found\n");
}















/* WPD specific management functions */
static char buffer[BUFFER_LEN];
static int battery_remaining_capacity = 0, battery_present_rate = 0, battery_last_update = 0;

/*
 * Do the real battery state update only if the required timeout had elapsed
 * Do nothing otherwise
 */
static void update_battery_state()
{
	int bpr, brc;
	struct timeval tv;
	if ( gettimeofday( &tv, 0 ) == -1 )
	{
		fprintf(wpd_conf_data.output_file, "Gettimeofday failed: %s (%d)\n", strerror(errno), errno);
		collect_battery_data( &battery_present_rate, &battery_remaining_capacity );
		return;
	}

	if ( tv.tv_sec > battery_last_update + wpd_conf_data.battery_update_freq )
	{
		collect_battery_data( &bpr, &brc );
		battery_present_rate = bpr;
		battery_remaining_capacity = brc;
		battery_last_update = tv.tv_sec;
	}
}

/* 
 * Calculate the remaining time
 */
int calculate_time()
{
	double tmp = 0.0;
	int minuti = 0;
	update_battery_state();
	
	if ( battery_present_rate != 0 )
	{
		tmp = (double)battery_remaining_capacity / (double)battery_present_rate;
		minuti = (int)tmp * 60 + (int)( (tmp-(int)tmp)*60.0 );
	}
	else
		minuti = 0;
	return minuti;
}

/*
 * Process the COMMAND_BATTERY family of commands
 */
void process_battery(int cmm, int tmp_socket)
{
	if ( cmm & BATTERY_PRESENT_RATE )
	{
		update_battery_state();
		command_socket_output( tmp_socket, battery_present_rate );
	}
	else if ( cmm & BATTERY_REMAINING_CAPACITY )
	{
		update_battery_state();
		command_socket_output( tmp_socket, battery_remaining_capacity );
	}
	else if ( cmm & BATTERY_MAX_CAPACITY )
	{
		command_socket_output( tmp_socket, collect_battery_info());
	}
	else if ( cmm & BATTERY_PRESENT )
	{
		collect_battery_presence( buffer );
		command_socket_output_str( tmp_socket, buffer );
	}
	else if ( cmm & BATTERY_AC_STATE )
	{
		collect_ac_state( buffer );
		command_socket_output_str( tmp_socket, buffer );
	}
	else
		command_socket_output( tmp_socket, -1 );
}

/*
 * Process the COMMAND_CPU family of commands
 */
void process_cpu(int cmm, int tmp_socket)
{
	int freq = 0;
	int freq_range[ WPD_MAX_N_FREQUENCIES ];
	if ( cmm & CPU_FREQ )
	{
		command_socket_output(tmp_socket, collect_cpu_data());
	}
	else if ( cmm & CPU_SLOW )
		command_socket_output( tmp_socket,  cpu_speed( CPU_FREQ_SLOW ) );
	else if ( cmm & CPU_FAST )
		command_socket_output( tmp_socket, cpu_speed( CPU_FREQ_FAST ) );
	else if ( cmm & CPU_AUTO )
		command_socket_output( tmp_socket, cpu_speed( CPU_FREQ_AUTO ) );
	else if ( cmm & CPU_GOVERNOR )
	{
		collect_governor_data( buffer );
		command_socket_output_str( tmp_socket, buffer );
	}
	else if ( cmm & CPU_AVAIL_FREQ )
	{
		freq = collect_freq_data( freq_range, WPD_MAX_N_FREQUENCIES );
		command_socket_output_array( tmp_socket, freq_range, freq );
	}
	else if ( cmm & CPU_TEMP )
		command_socket_output( tmp_socket, collect_cpu_temp() );
	else
		command_socket_output( tmp_socket, -1 );
}

/*
 * Process the SYSTEM_COMMAND family of commands
 */
void process_system(int cmm, int tmp_socket)
{
	int minuti = 0;
	char str[BUFFER_LEN];
	if ( cmm & SYSTEM_TIMEREMAINING )
	{
		minuti = calculate_time();	
		command_socket_output( tmp_socket, minuti );
		
	}
	else if ( cmm & SYSTEM_TIME_FORMATTED )
	{
		minuti = calculate_time();	
		sprintf( str, "%2dh:%02dm", minuti/60, minuti%60 );
		command_socket_output_str( tmp_socket, str );
	}
	else if ( cmm & SYSTEM_LCD_ON )
	{
		command_socket_output( tmp_socket, lcd_on() );
	}
	else if ( cmm & SYSTEM_DISPLAY_ON )
	{
		command_socket_output( tmp_socket, display_on() );
	}
	else if ( cmm & SYSTEM_BOTH_ON )
	{
		command_socket_output( tmp_socket, both_on() );
	}
	else
		command_socket_output( tmp_socket, -1 );
}

/*
 * Read the command and dispatch it
 */
void process_command( int tmp_socket )
{
	int cmm = 0;
	ERROR err = NO_ERROR;

	cmm = read_command( tmp_socket, &err );
	
	if ( cmm == -1 )
		remove_socket( tmp_socket );
	else if ( err == NO_ERROR )
	{
		if ( ( ( cmm & MASK ) & COMMAND_BATTERY ) )
			process_battery( cmm & SUBMASK, tmp_socket );
		else if ( ( cmm & MASK ) & COMMAND_CPU )
			process_cpu( cmm & SUBMASK, tmp_socket );
		else if ( ( cmm & MASK ) & COMMAND_SYSTEM )
			process_system( cmm & SUBMASK, tmp_socket );
		else if ( ( cmm & MASK ) & COMMAND_KEEP_CONNECTION )
			add_socket( tmp_socket );
		else
			fprintf(wpd_conf_data.output_file, "Unknown command\n");
	}
	else
		fprintf(wpd_conf_data.output_file, "Error: %d\n", err );

}

/*
 * main loop: "select" a bunch of sockets and listen to them
 * "should" be error tollerant
 */
int wait_event()
{
	struct sockaddr_in pin;
	int    addrlen;
	int    tmp_socket;
	int    max, ret, i;
	fd_set set;
	fd_set except;

	FD_ZERO( &set );
	FD_ZERO( &except );
	FD_SET( command_socket, &set );
	FD_SET( command_socket, &except );
	max = command_socket;
	for ( i = 0; i < n_sockets; i++ )
	{
		if ( keep_socket[i] > max )
			max = keep_socket[i];
		FD_SET( keep_socket[i], &set );
		FD_SET( keep_socket[i], &except );
	}

	ret = select( max+1, &set, NULL, &except, NULL );

	switch (ret)
	{
	case -1:
		/* errno == 4 is "interrupted system call", a signal has been received. */
		if ( errno == 4 )
			break;
		fprintf(wpd_conf_data.output_file, "Listen has failed: %s (%d)\n", strerror(errno), errno);
	
	default:
		for (i = 0; i < n_sockets; i++ )
		{
			if ( FD_ISSET( keep_socket[i], &except ) )
			{
				remove_socket( keep_socket[i] );
			}
			else if ( FD_ISSET( keep_socket[i], &set ) )
			{
				process_command( keep_socket[i] );
			}
		}
		
		if ( FD_ISSET( command_socket, &except ) )
		{
			fprintf(wpd_conf_data.output_file, "Command socket exception detected\n");
		}
		else if ( FD_ISSET( command_socket, &set ) )
		{
        	addrlen = sizeof(pin); 
			tmp_socket = accept( command_socket, (struct sockaddr *)  &pin, &addrlen);
			if ( tmp_socket != -1 )
			{
				process_command( tmp_socket );
			}
			else
				fprintf(wpd_conf_data.output_file, "Error accepting new socket - %s (%d)\n", strerror(errno), errno);
		}
		FD_ZERO( &except );
		FD_ZERO( &set );
		break;
	}
}










/* Signal handling, help and main */

void ctrlc_catcher(int boh )
{
	static int quitting = 0;
	int i;
	signal( SIGTERM, ctrlc_catcher );
	if ( quitting == 0 )
	{
		quitting = 1;
		fprintf(wpd_conf_data.output_file,"WPD exiting due to SIGTERM");
		close( command_socket );
		for ( i = 0; i < n_sockets; i++ )
			close( keep_socket[i] );
		exit(0);
	}
}

void hup_catcher(int boh )
{
	static int configuring = 0;
	signal( SIGHUP, hup_catcher );
	if ( configuring == 0 )
	{
		configuring = 1;
		fprintf(wpd_conf_data.output_file,"Reloading configuration due to SIGHUP");
		LoadConfig();
		configuring = 0;
	}
}

void pipe_catcher(int noh )
{
	signal( SIGPIPE, pipe_catcher );
}

void print_version()
{
	printf("WPD (Willy Power Deamon) - Version %d.%d.%d\n", WPD_MAJOR, WPD_MINOR, WPD_SUB );
}

void print_help()
{
	print_version();
	printf("\n Use: wpd -dDvV \n");
	printf("     d -> debug on\n");
	printf("     c -> use alternate config file\n");
	printf("     D -> no detach\n");
	printf("     v -> print version\n");
	printf("     V -> verbose output (not implemented)\n");
}

int main( int argc, char *argv[] )
{
	BOOL debug = FALSE;
	BOOL fdaemon = TRUE;
	static struct option long_options[7] = {
		{ "help", 0, 0, 'h' },
		{ "version", 0, 0, 'v' },
		{ "debug", 0, 0, 'd' },
		{ "no-daemon", 0, 0, 'D' },
		{ "verbosity", 0, 0, 'V' },
		{ "config", 0, 0, 'c' },
		{ 0, 0, 0, 0 },
						};
	
	int ch,option_index = 0;

	wpd_conf_data.output_file = stderr;

	/* Link to signals */
	signal( SIGTERM, ctrlc_catcher );
	signal( SIGHUP, hup_catcher );
	signal( SIGPIPE, pipe_catcher );

	while ((ch = getopt_long(argc, argv, "hvdDVc:", long_options, 
			         &option_index)) != -1) 
	{
		switch (ch)
		{
		case 'c':
			/* Setup a special config file instead of the default one */
			if ( strlen(optarg) > BUFFER_LEN )
				config_file[0] = '\0';
			else
				strncpy( config_file, optarg, BUFFER_LEN );
			break;
			
		case 'h':
			print_help();
			exit(0);
			break;
		
		case 'v':
			print_version();
			exit(0);
			break;

		case 'd':
			/* Set debug mode */
			debug = TRUE;
			break;

		case 'D':
			/* Unset daemon mode */
			fdaemon = FALSE;
			break;

		default:
			fprintf(wpd_conf_data.output_file, "wrong parameters format");
			print_help();
		}
	}

	LoadConfig();
	keep_socket = (int*)malloc( wpd_conf_data.n_sockets*sizeof(int) );

	if ( debug == TRUE )
	{
			  /* If in debug mode, close the output_file specified in the config
				* file and set it to stderr */
			  if ( ( wpd_conf_data.output_file != stderr ) &&
					 ( wpd_conf_data.output_file != stdout ) )
					fclose( wpd_conf_data.output_file );

			  wpd_conf_data.output_file = stderr;
			  fdaemon = FALSE;
	}
	
	if ( fdaemon == TRUE )	
	{
		daemon( 0, 0 );
	}
	else
		chroot( "/" );

	for ( ch = 0; ch < wpd_conf_data.n_sockets; ch ++ )
		keep_socket[ch] = -1;

	fprintf(wpd_conf_data.output_file, "WPD started...\n");
	
	setup_socket();

	while (1)
		wait_event();

	return 0;		
}


