
#include "wpd.h"


extern char config_file[1024];
extern struct _conf_data wpd_conf_data;


/* Process a string and localize the next option, also parse the string */
static int read_arg_value( char* str, int* value )
{
	int pos = 0, ind = 0;
	int end = 0;
	int len = strlen(str);
	while ( end == 0 )
	{
		if ( (pos) >= len )
			return -1;
		if ( str[pos] == '=' )
			end = 1;
		else
			pos++;
	}
	if ( pos <= 0 )
		return -1;

	/* terminate the command */
	str[pos] = '\0';
	if ( str[len-1] == '\n' )
		str[len-1] = '\0';

	(*value) = 0;
	if ( strcmp( str, "port" ) == 0 )
	{
		(*value) = atoi( &str[pos+1] );
		return CD_PORT;
	}
	else if ( strcmp( str, "n_sockets") == 0 )
	{
		(*value) = atoi( &str[pos+1] );
		return CD_N_SOCKETS;
	}
	else if ( strcmp( str, "battery_update_freq" ) == 0 )
	{
		(*value) = atoi( &str[pos+1] );
		return CD_BATTERY_UPDATE_FREQ;
	}
	else if ( strcmp( str, "output_file" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_OUTPUT_FILE;
	}
	else if ( strcmp( str, "battery_state" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_BATTERY_STATE;
	}
	else if ( strcmp( str, "battery_info" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_BATTERY_INFO;
	}
	else if ( strcmp( str, "cpu_state" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_CPU_STATE;
	}
	else if ( strcmp( str, "governor_file" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_GOVERNOR_FILE;
	}
	else if ( strcmp( str, "freq_file" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_FREQ_FILE;
	}
	else if ( strcmp( str, "temp_file" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_TEMP_FILE;
	}
	else if ( strcmp( str, "asus_lcd" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_ASUS_LCD;
	}
	else if ( strcmp( str, "asus_disp" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_ASUS_DISP;
	}
	else if ( strcmp( str, "ac_state" ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_AC_STATE;
	}
	return -1;
}

/* Store a string in the wpd_conf_data structure */
static void store_conf( char* str, int value, int who )
{
	switch ( who )
	{
	case CD_PORT:
		wpd_conf_data.port = value;
		break;

	case CD_OUTPUT_FILE:
		if ( strcmp( "stderr", str ) == 0 )
			wpd_conf_data.output_file = stderr;
		else if ( strcmp( "stdout", str ) == 0 )
			wpd_conf_data.output_file = stdout;
		else
			wpd_conf_data.output_file = fopen( str, "r");
		break;

	case CD_N_SOCKETS:
		wpd_conf_data.n_sockets = value;
		break;

	case CD_BATTERY_STATE:
		strcpy( wpd_conf_data.battery_state, str );
		break;

	case CD_BATTERY_INFO:
		strcpy( wpd_conf_data.battery_info, str );
		break;

	case CD_CPU_STATE:
		strcpy( wpd_conf_data.cpu_state, str );
		break;

	case CD_BATTERY_UPDATE_FREQ:
		wpd_conf_data.battery_update_freq = value;
		break;

	case CD_GOVERNOR_FILE:
		strcpy( wpd_conf_data.governor_file, str );
		break;

	case CD_FREQ_FILE:
		strcpy( wpd_conf_data.freq_file, str );
		break;
				
	case CD_TEMP_FILE:
		strcpy( wpd_conf_data.temp_file, str );
		break;
		
	case CD_ASUS_LCD:
		strcpy( wpd_conf_data.asus_lcd, str );
		break;
		
	case CD_ASUS_DISP:
		strcpy( wpd_conf_data.asus_disp, str );
		break;
		
	case CD_AC_STATE:
		strcpy( wpd_conf_data.ac_state, str );
		break;
	}
}

/* Parse the configuration file */
static BOOL read_conf()
{
	char * str;
	int value, who;
	FILE* file = fopen( config_file, "r" );
	if ( file == NULL )
	{
		return FALSE;
	}

	str = (char*)malloc( 1024*sizeof(char) );
	
	fgets( str, 1024, file );

	while ( !feof(file) )
	{
		if ( str[0] != '#' )
		{
			value = -1;
			who = read_arg_value( str, &value );
			store_conf( str, value, who );
		}
		fgets( str, 1024, file );
	}
	free(str);
	fclose(file);
	return TRUE;
}

/* Return TRUE if dir is an existing directory */
static BOOL dir_exist( char * dir )
{
	struct stat tmpstat, *st = &tmpstat;
	
	if ( stat( dir, st ) == -1 )
		return FALSE;
	
	if ( S_ISDIR(st->st_mode) )
	{
		return TRUE;
	}
	return FALSE;
}

/* Return TRUE if file is an existing file */
static BOOL file_exist( char * file )
{
	struct stat tmpstat, *st = &tmpstat;
	
	if ( stat( file, st ) == -1 )
		return FALSE;
	
	if ( S_ISREG(st->st_mode) )
	{
		return TRUE;
	}
	return FALSE;
}

static int conf_dir_file_check( char* base_dir, char* check_file, char* conf_str )
{
	DIR* dir;
	struct dirent* de;
	char name[1024];

	/* check if the base_dir exist and is a directory */
	if ( dir_exist( base_dir ) )
	{
		dir = opendir( base_dir );		
		if ( dir != NULL )
		{
			de = readdir( dir );
			/* process every thing contained in the directory */
			while ( de != NULL )
			{
				/* skip . and .. , we do not want to process the entire filesystem */
				if ( ( strcmp( de->d_name, "." ) != 0 ) &&
				     ( strcmp( de->d_name, "..") != 0 ) )
				{
					strcpy( name, base_dir );
					strcat( name, "/");
					strcat( name, de->d_name );
	
					/* for each subdir (skip files links and similar things) */
					if ( dir_exist( name ) )
					{
						strcat( name, "/");
						strcat( name, check_file );

						/* if the dir/subdir/check_file exists.. */
						if ( file_exist( name ) )
						{
							strcpy( conf_str, name );
							closedir(dir);
							return 1;
						}
					}
				}
				de = readdir( dir );
			}
			closedir( dir );
		}
	}
	strcpy( conf_str, "off" );
	return 0;
}


/* The following functions do the autodetect tricks */
static void detect_battery_state_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */

	if ( IS_TURNED_OFF( wpd_conf_data.battery_state ) )
		return;
	
	
	if ( strcmp( wpd_conf_data.battery_state, "" ) == 0 )
	{
		if ( conf_dir_file_check( ACPI_BASE"/"ACPI_BATTERY_BASE, 
			   	   			 		ACPI_BATTERY_STATE, 
				   	   				(char*)&wpd_conf_data.battery_state ) == 0 )
		{
			fprintf(wpd_conf_data.output_file, "Unable to detect battery state: %s\n", 
								 							wpd_conf_data.battery_state );
		}
	}
}

static void detect_ac_state_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */

	if ( IS_TURNED_OFF( wpd_conf_data.ac_state ) )
		return;
	
	
	if ( strcmp( wpd_conf_data.ac_state, "" ) == 0 )
	{
		if ( conf_dir_file_check( ACPI_BASE"/"ACPI_AC_BASE, 
			   	   			 		ACPI_AC_STATE, 
				   	   				(char*)&wpd_conf_data.ac_state ) == 0 )
		{
			fprintf(wpd_conf_data.output_file, "Unable to detect ac state: %s\n", 
								 							wpd_conf_data.ac_state );
		}
	}
}

static void detect_battery_info_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.battery_info ) )
		return;
	
	if ( strcmp( wpd_conf_data.battery_info, "" ) == 0 )
	{
		if ( conf_dir_file_check( ACPI_BASE"/"ACPI_BATTERY_BASE, 
			   	   			 		ACPI_BATTERY_INFO, 
				   	   				(char*)&wpd_conf_data.battery_info ) == 0 )
		{
			fprintf(wpd_conf_data.output_file, "Unable to detect battery info: %s\n", 
								 							wpd_conf_data.battery_info );
		}
	}
}

static void detect_cpu_state_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.cpu_state ) )
		return;
	
	if ( strcmp( wpd_conf_data.cpu_state, "" ) == 0 )
	{
		if ( file_exist( PROC_BASE"/"CPUINFO_FILE  ) )
		{
			strcpy( wpd_conf_data.cpu_state, PROC_BASE"/"CPUINFO_FILE );
		}
		else
		{
			strcpy( wpd_conf_data.cpu_state, "off" );
			fprintf(wpd_conf_data.output_file, "Unable to detect cpu state file: %s\n", 
							 							wpd_conf_data.cpu_state );
		}
	}
}

static void detect_governor_file_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.governor_file ) )
		return;
	
	if ( strcmp( wpd_conf_data.governor_file, "" ) == 0 )
	{
		if ( file_exist( CPUFREQ_BASE"/"CPUFREQ_GOVERNOR_FILE  ) )
		{
			strcpy( wpd_conf_data.governor_file, CPUFREQ_BASE"/"CPUFREQ_GOVERNOR_FILE );
		}
		else
		{
			strcpy( wpd_conf_data.governor_file, "off" );
			fprintf(wpd_conf_data.output_file, "Unable to detect governor file: %s\n", 
							 							wpd_conf_data.governor_file );
		}
	}
}

