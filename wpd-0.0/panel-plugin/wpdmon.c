
/***************************************************************************
 *   Copyright (C) 2005 by Willy Gardiol                                   *
 *   willy@gardiol.org                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Include GTK stuff */
#include <gtk/gtk.h>
/* Include the rich progress bar widget */
#include "rich_progress_bar.h"
/* Include the XFCE4 stuff */
#include <panel/global.h>
#include <panel/controls.h>
#include <panel/icons.h>
#include <panel/plugins.h>
/* Include WPD stuff */
#include "../src/wpd_commands.h"

#include "speed-icon.h"
#include "charge-icon.h"
#include "rate-icon.h"

/* Instruct the timer to stop when we are quitting */
static gboolean QUIT = FALSE;

/* This is the wpd connection socket */
gint wpd_socket;

/* update freq */
#define UPDATE_FREQ 1000
	
/* The WPD plugin structure */
typedef struct
{
	/* The eventbox is needed by XFCE4 */
	GtkWidget *eventbox;
	/* The plugin meters and labels are stored in a box */
	GtkWidget* box;

	/* Tooltips are used to show more verbose output */
	GtkTooltips* tooltips;
	gchar tooltip_str[1024];
	
	/* Timeout is used to callback and update the plugin meters */
	gint timeout_id;
	gint timeout_value;

	/* These are the meters */
	RichProgressBar* freq;
	RichProgressBar* rate;
	RichProgressBar* rcap;
	GtkLabel* temp;
	GtkLabel* time;

	/* Some data which we want to keep */
	guint battery_max_capacity;

	/* List of available CPU frequencies */
	gint n_frequencies;
	gint frequencies[WPD_MAX_N_FREQUENCIES];
	gint max_frequency;
	gint min_frequency;

} t_wpd;



/*
 * GTK events and signals
 */


/* 
 * Unused so far
 */
static gboolean on_button_press_event_cb (GtkWidget * widget, GdkEventButton * event, Control * control)
{
    if (event->button == 1)
    {
    }

    return FALSE;
}


/* 
 * Plugin internal methods
 */

/*
 * Read the configuration parameters and reload the basic values
 */
static void reset( t_wpd* wpd )
{
	int i;
	wpd->n_frequencies = 0;
	wpd->n_frequencies = keep_send_command_array( COMMAND_CPU | CPU_AVAIL_FREQ, wpd->frequencies, &wpd_socket ) / sizeof(int);
	wpd->battery_max_capacity = keep_send_command( COMMAND_BATTERY | BATTERY_MAX_CAPACITY, &wpd_socket );

	printf("nf is %d\n", wpd->n_frequencies );
	if ( wpd->n_frequencies < 0 )
	{
		wpd->n_frequencies = 0;
	}
	
	wpd->max_frequency = -1;
	wpd->min_frequency = 10000;
	for (i=0; i < wpd->n_frequencies;i++ )
	{
		wpd->frequencies[i] = wpd->frequencies[i]/1000;
		if ( wpd->frequencies[i] > wpd->max_frequency )
			wpd->max_frequency = wpd->frequencies[i];
		if ( wpd->frequencies[i] < wpd->min_frequency )
			wpd->min_frequency = wpd->frequencies[i];
	}
	
	rich_progress_bar_set_max( wpd->freq, wpd->max_frequency );
	rich_progress_bar_set_min( wpd->freq, wpd->min_frequency );
	
	rich_progress_bar_set_max( wpd->rcap, wpd->battery_max_capacity );
	rich_progress_bar_set_min( wpd->rcap, 0 );
	
	gtk_label_set_text( wpd->time, " 0h:00m" );
	gtk_label_set_text( wpd->temp, " 0 C" );
}

/*
 * Cyclic plugin update
 */
