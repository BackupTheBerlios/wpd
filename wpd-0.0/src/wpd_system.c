
#include "wpd.h"

extern struct _conf_data wpd_conf_data;

static int asus_write_command( char* name, char* v)
{
	FILE* file;
	if ( !IS_TURNED_OFF( name ) )
	{
		file = fopen( name, "w" );
		if ( file == NULL )
			return 0;
		
		fwrite( v, 1, 1, file );
		fclose(file);
		return 1;
	}
	return 0;
}

static int asus_lcd_mode()
{
	return asus_write_command( wpd_conf_data.asus_disp, "3" );
}

static int asus_display_mode()
{
	return asus_write_command( wpd_conf_data.asus_disp, "2" );
}

static int asus_both_mode()
{
	return asus_write_command( wpd_conf_data.asus_disp, "1" );

}





int display_on()
{
	return asus_display_mode();
}

int lcd_on()
{
	return asus_lcd_mode();
}


int both_on()
{
	return asus_both_mode();
}