static void detect_freq_file_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.freq_file ) )
		return;
	
	if ( strcmp( wpd_conf_data.freq_file, "" ) == 0 )
	{
		if ( file_exist( CPUFREQ_BASE"/"CPUFREQ_FREQ_FILE  ) )
		{
			strcpy( wpd_conf_data.freq_file, CPUFREQ_BASE"/"CPUFREQ_FREQ_FILE );
		}
		else
		{
			strcpy( wpd_conf_data.freq_file, "off" );
			fprintf(wpd_conf_data.output_file, "Unable to detect freq file: %s\n", 
							 							wpd_conf_data.freq_file );
		}
	}
}

static void detect_temp_file_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.temp_file ) )
		return;
	
	if ( strcmp( wpd_conf_data.temp_file, "" ) == 0 )
	{
		if ( conf_dir_file_check( ACPI_BASE"/"ACPI_THERMAL_BASE, 
			   	 			 		ACPI_THERMAL_TEMP, 
				      				(char*)&wpd_conf_data.temp_file ) == 0 )
		{
			fprintf(wpd_conf_data.output_file, "Unable to detect temp file: %s\n", 
								 							wpd_conf_data.temp_file );
		}
	}
}

static void detect_asus_conf()
{
	if ( !dir_exist( ACPI4ASUS_BASE ) )
	{
		strcpy( wpd_conf_data.asus_lcd, "off" );
		strcpy( wpd_conf_data.asus_disp, "off" );
		fprintf(wpd_conf_data.output_file, "ACPI4ASUS not detected\n" );
		return;
	}
	
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.asus_lcd ) == FALSE )
	{
		if ( strcmp( wpd_conf_data.asus_lcd, "" ) == 0 )
		{
			if ( file_exist( ACPI4ASUS_BASE"/"ACPI4ASUS_LCD ) )
			{
				strcpy( wpd_conf_data.asus_lcd, ACPI4ASUS_BASE"/"ACPI4ASUS_LCD );
			}
			else
			{
				strcpy( wpd_conf_data.asus_lcd, "off" );
				fprintf(wpd_conf_data.output_file, "Unable to detect asus lcd file: %s\n", 
								 							wpd_conf_data.asus_lcd );
			}
		}
	}
	
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.asus_disp ) == FALSE )
	{
		if ( strcmp( wpd_conf_data.asus_disp, "" ) == 0 )
		{
			if ( file_exist( ACPI4ASUS_BASE"/"ACPI4ASUS_DISP ) )
			{
				strcpy( wpd_conf_data.asus_disp, ACPI4ASUS_BASE"/"ACPI4ASUS_DISP );
			}
			else
			{
				strcpy( wpd_conf_data.asus_disp, "off" );
				fprintf(wpd_conf_data.output_file, "Unable to detect asus disp file: %s\n", 
								 							wpd_conf_data.asus_disp );
			}
		}
	}
}

