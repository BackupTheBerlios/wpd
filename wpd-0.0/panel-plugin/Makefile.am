plugindir = @XFCE4_PANEL_PLUGINSDIR@

plugin_LTLIBRARIES =							\
	libwpdmon.la

libwpdmon_la_LDFLAGS =							\
	-avoid-version							\
	-export-dynamic							\
	-module \
	-L../src -lwpd

libwpdmon_la_SOURCES =							\
	wpdmon.c \
	rich_progress_bar.c \
	rich_progress_bar.h \
	speed-icon.h \
	charge-icon.h \
	rate-icon.h
	
libwpdmon_la_CFLAGS =							\
	@XFCE4_PANEL_CFLAGS@

libwpdmon_la_LIBADD = 							\
	@XFCE4_PANEL_LIBS@

noinst_bin_SCRIPT = convert-icons.sh
