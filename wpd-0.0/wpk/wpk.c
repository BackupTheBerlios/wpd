
#include <config.h>

#include <gkrellm2/gkrellm.h>
#include "../src/wpd_commands.h"


#define CONFIG_NAME "wpk_wpd"
#define STYLE_NAME "wpk_wpd"

static GkrellmMonitor* monitor;
static GkrellmTicks* pGK;
static gint style_id;


/* Panels */
static GkrellmPanel* gvrn_panel;
static GkrellmDecal* gvrn_decal;

static GkrellmPanel* freq_panel;
static GkrellmDecal* freq_decal;
static GkrellmKrell* freq_krell;

static GkrellmPanel* rate_panel;
static GkrellmDecal* rate_decal;
static GkrellmKrell* rate_krell;

static GkrellmPanel* rcap_panel;
static GkrellmDecal* rcap_decal;
static GkrellmKrell* rcap_krell;

static GkrellmPanel* time_panel;
static GkrellmDecal* time_decal;

static GkrellmPanel* temp_panel;
static GkrellmDecal* temp_decal;

/* Charts */

static GkrellmChart* freq_chart;
static GkrellmChartconfig* freq_config;
static GkrellmChartdata* freq_data;

static GkrellmChart* rate_chart;
static GkrellmChartconfig* rate_config;
static GkrellmChartdata* rate_data;

static GkrellmChart* rcap_chart;
static GkrellmChartconfig* rcap_config;
static GkrellmChartdata* rcap_data;


/* WPD specific data */

static gint wpd_socket = -1;
static gint battery_max_capacity = 22000;
static gint time_remaining = 0;
static gint max_frequency = 0;
static gint min_rate = 1000000;
static gint max_rate = -1;
static gchar governor[50] = "no governor";
static gint cur_rate = 0;
static gint cur_freq = 0;
static gint cur_rcap = 0;
static gint min_frequency;
static gint frequencies[50];
static gint n_frequencies;
static gint temp = 0;

static void set_rate()
{
		if ( cur_rate > max_rate )
		{
				  max_rate = cur_rate;
		}
		if ( cur_rate < min_rate )
		{
				  min_rate = cur_rate;
		}
}

/* GKrellm plugin functions */

static gint gvrn_panel_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 gvrn_panel->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint freq_panel_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 freq_panel->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint freq_chart_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 freq_chart->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint rate_panel_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 rate_panel->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint rate_chart_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 rate_chart->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint rcap_panel_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 rcap_panel->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint rcap_chart_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 rcap_chart->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint time_panel_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 time_panel->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

static gint temp_panel_ee( GtkWidget* widget, GdkEventExpose* ev )
{
	gdk_draw_pixmap(widget->window,
	  		          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
						 temp_panel->pixmap, ev->area.x, ev->area.y, ev->area.x, ev->area.y,
																         ev->area.width, ev->area.height);
	return FALSE;
}

/* chart range functions */

static void setup_freq_scaling(GkrellmChartconfig *cf)
{
	gkrellm_set_chartconfig_fixed_grids(cf, 3);
	gkrellm_set_chartconfig_grid_resolution(cf, max_frequency/3 );
}

static void setup_rate_scaling(GkrellmChartconfig *cf)
{
	gkrellm_set_chartconfig_fixed_grids(cf, 3);
	gkrellm_set_chartconfig_grid_resolution(cf, (max_rate-min_rate)/3 );
}

static void setup_rcap_scaling(GkrellmChartconfig *cf)
{
	gkrellm_set_chartconfig_fixed_grids(cf, 3);
	gkrellm_set_chartconfig_grid_resolution(cf, battery_max_capacity/3 );
}

/* panel and chart update */