void LoadConfig()
{
	/* Setup the default values */
	wpd_conf_data.port = DEFCONF_PORT;
	wpd_conf_data.n_sockets = DEFCONF_NSOCKETS;
	strcpy(wpd_conf_data.battery_state, "");
	strcpy(wpd_conf_data.battery_info, "");
	strcpy(wpd_conf_data.cpu_state, "");
	wpd_conf_data.battery_update_freq = DEFCONF_BATTUPDFREQ;
	strcpy(wpd_conf_data.governor_file, "");
	strcpy(wpd_conf_data.freq_file, "");
	strcpy(wpd_conf_data.temp_file, "");
	wpd_conf_data.output_file = stderr;
	strcpy(wpd_conf_data.asus_lcd, "");
	strcpy(wpd_conf_data.asus_disp, "");
	strcpy(wpd_conf_data.ac_state, "");
	
	if ( read_conf() == 0 )
	{
		fprintf(stderr, "\nWARNING: Unable to parse configuration file %s\n", config_file );
		fprintf(stderr, "         I will use the default configuration and try to guess what is missing...\n");
		fprintf(stderr, "         If some functionalities are missing please configure manually.\n\n");
	}

	detect_battery_state_conf();
	detect_battery_info_conf();
	detect_cpu_state_conf();
	detect_governor_file_conf();
	detect_freq_file_conf();
	detect_temp_file_conf();
	detect_asus_conf();
	detect_ac_state_conf();

	fprintf(wpd_conf_data.output_file, "The port will be %d\n", wpd_conf_data.port );		  
	fprintf(wpd_conf_data.output_file, "The max num of sockets will be %d\n", wpd_conf_data.n_sockets ); 
	fprintf(wpd_conf_data.output_file, "The update frequency wll be %d\n", wpd_conf_data.
						 																			battery_update_freq); 
	fprintf(wpd_conf_data.output_file, "The battery file will be %s\n", wpd_conf_data.battery_state );
	fprintf(wpd_conf_data.output_file, "The battery info will be %s\n", wpd_conf_data.battery_info );
	fprintf(wpd_conf_data.output_file, "The cpu state will be %s\n", wpd_conf_data.cpu_state );
	fprintf(wpd_conf_data.output_file, "The governor file will be %s\n", wpd_conf_data.governor_file );
	fprintf(wpd_conf_data.output_file, "The freq file will be %s\n", wpd_conf_data.freq_file );
	fprintf(wpd_conf_data.output_file, "The temp file will be %s\n", wpd_conf_data.temp_file );
	fprintf(wpd_conf_data.output_file, "The asus lcd file will be %s\n", wpd_conf_data.asus_lcd );
	fprintf(wpd_conf_data.output_file, "The asus display file will be %s\n", wpd_conf_data.asus_disp );
	fprintf(wpd_conf_data.output_file, "The ac state file will be %s\n", wpd_conf_data.ac_state );

}

