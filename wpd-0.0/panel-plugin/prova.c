
#include <gtk/gtk.h>
#include "rich_progress_bar.h"

#include "mhz.h"


static gboolean update( RichProgressBar* ttt)
{
	static gint t = 0;
	rich_progress_bar_set_cur( ttt, t );

	t += 5;

	if ( t > 100 ) t = 0;
	return TRUE;
}

int 
main (int argc, char *argv[])
{
		    GtkWidget *window;
			   GtkWidget *ttt;
				GtkIconTheme* icontheme;
				GdkPixbuf* pixbf;
				  


				  gtk_init (&argc, &argv);

				    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
					   
					   gtk_window_set_title (GTK_WINDOW (window), "Aspect Frame");
						  
						  gtk_signal_connect (GTK_OBJECT (window), "destroy",
														      GTK_SIGNAL_FUNC (gtk_exit), NULL);
						    
						    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

							   /* Create a new Tictactoe widget */
							   ttt = rich_progress_bar_new ();

	pixbf = gdk_pixbuf_new_from_inline( -1, mhz_inline, FALSE, NULL );
	
	icontheme = gtk_icon_theme_get_default();

	gtk_icon_theme_add_builtin_icon( "mhz", 24, pixbf );

	rich_progress_bar_set_image( RICH_PROGRESS_BAR(ttt), icontheme, "mhz" );
	
	rich_progress_bar_set_type( RICH_PROGRESS_BAR(ttt), RICH_PROGRESS_BAR_IMAGE_TYPE );
	rich_progress_bar_set_orientation( RICH_PROGRESS_BAR(ttt), RICH_PROGRESS_BAR_HORIZONTAL_ORIENTATION );
	rich_progress_bar_set_valuestyle( RICH_PROGRESS_BAR(ttt), RICH_PROGRESS_BAR_AUTOADJUST_VALUESTYLE );

	rich_progress_bar_set_max( RICH_PROGRESS_BAR(ttt), 50 );
	rich_progress_bar_set_text(RICH_PROGRESS_BAR(ttt), "P" );
								  gtk_container_add (GTK_CONTAINER (window), ttt);
								    gtk_widget_show (ttt);

		g_timeout_add( 500, (GSourceFunc)update, ttt );
										  gtk_widget_show (window);
										    
										    gtk_main ();
											   
											   return 0;
}
