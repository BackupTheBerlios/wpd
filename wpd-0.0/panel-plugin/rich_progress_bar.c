

#include <gtk/gtk.h>
#include "rich_progress_bar.h"

#define FIX_RATE(aaa) ( ( aaa > 1.0 ) ? 1.0 : ( aaa < 0.0 ) ? 0.0 : aaa )

static void private_rpb_draw( RichProgressBar* rpb )
{
	gchar* str;
	gint cur;
	gchar* iconname;
	
	if ( rpb->is_flashing )
	{
		str = rpb->flash_str;
		cur = rpb->flash_cur;
		iconname = rpb->flash_icon;	
	}
	else
	{
		str = rpb->label_str;
		cur = rpb->cur_value;
		iconname = rpb->icon_str;	
	}
	
	
	if ( rpb->type == RICH_PROGRESS_BAR_LABEL_TYPE )
		gtk_label_set_text( GTK_LABEL(rpb->label), str );
//	else if ( rpb->type == RICH_PROGRESS_BAR_IMAGE_TYPE )
//		gtk_image_set_from_icon_name( GTK_IMAGE(rpb->image), iconname, GTK_ICON_SIZE_SMALL_TOOLBAR );

	gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(rpb->progress_bar), 
						 	FIX_RATE((double)(cur-rpb->min_value) / (double)(rpb->max_value-rpb->min_value)));
}

static gchar* private_rpb_str_init( gchar* str, int len )
{
	if ( str != NULL )
	{
		if ( strlen( str ) < len )
		{
			g_free( str );
			str = NULL;
		}
	}
	
	if ( str == NULL )
		str = (gchar*)malloc( len*sizeof(gchar) );

	return str;
}
	
static gboolean private_rpb_flash( RichProgressBar* rpb )
{
	rpb->is_flashing = FALSE;
	rpb->timeout_id = 0;
	private_rpb_draw(rpb);
	return FALSE;
}









void rich_progress_bar_clear (RichProgressBar *rpb )
{
	if ( rpb->label_str != NULL )
		g_free( rpb->label_str );
	if ( rpb->flash_str != NULL )
		g_free( rpb->flash_str );
	g_free( rpb->progress_bar );
	g_free( rpb->label );
	g_free( rpb->image );
	g_free( rpb->box );
	g_free( rpb );
}





static void rich_progress_bar_class_init (RichProgressBarClass *class)
{
   class->rich_progress_bar = NULL;
}

static void rich_progress_bar_init (RichProgressBar *rpb)
{
	gint i,j;
	  
	rpb->box = gtk_hbox_new( FALSE, 0 );
	rpb->timeout_id = 0;
	rpb->is_flashing = FALSE;
	rpb->label_str = NULL;
	rpb->flash_str = NULL;
	rpb->flash_icon = NULL;
	rpb->icon_str = NULL;
	rpb->orientation = RICH_PROGRESS_BAR_HORIZONTAL_ORIENTATION;
	rpb->valuestyle = RICH_PROGRESS_BAR_NORMAL_VALUESTYLE;

	rpb->min_value = 0;
	rpb->max_value = 0;
	rpb->cur_value = 0;
	rpb->flash_cur = 0;
	
	gtk_container_add (GTK_CONTAINER(rpb), rpb->box);
	gtk_widget_show (rpb->box);

	rpb->progress_bar = gtk_progress_bar_new();
	rpb->label = gtk_label_new("x");
	rpb->image = gtk_image_new();
	
	gtk_box_pack_start( GTK_BOX(rpb->box), GTK_WIDGET( rpb->image), FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(rpb->box), GTK_WIDGET( rpb->label), FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(rpb->box), GTK_WIDGET( rpb->progress_bar), FALSE, TRUE, 0 );
	
	
	gtk_progress_bar_set_orientation( GTK_PROGRESS_BAR(rpb->progress_bar), GTK_PROGRESS_BOTTOM_TO_TOP );
	 
	gtk_widget_set_size_request (rpb->progress_bar, -1, -1);
	gtk_widget_set_size_request (rpb->label, -1, -1);
	
	gtk_widget_show (rpb->progress_bar);
	gtk_widget_show (rpb->label);
   
}