static void update_wpk()
{
	gchar tmp[128];
	
	if ( pGK->second_tick )
	{
		cur_freq = keep_send_command( COMMAND_CPU | CPU_FREQ, &wpd_socket );
		cur_rate = keep_send_command( COMMAND_BATTERY | BATTERY_PRESENT_RATE, &wpd_socket );
		set_rate();
		cur_rcap = keep_send_command( COMMAND_BATTERY | BATTERY_REMAINING_CAPACITY, &wpd_socket );
		time_remaining = keep_send_command( COMMAND_SYSTEM | SYSTEM_TIMEREMAINING, &wpd_socket );
      keep_send_command_str( COMMAND_CPU | CPU_GOVERNOR, governor, &wpd_socket );
		temp = keep_send_command( COMMAND_CPU | CPU_TEMP, &wpd_socket );

		gkrellm_store_chartdata( freq_chart, max_frequency, cur_freq );
		gkrellm_update_krell( freq_panel, freq_krell, cur_freq );

		setup_rate_scaling( rate_config );
		gkrellm_store_chartdata( rate_chart, max_rate, cur_rate );
		gkrellm_set_krell_full_scale( rate_krell, max_rate-min_rate+1,
											   ( (max_rate - min_rate) > 500 ) ? 1 : 10 );
		gkrellm_update_krell( rate_panel, rate_krell, cur_rate-min_rate );

		gkrellm_store_chartdata( rcap_chart, battery_max_capacity, cur_rcap );
		gkrellm_update_krell( rcap_panel, rcap_krell, cur_rcap );
	}
	
	gkrellm_draw_decal_text( gvrn_panel, gvrn_decal, governor, -1 );
	gkrellm_draw_panel_layers( gvrn_panel );

	gkrellm_draw_chartdata( freq_chart );
	gkrellm_draw_chart_to_screen( freq_chart );
	sprintf( tmp, "%dMhz", cur_freq );
	gkrellm_draw_decal_text( freq_panel, freq_decal, tmp, -1 );
	gkrellm_draw_panel_layers( freq_panel );
	
	gkrellm_draw_chartdata( rate_chart );
	gkrellm_draw_chart_to_screen( rate_chart );
	sprintf( tmp, "%dW", cur_rate );
	gkrellm_draw_decal_text( rate_panel, rate_decal, tmp, -1 );
	gkrellm_draw_panel_layers( rate_panel );

	gkrellm_draw_chartdata( rcap_chart );
	gkrellm_draw_chart_to_screen( rcap_chart );
	sprintf( tmp, "%dWh", cur_rcap );
	gkrellm_draw_decal_text( rcap_panel, rcap_decal, tmp, -1 );
	gkrellm_draw_panel_layers( rcap_panel );

	sprintf(tmp, "%2dh:%02dm", time_remaining/ 60, time_remaining % 60 );
	gkrellm_draw_decal_text( time_panel, time_decal, tmp, -1 );
	gkrellm_draw_panel_layers( time_panel );

	sprintf(tmp, "Temp %2d C", temp );
	gkrellm_draw_decal_text( temp_panel, temp_decal, tmp, -1 );
	gkrellm_draw_panel_layers( temp_panel );
}


