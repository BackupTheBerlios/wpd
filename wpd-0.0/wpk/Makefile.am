
if GPD_USR_LIB
GDIR=/usr/lib/gkrellm2/plugins
endif
if GPD_USR_LOCAL_LIB
GDIR=/usr/local/lib/gkrellm2/plugins
endif

gkrellmplugdir = $(GDIR)
#gkrellmplugdir = /usr/lib/gkrellm2/plugins

gkrellmplug_LTLIBRARIES = wpk.la
wpk_la_SOURCES = wpk.c
wpk_la_LDFLAGS = -module -L../src -lwpd
wpk_la_CFLAGS = -Wall `pkg-config --cflags gtk+-2.0`
wpk_la_CCFLAGS = -Wall `pkg-config --cflags gtk+-2.0`
