bin_PROGRAMS = wpd
include_HEADERS = wpd_commands.h
lib_LTLIBRARIES = libwpd.la
libwpd_la_SOURCES = wpd_commons.c wpd_commands.h
wpd_SOURCES = wpd_main.c wpd.h wpd_commands.h wpd_collect_data.c wpd_cpu.c wpd_config.c wpd_system.c
wpd_LDADD = $(INTLLIBS) $(ALLOCA) -L. -lwpd
sysconf_DATA = wpd.conf