void rich_progress_bar_set_type( RichProgressBar* rpb, RichProgressBarType t )
{
	rpb->type = t;
	switch (t)
	{
	case RICH_PROGRESS_BAR_LABEL_TYPE:
		gtk_widget_show( GTK_WIDGET(rpb->label));
		gtk_widget_hide( GTK_WIDGET(rpb->image));
		break;
	
	case RICH_PROGRESS_BAR_IMAGE_TYPE:
		gtk_widget_hide( GTK_WIDGET(rpb->label));
		gtk_widget_show( GTK_WIDGET(rpb->image));
		break;
	}
}

void rich_progress_bar_set_orientation( RichProgressBar* rpb, RichProgressBarOrientation o )
{
	rpb->orientation = o;
}

void rich_progress_bar_set_valuestyle( RichProgressBar* rpb, RichProgressBarValuestyle v )
{
	rpb->valuestyle = v;
	if ( v == RICH_PROGRESS_BAR_AUTOADJUST_VALUESTYLE )
	{
		rpb->max_value = -1000000;
		rpb->min_value = 1000000;
	}
}

void rich_progress_bar_set_size( RichProgressBar* rpb, gint xsize, gint ysize )
{
	gtk_widget_set_size_request (rpb->progress_bar, xsize*3/10, ysize);
	if ( rpb->type == RICH_PROGRESS_BAR_LABEL_TYPE )
		gtk_widget_set_size_request (rpb->label, xsize*7/10, ysize);
	else if ( rpb->type == RICH_PROGRESS_BAR_IMAGE_TYPE )
		gtk_widget_set_size_request( rpb->image, xsize*7/10, ysize);
}

GtkWidget* rich_progress_bar_new ( )
{
	RichProgressBar* rpb;
	rpb = RICH_PROGRESS_BAR ( gtk_type_new (rich_progress_bar_get_type ()));
	return GTK_WIDGET(rpb);
}

void rich_progress_bar_set_max( RichProgressBar* rpb, gint m )
{
	rpb->max_value = m;
}

void rich_progress_bar_set_min( RichProgressBar* rpb, gint m )
{
	rpb->min_value = m;
}

void rich_progress_bar_set_cur( RichProgressBar* rpb, gint m )
{
	if ( rpb->valuestyle == RICH_PROGRESS_BAR_NORMAL_VALUESTYLE )
	{
		rpb->cur_value = m;
	}
	else if ( rpb->valuestyle == RICH_PROGRESS_BAR_AUTOADJUST_VALUESTYLE )
	{
		if ( m > rpb->max_value )
		{
			rpb->max_value = m;
		}
		if ( m < rpb->min_value )
		{
			rpb->min_value = m;
		}
		rpb->cur_value = m;
	}
	private_rpb_draw(rpb);
}

void rich_progress_bar_set_text( RichProgressBar* rpb, gchar *c )
{
	rpb->label_str = private_rpb_str_init( rpb->label_str, strlen(c) );
	strcpy( rpb->label_str, c );
	private_rpb_draw(rpb);
}

void rich_progress_bar_set_image( RichProgressBar* rpb, GtkIconTheme* t, gchar* c )
{
	GdkPixbuf *pix =  gtk_icon_theme_load_icon( t, c, 24, 0, NULL );
	rpb->icon_str = private_rpb_str_init( rpb->icon_str, strlen(c) );
	gtk_image_set_from_pixbuf(GTK_IMAGE(rpb->image), pix );
	strcpy( rpb->icon_str, c );
	private_rpb_draw(rpb);
}

void rich_progress_bar_flash( RichProgressBar* rpb, gint freq, gchar* c, gint cur )
{
	rpb->flash_str = private_rpb_str_init( rpb->flash_str, strlen(c) );
	strcpy( rpb->flash_str, c );
	rpb->flash_cur = cur;
	rpb->is_flashing = TRUE;
	private_rpb_draw(rpb);
	
	 if ( rpb->timeout_id == 0 )
		rpb->timeout_id = g_timeout_add( 500, (GSourceFunc)private_rpb_flash, rpb );
}

GtkType rich_progress_bar_get_type ()
{
static guint rpb_type = 0;

   if (!rpb_type)
   {
   GtkTypeInfo rpb_info =
      {
			"RichProgressBar",
			sizeof (RichProgressBar),
			sizeof (RichProgressBarClass),
			(GtkClassInitFunc) rich_progress_bar_class_init,
			(GtkObjectInitFunc) rich_progress_bar_init,
			 NULL,
			 NULL
      };

      rpb_type = gtk_type_unique (gtk_vbox_get_type (), &rpb_info);
}

  return rpb_type;
}


