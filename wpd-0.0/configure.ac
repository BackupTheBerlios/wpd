dnl Process this file with autoconf to produce a configure script.

AC_INIT([wpd])
AC_PREREQ([2.50])

AC_CONFIG_HEADERS([config.h])

WPD_MAJOR=0
WPD_MINOR=7
WPD_SUB=6

DEFCONF_PORT=1978
DEFCONF_NSOCKETS=10
DEFCONF_BATTUPDFREQ=10

WPD_CONF="\"/etc/wpd.conf"\"
#"

AC_SUBST(WPD_MAJOR)
AC_SUBST(WPD_MINOR)
AC_SUBST(WPD_MSUB)
AC_SUBST(WPD_CONF)
AC_SUBST(DEFCONF_PORT)
AC_SUBST(DEFCONF_NSOCKETS)
AC_SUBST(DEFCONF_BATTUPDFREQ)

AC_DEFINE_UNQUOTED(WPD_MAJOR, $WPD_MAJOR, [wpd major version number])
AC_DEFINE_UNQUOTED(WPD_MINOR, $WPD_MINOR, [wpd minor version number])
AC_DEFINE_UNQUOTED(WPD_SUB, $WPD_SUB, [wpd sub version number])
AC_DEFINE_UNQUOTED(WPD_CONF, $WPD_CONF, [wpd configuration file])
AC_DEFINE_UNQUOTED(DEFCONF_PORT, $DEFCONF_PORT, [DEFAULT CONFIGURATION: default port])
AC_DEFINE_UNQUOTED(DEFCONF_NSOCKETS, $DEFCONF_NSOCKETS, [DEFAULT CONFIGURATION: maximum number of sockets])
AC_DEFINE_UNQUOTED(DEFCONF_BATTUPDFREQ, $DEFCONF_BATTUPDFREQ, [DEFAULT CONFIGURATION: battery update frequency])

AC_CONFIG_SRCDIR(src/wpd_main.c)

AM_INIT_AUTOMAKE([wpd], $WPD_MAJOR.$WPD_MINOR.$WPD_SUB)

AC_DISABLE_STATIC

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CHECK_LIB(m, main)
AC_CHECK_PROG([HAS_GKRELLM], [gkrellm2], [TRUE], [FALSE])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h)

dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(gettimeofday select socket strerror)

AC_ARG_ENABLE([wpc], AS_HELP_STRING([--disable-wpc], [Do not create the wpc command line tool])
						 , [enable_wpc=$enableval], [enable_wpc=yes])
AM_CONDITIONAL(ENABLE_WPC, [test x"$enable_wpc" = xyes])

AC_ARG_ENABLE([wpk], AS_HELP_STRING([--disable-wpk], [Do not create the wpk Gkrellm2 plugin])
						 , [enable_wpk=$enableval], [enable_wpk=yes])
AM_CONDITIONAL(ENABLE_WPK, [test x"$enable_wpk" = xyes])

AC_ARG_ENABLE([ebuild], AS_HELP_STRING([--ebable-ebuild], [Create the ebuild for Gentoo])
						 , [enable_ebuild=$enableval], [enable_ebuild=no])

AM_CONDITIONAL(GPD_USR_LIB, [test -e "/usr/lib/gkrellm2/plugins"])
AM_CONDITIONAL(GPD_USR_LOCAL_LIB, [test -e "/usr/locsl/lib/gkrellm2/plugins"])


AC_ARG_ENABLE([xfce4], AS_HELP_STRING([--disable-xfce4], [do not build the xfce4 panel plugin])
						 , [enable_xfce4=$enableval], [enable_xfce4=yes])
AM_CONDITIONAL(ENABLE_XFCE4, [test x"$enable_xfce4" = xyes])

if test x"$enable_xfce4" = xyes 
then

XFCE_PANEL_PLUGIN([XFCE4_PANEL], [4.2.0])

fi


AC_OUTPUT([Makefile doc/Makefile src/Makefile wpk/Makefile wpc/Makefile panel-plugin/Makefile] )

version=$WPD_MAJOR.$WPD_MINOR.$WPD_SUB

if test x"$enable_ebuild" = xyes 
then

	ebuild=wpd-$version.ebuild

	if test -e wpd.ebuild.in
	then

		if test -e $ebuild
		then

			echo > $ebuild

		fi

		cat wpd.ebuild.in >> $ebuild
		echo Created $ebuild

	else

		echo I cannot create $ebuild because wpd.ebuild.in is missing, to create the
		echo Gentoo ebuild you need the full development package, not this release package.

	fi

fi


	echo '# WPD sample configuration' > src/wpd.conf
	echo '# Remember to modify this to suit your system' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '# Any commented option will be "autodetect"ed, if you want to ' >> src/wpd.conf
	echo '# explicitly DISABLE a feature you must specify off in this configuraton file.' >> src/wpd.conf
	echo '# ex: cpu_state=off ' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#output_file=stderr' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#port=1978' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#n_sockets=10' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#battery_state=/proc/acpi/battery/BAT0/state' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#battery_info=/proc/acpi/battery/BAT0/info' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#battery_update_freq=10' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#cpu_state=/proc/cpuinfo' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#governor_file=/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#freq_file=/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#acpi_asus=/proc/acpi/asus' >> src/wpd.conf
	echo ' ' >> src/wpd.conf
	echo '#ac_state=/proc/acpi/ac_adapter/AC0/state' >> src/wpd.conf
	echo ' ' >> src/wpd.conf

	echo Created sample src/wpd.conf

echo WARNING: WPD is capable of autodetecting you configuration, please REMOVE you old wpd.conf
echo from /etc!
echo -

