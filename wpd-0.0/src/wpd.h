
/* various needed headers */
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
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>


/* autoconf generated header file */
#include <config.h>

/* some internal defines */
typedef enum {
		  FALSE,
		  TRUE
} BOOL;
#define BUFFER_LEN 1024
/* mask and submask are used to parse the incoming command */

typedef enum {
		  NO_ERROR,
		  READ_ERROR,
		  WRITE_ERROR,
		  CONNECT_ERROR
} ERROR;


/* Defines used to parse the configuration file */
#define CD_PORT                 1
#define CD_OUTPUT_FILE          2
#define CD_N_SOCKETS            3
#define CD_BATTERY_STATE        4
#define CD_BATTERY_INFO         5
#define CD_CPU_STATE            6
#define CD_BATTERY_UPDATE_FREQ  7
#define CD_GOVERNOR_FILE        8
#define CD_FREQ_FILE            9
#define CD_TEMP_FILE           10
#define CD_ASUS_LCD            11
#define CD_ASUS_DISP           12
#define CD_AC_STATE            13

/* Defines used for the autoconfiguration process */
#define ACPI_BASE          "/proc/acpi"
#define ACPI_BATTERY_BASE  "battery"
#define ACPI_BATTERY_STATE "state"
#define ACPI_BATTERY_INFO  "info"
#define ACPI_THERMAL_BASE  "thermal_zone"
#define ACPI_THERMAL_TEMP  "temperature"
#define ACPI_AC_BASE       "ac_adapter"
#define ACPI_AC_STATE      "state"

#define PROC_BASE    "/proc"
#define CPUINFO_FILE "cpuinfo"

#define CPUFREQ_BASE          "/sys/devices/system/cpu/cpu0/cpufreq"
#define CPUFREQ_GOVERNOR_FILE "scaling_governor"
#define CPUFREQ_FREQ_FILE     "scaling_available_frequencies"

#define ACPI4ASUS_BASE "/proc/acpi/asus"
#define ACPI4ASUS_DISP "disp"
#define ACPI4ASUS_LCD  "lcd"

/* this macro will return 1 if the aaa char string is equal to "off" (case insensitive) */
#define IS_TURNED_OFF(aaa) ( ( (aaa[0] == 'o')||(aaa[0] == 'O') ) && \
									  ( (aaa[1] == 'f')||(aaa[1] == 'F') ) && \
					 				  ( (aaa[2] == 'f')||(aaa[2] == 'F') ) )

/* Commands for the wpd_cpu */
#define CPU_FREQ_SLOW 1
#define CPU_FREQ_FAST 2
#define CPU_FREQ_AUTO 3
#define CPU_FREQ_CONSERVATIVE 4

void collect_battery_data( int*, int* );
int collect_cpu_data( );
int collect_battery_info( );
int collect_freq_data( int*, int );
int collect_cpu_temp();
void collect_battery_presence( char* );
void collect_ac_state( char* );
int cpu_speed(int );
void LoadConfig();
int lcd_on();
int display_on();
int both_on();

int atom_send( int, char*, int, ERROR* );
int atom_read( int, char*, int, ERROR* );


struct _conf_data
{
	int port;
	FILE* output_file;
	int n_sockets;
	char battery_state[ BUFFER_LEN ];
	char battery_info[ BUFFER_LEN ];
	char cpu_state[ BUFFER_LEN ];
	int battery_update_freq;
	char governor_file[ BUFFER_LEN ];
	char freq_file[ BUFFER_LEN ];
	char temp_file[ BUFFER_LEN ];
	char asus_lcd[ BUFFER_LEN ];
	char asus_disp[ BUFFER_LEN ];
	char ac_state[ BUFFER_LEN ];
};

