
#include "wpd.h"

extern struct _conf_data wpd_conf_data;

void set_governor( char* gov )
{
	FILE* tmp_file = fopen(wpd_conf_data.governor_file, "w" );
	if ( tmp_file == NULL )
	{
		fprintf(wpd_conf_data.output_file, "Unable to set new governor\n");
		return;
	}
	fprintf( tmp_file, gov );
	fclose(tmp_file );
}

int cpu_speed( int speed )
{
	switch (speed)
	{
	case CPU_FREQ_SLOW:
		set_governor( "powersave\n" );
		break;
	case CPU_FREQ_FAST:
		set_governor( "performance\n" );
		break;
	
	case CPU_FREQ_AUTO:
		set_governor( "ondemand\n" );
		break;
	}
	return 1;
}