static void create_wpk( GtkWidget* box, gint first_create )
{
	GkrellmStyle    *style;
	GkrellmTextstyle* ts;
	GkrellmPiximage* rate_krell_image;
	GkrellmPiximage* freq_krell_image;
	GkrellmPiximage* rcap_krell_image;
	int i;
	
	if ( first_create )
	{
		gvrn_panel = gkrellm_panel_new0();
		
		rate_chart = gkrellm_chart_new0();
		rate_chart->panel = gkrellm_panel_new0();

		freq_chart = gkrellm_chart_new0();
		freq_chart->panel = gkrellm_panel_new0();
		
		rcap_chart = gkrellm_chart_new0();
		rcap_chart->panel = gkrellm_panel_new0();
		
		time_panel = gkrellm_panel_new0();
		
		temp_panel = gkrellm_panel_new0();
	
		cur_rate = keep_send_command( COMMAND_BATTERY | BATTERY_PRESENT_RATE, &wpd_socket );
		battery_max_capacity = keep_send_command( COMMAND_BATTERY | BATTERY_MAX_CAPACITY, &wpd_socket );

		n_frequencies = keep_send_command_array( COMMAND_CPU | CPU_AVAIL_FREQ, frequencies, &wpd_socket ) / 4;
		max_frequency = -1;
		min_frequency = 10000;
		for (i=0; i < n_frequencies;i++ )
		{
			frequencies[i] = frequencies[i]/1000;
			if ( frequencies[i] > max_frequency )
				max_frequency = frequencies[i];
			if ( frequencies[i] < min_frequency )
				min_frequency = frequencies[i];
		}
	
		set_rate();
	}

	freq_panel = freq_chart->panel;
	rate_panel = rate_chart->panel;
	rcap_panel = rcap_chart->panel;
		
	style = gkrellm_meter_style( style_id );
	ts = gkrellm_meter_textstyle( style_id );


	gvrn_decal = gkrellm_create_decal_text( gvrn_panel, "Ay", ts, style, -1, -1, -1 );
	gkrellm_panel_configure( gvrn_panel, NULL, style );
	gkrellm_panel_create( box, monitor, gvrn_panel );


	gkrellm_set_chart_height_default( freq_chart, 20 );
	gkrellm_chart_create( box, monitor, freq_chart, &freq_config );
	freq_data = gkrellm_add_default_chartdata( freq_chart, _("Freq") );
	gkrellm_set_chartdata_flags( freq_data, CHARTDATA_IMPULSE );
	gkrellm_monotonic_chartdata( freq_data, FALSE );
	gkrellm_set_chartconfig_auto_grid_resolution( freq_config, FALSE);
	gkrellm_chartconfig_fixed_grids_connect( freq_config, setup_freq_scaling, NULL);
//	gkrellm_set_chart_height_default( freq_chart, 20 );
	setup_freq_scaling(freq_config);
									
	
	freq_krell_image = gkrellm_krell_meter_piximage( style_id );
	freq_krell = gkrellm_create_krell( freq_panel, freq_krell_image, style );
	gkrellm_monotonic_krell_values( freq_krell, FALSE );
	gkrellm_set_krell_full_scale( freq_krell, max_frequency, 1);
	freq_decal = gkrellm_create_decal_text( freq_panel, "Ay", ts, style, -1, -1, -1 );
	gkrellm_panel_configure( freq_panel, NULL, style );
	gkrellm_panel_create( box, monitor, freq_panel );
	

	gkrellm_alloc_chartdata( freq_chart );


	gkrellm_set_chart_height_default( rate_chart, 20 );
	gkrellm_chart_create( box, monitor, rate_chart, &rate_config );
	rate_data = gkrellm_add_default_chartdata( rate_chart, _("Rate") );
	gkrellm_set_chartdata_flags( rate_data, CHARTDATA_IMPULSE );
	gkrellm_monotonic_chartdata( rate_data, FALSE );
	gkrellm_set_chartconfig_auto_grid_resolution( rate_config, FALSE);
	gkrellm_chartconfig_fixed_grids_connect( rate_config, setup_rate_scaling, NULL);
	setup_freq_scaling(rate_config);
									
	
	rate_krell_image = gkrellm_krell_meter_piximage( style_id );
	rate_krell = gkrellm_create_krell( rate_panel, rate_krell_image, style );
	gkrellm_monotonic_krell_values( rate_krell, FALSE );
	gkrellm_set_krell_full_scale( rate_krell, max_rate, 1);
	rate_decal = gkrellm_create_decal_text( rate_panel, "Ay", ts, style, -1, -1, -1 );
	gkrellm_panel_configure( rate_panel, NULL, style );
	gkrellm_panel_create( box, monitor, rate_panel );
	

	gkrellm_alloc_chartdata( rate_chart );

	gkrellm_set_chart_height_default( rcap_chart, 20 );
	gkrellm_chart_create( box, monitor, rcap_chart, &rcap_config );
	rcap_data = gkrellm_add_default_chartdata( rcap_chart, _("Rcap") );
	gkrellm_set_chartdata_flags( rcap_data, CHARTDATA_IMPULSE );
	gkrellm_monotonic_chartdata( rcap_data, FALSE );
	gkrellm_set_chartconfig_auto_grid_resolution( rcap_config, FALSE);
	gkrellm_chartconfig_fixed_grids_connect( rcap_config, setup_rcap_scaling, NULL);
	setup_freq_scaling(rcap_config);
									
	
	rcap_krell_image = gkrellm_krell_meter_piximage( style_id );
	rcap_krell = gkrellm_create_krell( rcap_panel, rcap_krell_image, style );
	gkrellm_monotonic_krell_values( rcap_krell, FALSE );
	gkrellm_set_krell_full_scale( rcap_krell, battery_max_capacity, 1);
	rcap_decal = gkrellm_create_decal_text( rcap_panel, "Ay", ts, style, -1, -1, -1 );
	gkrellm_panel_configure( rcap_panel, NULL, style );
	gkrellm_panel_create( box, monitor, rcap_panel );
	

	gkrellm_alloc_chartdata( rcap_chart );


	time_decal = gkrellm_create_decal_text( time_panel, "Ay", ts, style, -1, -1, -1 );
	gkrellm_panel_configure( time_panel, NULL, style );
	gkrellm_panel_create( box, monitor, time_panel );
	
	temp_decal = gkrellm_create_decal_text( temp_panel, "Ay", ts, style, -1, -1, -1 );
	gkrellm_panel_configure( temp_panel, NULL, style );
	gkrellm_panel_create( box, monitor, temp_panel );
	
   if (first_create)
	{
		g_signal_connect(G_OBJECT (gvrn_panel->drawing_area), "expose_event",
			              G_CALLBACK(gvrn_panel_ee), NULL);

		g_signal_connect(G_OBJECT (freq_panel->drawing_area), "expose_event",
			              G_CALLBACK(freq_panel_ee), NULL);
		g_signal_connect(G_OBJECT(freq_chart->drawing_area), "expose_event", 
							  G_CALLBACK(freq_chart_ee), NULL);

		g_signal_connect(G_OBJECT (rate_panel->drawing_area), "expose_event",
			              G_CALLBACK(rate_panel_ee), NULL);
		g_signal_connect(G_OBJECT(rate_chart->drawing_area), "expose_event", 
							  G_CALLBACK(rate_chart_ee), NULL);
				
		g_signal_connect(G_OBJECT (rcap_panel->drawing_area), "expose_event",
			              G_CALLBACK(rcap_panel_ee), NULL);
		g_signal_connect(G_OBJECT(rcap_chart->drawing_area), "expose_event", 
							  G_CALLBACK(rcap_chart_ee), NULL);
				
		g_signal_connect(G_OBJECT (time_panel->drawing_area), "expose_event",
			              G_CALLBACK(time_panel_ee), NULL);

		g_signal_connect(G_OBJECT (temp_panel->drawing_area), "expose_event",
			              G_CALLBACK(temp_panel_ee), NULL);
	}
	
		  
}


static GkrellmMonitor   wpk_mon  =
{
	CONFIG_NAME,        /* Title for config clist.   */
	0,                  /* Id,  0 if a plugin       */
	create_wpk,      /* The create function      */
	update_wpk,      /* The update function      */
	NULL,               /* The config tab create function   */
	NULL,               /* Apply the config function        */

	NULL,               /* Save user config         */
	NULL,               /* Load user config         */
	NULL,               /* config keyword        */

	NULL,               /* Undefined 2  */
	NULL,               /* Undefined 1  */
	NULL,               /* private   */

	MON_MAIL,           /* Insert plugin before this monitor       */
 
	NULL,               /* Handle if a plugin, filled in by GKrellM     */
	NULL                /* path if a plugin, filled in by GKrellM       */
};


GkrellmMonitor* gkrellm_init_plugin( void )
{
	pGK = gkrellm_ticks();

	//style_id = gkrellm_add_chart_style( &wpk_mon, STYLE_NAME );
	style_id = gkrellm_add_meter_style( &wpk_mon, STYLE_NAME );
	
	monitor = &wpk_mon;
	return &wpk_mon;
}


