/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
*/
#ifndef __RICH_PROGRESS_BAR_H__
#define __WPDSIDGET_H__


#include <gdk/gdk.h>
#include <gtk/gtkvbox.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RICH_PROGRESS_BAR(obj)          GTK_CHECK_CAST (obj, rich_progress_bar_get_type (), RichProgressBar)
#define RICH_PROGRESS_BAR_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, rich_progress_bar_get_type (), RichProgressBarClass)
#define IS_RICH_PROGRESS_BAR(obj)       GTK_CHECK_TYPE (obj, rich_progress_bar_get_type ())


typedef struct _RichProgressBar RichProgressBar;
typedef struct _RichProgressBarClass  RichProgressBarClass;

typedef enum 
{
	RICH_PROGRESS_BAR_LABEL_TYPE,
	RICH_PROGRESS_BAR_IMAGE_TYPE
} RichProgressBarType;

typedef enum
{
	RICH_PROGRESS_BAR_NORMAL_VALUESTYLE,
	RICH_PROGRESS_BAR_AUTOADJUST_VALUESTYLE
} RichProgressBarValuestyle;

typedef enum
{
	RICH_PROGRESS_BAR_HORIZONTAL_ORIENTATION,
	RICH_PROGRESS_BAR_VERTICAL_ORIENTATION
} RichProgressBarOrientation;

struct _RichProgressBar
{
	GtkVBox vbox;
	
	GtkWidget *box;
	GtkWidget *progress_bar;
	GtkWidget *label;
	GtkWidget *image;

	guint type : 1;
	guint valuestyle : 1;
	guint orientation : 1;

	gint max_value;
	gint min_value;
	
	gint cur_value;

	gchar *label_str;
	gchar *flash_str;
	gchar *icon_str;
	gchar *flash_icon;

	gint flash_cur;
	
	gint timeout_id;
	gboolean is_flashing;
};

struct _RichProgressBarClass
{
	GtkVBoxClass parent_class;

	void (*rich_progress_bar) (RichProgressBar *rpb);
};

GtkType        rich_progress_bar_get_type (void);
GtkWidget*     rich_progress_bar_new ( );
void rich_progress_bar_clear( RichProgressBar* );

void rich_progress_bar_set_max( RichProgressBar*, gint);
void rich_progress_bar_set_min( RichProgressBar*, gint);
void rich_progress_bar_set_cur( RichProgressBar*, gint);
void rich_progress_bar_set_text( RichProgressBar*, gchar* );
void rich_progress_bar_set_image( RichProgressBar*, GtkIconTheme* t, gchar* );
void rich_progress_bar_set_size( RichProgressBar*, gint , gint );
void rich_progress_bar_flash( RichProgressBar*, gint, gchar*, gint );
void rich_progress_bar_set_type( RichProgressBar* , RichProgressBarType  );
void rich_progress_bar_set_orientation( RichProgressBar* , RichProgressBarOrientation  );
void rich_progress_bar_set_valuestyle( RichProgressBar* , RichProgressBarValuestyle  );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RICH_PROGRESS_BAR_H__ */