static gboolean update( t_wpd* wpd)
{
	static gboolean flash = 0;
	gchar governor[256];
	gchar time[256];
	gint cur_freq, cur_rate, cur_rcap;
	gint cur_temp;
	
	/* Get out of here and stop the timer if we are quitting */
	if ( QUIT )
		return FALSE;

	/* If any basic value is unset, try to reset it */
	if ( ( wpd->max_frequency == -1 ) || 
		  ( wpd->battery_max_capacity == -1 ) )
		reset(wpd);

	governor[0] = '\0';
	time[0] = '\0';

	cur_freq = keep_send_command( COMMAND_CPU | CPU_FREQ, &wpd_socket );
	cur_rate = keep_send_command( COMMAND_BATTERY | BATTERY_PRESENT_RATE, &wpd_socket );
	cur_rcap = keep_send_command( COMMAND_BATTERY | BATTERY_REMAINING_CAPACITY, &wpd_socket );
   keep_send_command_str( COMMAND_CPU | CPU_GOVERNOR, governor, &wpd_socket );
   keep_send_command_str( COMMAND_SYSTEM | SYSTEM_TIME_FORMATTED, time, &wpd_socket );
	cur_temp = keep_send_command( COMMAND_CPU | CPU_TEMP, &wpd_socket );


	rich_progress_bar_set_cur( wpd->rcap, cur_rcap );
	rich_progress_bar_set_cur( wpd->freq, cur_freq );
	rich_progress_bar_set_cur( wpd->rate, cur_rate );

	if ( ( cur_rcap < (double)wpd->battery_max_capacity * 0.05 ) && ( flash == 0 ) )
	{
		rich_progress_bar_flash( wpd->rcap, UPDATE_FREQ/2, "X", wpd->battery_max_capacity );
		flash = 1;
	}
	else
	{
		flash = 0;
	}
	
	sprintf( wpd->tooltip_str, "CPU Freq is %dMhz out of %dMhz maximum (%s)\nDischarge rate is %dW\nBattery charge is %dWh out of %dWh\nCPU temperature is %d C degrees",  cur_freq, wpd->max_frequency, governor,
									cur_rate, cur_rcap, wpd->battery_max_capacity, cur_temp );

	gtk_tooltips_set_tip( GTK_TOOLTIPS(wpd->tooltips), GTK_WIDGET(wpd->eventbox), 
						  								wpd->tooltip_str,
						                        NULL);
	
	sprintf( governor, "%d C", cur_temp );
	gtk_label_set_text( wpd->temp, governor ); 

	gtk_label_set_text( wpd->time, time );
	return TRUE;
}

	
/*
 * Create a new plugin
 */
static t_wpd *
wpd_new_plugin (void)
{
	t_wpd *wpd = g_new (t_wpd, 1);
	GdkPixbuf* pixbf;
	GtkIconTheme *icontheme;
	 
	wpd->timeout_id = 0;
	wpd_socket = -1;
	 
	wpd->battery_max_capacity = -1;
	wpd->max_frequency = -1;
	 
	strcpy( wpd->tooltip_str, "");

	/* todo implement orientation */
   wpd->eventbox = gtk_event_box_new ();
	wpd->box = gtk_hbox_new(FALSE,1);
	 
   gtk_widget_set_name (wpd->eventbox, "xfce_wpd");
	icontheme = gtk_icon_theme_get_default();

	gtk_container_add( GTK_CONTAINER(wpd->eventbox), wpd->box );
	 
	wpd->freq = RICH_PROGRESS_BAR(rich_progress_bar_new());
	pixbf = gdk_pixbuf_new_from_inline( -1, speed_icon, FALSE, NULL );
	gtk_icon_theme_add_builtin_icon( "speed_icon", 24, pixbf );
	rich_progress_bar_set_image( wpd->freq, icontheme, "speed_icon" );
	rich_progress_bar_set_type( wpd->freq, RICH_PROGRESS_BAR_IMAGE_TYPE );
	rich_progress_bar_set_orientation( wpd->freq, RICH_PROGRESS_BAR_HORIZONTAL_ORIENTATION );
	rich_progress_bar_set_valuestyle( wpd->freq, RICH_PROGRESS_BAR_NORMAL_VALUESTYLE );
	
	wpd->rate = RICH_PROGRESS_BAR(rich_progress_bar_new( ));
	pixbf = gdk_pixbuf_new_from_inline( -1, rate_icon, FALSE, NULL );
	gtk_icon_theme_add_builtin_icon( "rate_icon", 24, pixbf );
	rich_progress_bar_set_image( wpd->rate, icontheme, "rate_icon" );
	rich_progress_bar_set_type( wpd->rate, RICH_PROGRESS_BAR_IMAGE_TYPE );
	rich_progress_bar_set_orientation( wpd->rate, RICH_PROGRESS_BAR_HORIZONTAL_ORIENTATION );
	rich_progress_bar_set_valuestyle( wpd->rate, RICH_PROGRESS_BAR_AUTOADJUST_VALUESTYLE );
			  
	wpd->rcap = RICH_PROGRESS_BAR(rich_progress_bar_new( ));
	pixbf = gdk_pixbuf_new_from_inline( -1, charge_icon, FALSE, NULL );
	gtk_icon_theme_add_builtin_icon( "rcap_icon", 24, pixbf );
	rich_progress_bar_set_image( wpd->rcap, icontheme, "rcap_icon" );
	rich_progress_bar_set_type( wpd->rcap, RICH_PROGRESS_BAR_IMAGE_TYPE );
	rich_progress_bar_set_orientation( wpd->rcap, RICH_PROGRESS_BAR_HORIZONTAL_ORIENTATION );
	rich_progress_bar_set_valuestyle( wpd->rcap, RICH_PROGRESS_BAR_NORMAL_VALUESTYLE );

	wpd->temp = GTK_LABEL(gtk_label_new(" 0 C"));
			  
	wpd->time = GTK_LABEL(gtk_label_new(" 0h:00m"));

   gtk_widget_show (wpd->eventbox);
   gtk_widget_show (wpd->box);

	gtk_box_pack_start( GTK_BOX(wpd->box), GTK_WIDGET(wpd->freq), FALSE, FALSE,0 );
   gtk_widget_show (GTK_WIDGET(wpd->freq));
	 
	gtk_box_pack_start( GTK_BOX(wpd->box), GTK_WIDGET(wpd->rate), FALSE, FALSE,0 );
   gtk_widget_show (GTK_WIDGET(wpd->rate));

	gtk_box_pack_start( GTK_BOX(wpd->box), GTK_WIDGET(wpd->rcap), FALSE, FALSE,0 );
   gtk_widget_show (GTK_WIDGET(wpd->rcap));

	gtk_box_pack_start( GTK_BOX(wpd->box), GTK_WIDGET(wpd->temp), FALSE, FALSE,0 );
   gtk_widget_show (GTK_WIDGET(wpd->temp));
	 
	gtk_box_pack_start( GTK_BOX(wpd->box), GTK_WIDGET(wpd->time), FALSE, FALSE,0 );
   gtk_widget_show (GTK_WIDGET(wpd->time));
	 
	wpd->tooltips = gtk_tooltips_new();
	gtk_tooltips_enable( GTK_TOOLTIPS(wpd->tooltips ) );
	 
   g_signal_connect (G_OBJECT (wpd->eventbox), "button-press-event",
		      G_CALLBACK (on_button_press_event_cb), wpd);

	if ( wpd->timeout_id == 0 )
		wpd->timeout_id = g_timeout_add( UPDATE_FREQ, (GSourceFunc)update, wpd );
	
   return wpd;
}

