
#include "wpd.h"

/* defines to simplify searches in files */
#define PRESENT_RATE_STR    "present rate:"
#define REMAINING_CAPACITY_STR    "remaining capacity:"
#define CPU_MHZ_STR "cpu MHz"
#define LAST_FULL_CAPACITY_STR    "last full capacity:"
#define TEMPERATURE_STR "temperature:"
#define AC_STATE_STR "state:"
#define BATTERY_PRESENT_STR "present:"

extern struct _conf_data wpd_conf_data;

static char prebuffer[BUFFER_LEN];

/* open a file and read it into prebuffer */
static BOOL buffer_file( char* name )
{
	FILE* file = fopen( name, "r" );
	if ( file != NULL )
	{
		fread( prebuffer, BUFFER_LEN, 1, file );
		prebuffer[ BUFFER_LEN ] = '\0';
		fclose(file);
		return TRUE;
	}
	fprintf(wpd_conf_data.output_file, "Unable to open %s: %s (%d)\n", name, strerror(errno), errno );
	return FALSE;
}

/* Parse prebuffer to find needle and return the first following integer */
static int buffer_read_int( char* needle )
{
	char* tmp;
	int ret = 0;
	if ( needle != NULL )
	{
		if ( (tmp = strstr( prebuffer, needle )) != NULL )
		{
			tmp = &tmp[ strlen(needle) ];
			while ( (!isdigit( tmp[0] )) && ( tmp[0] != '\0') )
			{
				tmp = &tmp[1];
			}
			sscanf(tmp, "%d", &ret );
		}
	}
	else
		sscanf(prebuffer, "%d", &ret );
	return ret;
}

/* Parse prebuffer to find needle and return the following string */
static void buffer_read_str( char* needle, char* str )
{
	char* tmp;
	if ( needle != NULL )
	{
		if ( (tmp = strstr( prebuffer, needle )) != NULL )
		{
			sscanf(&tmp[ strlen(needle)  ], "%s", str );
		}
	}
	else
		sscanf(prebuffer, "%s", str );
}

/* the following functions reads data from the various ACPI/cpufreq/aus/etc files */
void collect_battery_data( int *present_rate, int *remaining_capacity )
{
	if ( buffer_file( wpd_conf_data.battery_state ) )
	{
		(*present_rate) = buffer_read_int( PRESENT_RATE_STR );
		(*remaining_capacity) = buffer_read_int( REMAINING_CAPACITY_STR );
	}
	else
	{
		(*present_rate) = 0;
		(*remaining_capacity) = 0;
	}
}

int collect_battery_info()
{
	int ret;
	if ( buffer_file( wpd_conf_data.battery_info) )
	{
		ret = buffer_read_int( LAST_FULL_CAPACITY_STR );
	}
	else
	{
		ret = 0;
	}
	return ret;
}

void collect_battery_presence( char* str )
{
	if ( buffer_file( wpd_conf_data.battery_info) )
	{
		buffer_read_str( BATTERY_PRESENT_STR,str );
	}
	else
	{
		strcpy( str, "unk");
	}
}

int collect_cpu_data()
{
	int ret;
	if ( buffer_file( wpd_conf_data.cpu_state ) )
	{
		ret = buffer_read_int( CPU_MHZ_STR );
	}
	else
	{
		ret = 0;
	}
	return ret;
}

void collect_governor_data( char* governor )
{
	if ( buffer_file( wpd_conf_data.governor_file) )
	{
		buffer_read_str( NULL, governor );
	}
	else
	{
		strcpy( governor, "unk");
	}
}

int collect_cpu_temp()
{
	int ret;
	if ( buffer_file( wpd_conf_data.temp_file) )
	{
		ret = buffer_read_int( TEMPERATURE_STR );
	}
	else
	{
		ret = 0;
	}
	return ret;
}

void collect_ac_state( char* str )
{
	if ( buffer_file( wpd_conf_data.ac_state) )
	{
		buffer_read_str( AC_STATE_STR, str );
	}
	else
	{
		strcpy( str, "unk");
	}
}

/* The following function retrieves the full list of cpufreq available frequencies
 * It does not use the buffer_file / buffer_read* facilities */
int collect_freq_data( int* freq, int n )
{
	FILE* file = fopen( wpd_conf_data.freq_file, "r" );
	char tmp[BUFFER_LEN];
	int increase;
	char pos;
	int i, n_freq = 0;
	
	for ( i = 0; i < n; i++ )
		freq[i] = 0;
	
	if ( file == NULL )
		return 0;

	pos = 0;
	while ( ( !feof(file) ) && ( n_freq < n ) )
	{
		fscanf( file, "%s", tmp );
		freq[n_freq] = atoi( tmp );
		increase = 1;
		pos = 0;
		for ( i = 0; i < n_freq; i++ )
		{
			if ( freq[i] == freq[n_freq] )
			{
				increase = 0;
			}
		}
		if ( increase )
			n_freq++;

	}

	fclose(file);
	return n_freq;
}
