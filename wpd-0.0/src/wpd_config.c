
#include "wpd.h"


extern char config_file[BUFFER_LEN];
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
	if ( strncmp( str, "port", 4 ) == 0 )
	{
		(*value) = atoi( &str[pos+1] );
		return CD_PORT;
	}
	else if ( strncmp( str, "n_sockets", 9) == 0 )
	{
		(*value) = atoi( &str[pos+1] );
		return CD_N_SOCKETS;
	}
	else if ( strncmp( str, "battery_update_freq",19  ) == 0 )
	{
		(*value) = atoi( &str[pos+1] );
		return CD_BATTERY_UPDATE_FREQ;
	}
	else if ( strncmp( str, "output_file",11 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_OUTPUT_FILE;
	}
	else if ( strncmp( str, "battery_state",13 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_BATTERY_STATE;
	}
	else if ( strncmp( str, "battery_info",12 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_BATTERY_INFO;
	}
	else if ( strncmp( str, "cpu_state",9 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_CPU_STATE;
	}
	else if ( strncmp( str, "governor_file",13 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_GOVERNOR_FILE;
	}
	else if ( strncmp( str, "freq_file",9 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_FREQ_FILE;
	}
	else if ( strncmp( str, "temp_file",9 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_TEMP_FILE;
	}
	else if ( strncmp( str, "asus_lcd",8 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_ASUS_LCD;
	}
	else if ( strncmp( str, "asus_disp",9 ) == 0 )
	{
		memmove( str, &str[pos+1], len-pos );
		return CD_ASUS_DISP;
	}
	else if ( strncmp( str, "ac_state",8 ) == 0 )
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
		if ( strncmp( "stderr", str,6 ) == 0 )
			wpd_conf_data.output_file = stderr;
		else if ( strncmp( "stdout", str,6 ) == 0 )
			wpd_conf_data.output_file = stdout;
		else
			wpd_conf_data.output_file = fopen( str, "r");
		break;

	case CD_N_SOCKETS:
		wpd_conf_data.n_sockets = value;
		break;

	case CD_BATTERY_STATE:
		strncpy( wpd_conf_data.battery_state, str, BUFFER_LEN  );
		break;

	case CD_BATTERY_INFO:
		strncpy( wpd_conf_data.battery_info, str, BUFFER_LEN );
		break;

	case CD_CPU_STATE:
		strncpy( wpd_conf_data.cpu_state, str, BUFFER_LEN );
		break;

	case CD_BATTERY_UPDATE_FREQ:
		wpd_conf_data.battery_update_freq = value;
		break;

	case CD_GOVERNOR_FILE:
		strncpy( wpd_conf_data.governor_file, str, BUFFER_LEN );
		break;

	case CD_FREQ_FILE:
		strncpy( wpd_conf_data.freq_file, str, BUFFER_LEN );
		break;
				
	case CD_TEMP_FILE:
		strncpy( wpd_conf_data.temp_file, str, BUFFER_LEN );
		break;
		
	case CD_ASUS_LCD:
		strncpy( wpd_conf_data.asus_lcd, str, BUFFER_LEN );
		break;
		
	case CD_ASUS_DISP:
		strncpy( wpd_conf_data.asus_disp, str, BUFFER_LEN );
		break;
		
	case CD_AC_STATE:
		strncpy( wpd_conf_data.ac_state, str, BUFFER_LEN );
		break;
	}
}

/* Parse the configuration file */
static BOOL read_conf()
{
	char str[BUFFER_LEN];
	int value, who;
	FILE* file = fopen( config_file, "r" );
	if ( file == NULL )
	{
		return FALSE;
	}

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
	char name[BUFFER_LEN];

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
				if ( ( strncmp( de->d_name, ".", 2 ) != 0 ) &&
				     ( strncmp( de->d_name, "..", 3) != 0 ) )
				{
					strncpy( name, base_dir, BUFFER_LEN );
					strncat( name, "/", 2);
					strncat( name, de->d_name, NAME_MAX+1 );
	
					/* for each subdir (skip files links and similar things) */
					if ( dir_exist( name ) )
					{
						strncat( name, "/", 2);
						strncat( name, check_file, BUFFER_LEN );

						/* if the dir/subdir/check_file exists.. */
						if ( file_exist( name ) )
						{
							strncpy( conf_str, name, BUFFER_LEN );
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
	strncpy( conf_str, "off", 4 );
	return 0;
}


/* The following functions do the autodetect tricks */
static void detect_battery_state_conf()
{
	/* do not autoconf if the feature has been switched off explicitly */

	if ( IS_TURNED_OFF( wpd_conf_data.battery_state ) )
		return;
	
	
	if ( strncmp( wpd_conf_data.battery_state, "", 1 ) == 0 )
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
	
	
	if ( strncmp( wpd_conf_data.ac_state, "",1 ) == 0 )
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
	
	if ( strncmp( wpd_conf_data.battery_info, "", 1 ) == 0 )
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
	
	if ( strncmp( wpd_conf_data.cpu_state, "", 1 ) == 0 )
	{
		if ( file_exist( PROC_BASE"/"CPUINFO_FILE  ) )
		{
			strncpy( wpd_conf_data.cpu_state, PROC_BASE"/"CPUINFO_FILE, BUFFER_LEN );
		}
		else
		{
			strncpy( wpd_conf_data.cpu_state, "off", 4 );
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
	
	if ( strncmp( wpd_conf_data.governor_file, "",1 ) == 0 )
	{
		if ( file_exist( CPUFREQ_BASE"/"CPUFREQ_GOVERNOR_FILE  ) )
		{
			strncpy( wpd_conf_data.governor_file, CPUFREQ_BASE"/"CPUFREQ_GOVERNOR_FILE, BUFFER_LEN );
		}
		else
		{
			strncpy( wpd_conf_data.governor_file, "off",4 );
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
	
	if ( strncmp( wpd_conf_data.freq_file, "", 1 ) == 0 )
	{
		if ( file_exist( CPUFREQ_BASE"/"CPUFREQ_FREQ_FILE  ) )
		{
			strncpy( wpd_conf_data.freq_file, CPUFREQ_BASE"/"CPUFREQ_FREQ_FILE, BUFFER_LEN );
		}
		else
		{
			strncpy( wpd_conf_data.freq_file, "off",4 );
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
	
	if ( strncmp( wpd_conf_data.temp_file, "",1 ) == 0 )
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
		strncpy( wpd_conf_data.asus_lcd, "off",4 );
		strncpy( wpd_conf_data.asus_disp, "off",4 );
		fprintf(wpd_conf_data.output_file, "ACPI4ASUS not detected\n" );
		return;
	}
	
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.asus_lcd ) == FALSE )
	{
		if ( strncmp( wpd_conf_data.asus_lcd, "", 1 ) == 0 )
		{
			if ( file_exist( ACPI4ASUS_BASE"/"ACPI4ASUS_LCD ) )
			{
				strncpy( wpd_conf_data.asus_lcd, ACPI4ASUS_BASE"/"ACPI4ASUS_LCD, BUFFER_LEN );
			}
			else
			{
				strncpy( wpd_conf_data.asus_lcd, "off", 4 );
				fprintf(wpd_conf_data.output_file, "Unable to detect asus lcd file: %s\n", 
								 							wpd_conf_data.asus_lcd );
			}
		}
	}
	
	/* do not autoconf if the feature has been switched off explicitly */
	if ( IS_TURNED_OFF( wpd_conf_data.asus_disp ) == FALSE )
	{
		if ( strncmp( wpd_conf_data.asus_disp, "", 1 ) == 0 )
		{
			if ( file_exist( ACPI4ASUS_BASE"/"ACPI4ASUS_DISP ) )
			{
				strncpy( wpd_conf_data.asus_disp, ACPI4ASUS_BASE"/"ACPI4ASUS_DISP, BUFFER_LEN );
			}
			else
			{
				strncpy( wpd_conf_data.asus_disp, "off", 4 );
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
	strncpy(wpd_conf_data.battery_state, "", 1);
	strncpy(wpd_conf_data.battery_info, "",1 );
	strncpy(wpd_conf_data.cpu_state, "",1 );
	wpd_conf_data.battery_update_freq = DEFCONF_BATTUPDFREQ;
	strncpy(wpd_conf_data.governor_file, "",1 );
	strncpy(wpd_conf_data.freq_file, "",1 );
	strncpy(wpd_conf_data.temp_file, "",1 );
	wpd_conf_data.output_file = stderr;
	strncpy(wpd_conf_data.asus_lcd, "",1 );
	strncpy(wpd_conf_data.asus_disp, "",1 );
	strncpy(wpd_conf_data.ac_state, "",1 );
	
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