static void
wpd_free (Control * control)
{
    t_wpd *wpd = control->data;

    g_return_if_fail (wpd != NULL);

	 QUIT = TRUE;
	 gtk_widget_destroy( GTK_WIDGET(wpd->temp) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->rate) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->rcap) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->freq) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->time) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->box) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->tooltips) );
	 gtk_widget_destroy( GTK_WIDGET(wpd->eventbox) );
    g_free (wpd);
}

static void
wpd_attach_callback (Control * control, const char *signal,
		       GCallback callback, gpointer data)
{
    t_wpd *wpd = control->data;
    g_signal_connect (wpd->eventbox, signal, callback, data);
}

/* panel preferences */
void
wpd_set_size_plugin (Control * control, int size)
{
	t_wpd* wpd = (t_wpd*)control->data;

	gtk_widget_set_size_request( GTK_WIDGET(wpd->eventbox), icon_size[size]*6+6, icon_size[size] );
	gtk_widget_set_size_request( GTK_WIDGET(wpd->box), icon_size[size]*6, icon_size[size] );
	gtk_widget_set_size_request( GTK_WIDGET(wpd->freq), icon_size[size], icon_size[size] );
	gtk_widget_set_size_request( GTK_WIDGET(wpd->rate), icon_size[size], icon_size[size] );
	gtk_widget_set_size_request( GTK_WIDGET(wpd->rcap), icon_size[size], icon_size[size] );
	gtk_widget_set_size_request( GTK_WIDGET(wpd->temp), icon_size[size], icon_size[size] );
	gtk_widget_set_size_request( GTK_WIDGET(wpd->time), icon_size[size]*2, icon_size[size] );

	gtk_widget_set_size_request( control->base, -1, -1 );

	rich_progress_bar_set_size( wpd->freq, icon_size[size], icon_size[size] );
	rich_progress_bar_set_size( wpd->rate, icon_size[size], icon_size[size] );
	rich_progress_bar_set_size( wpd->rcap, icon_size[size], icon_size[size] );

}

/* wpd options box */
void
wpd_create_options (Control * control, GtkContainer * container,
		      GtkWidget * done)
{

}

/*  Clock panel control
 *  -------------------
*/
gboolean
create_wpd_control (Control * control)
{
    t_wpd *wpd = wpd_new_plugin ();
	 QUIT = FALSE;
    gtk_container_add (GTK_CONTAINER (control->base), wpd->eventbox);

    control->data = (gpointer) wpd;
    control->with_popup = FALSE;

    gtk_widget_set_size_request (control->base, 20, -1);
    wpd_set_size_plugin (control, settings.size);

	 reset(wpd);

    return TRUE;
}

/* calculate position of calendar popup */

G_MODULE_EXPORT void
xfce_control_class_init (ControlClass * cc)
{
    xfce_textdomain (GETTEXT_PACKAGE, LOCALEDIR, "UTF-8");

    cc->name = "WpdPlugin";
    cc->caption = _("WPD Plugin");

    cc->create_control = (CreateControlFunc) create_wpd_control;

    cc->free = wpd_free;
    cc->read_config = NULL;
    cc->write_config = NULL;
    cc->attach_callback = wpd_attach_callback;

    cc->create_options = wpd_create_options;

    cc->set_size = wpd_set_size_plugin;

    control_class_set_unique (cc, TRUE);
}

/* macro defined in plugins.h */
XFCE_PLUGIN_CHECK_INIT
