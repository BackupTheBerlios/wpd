AUTOMAKE_OPTIONS = no-dependencies 1.8

if ENABLE_WPC
DOIT = wpc
endif
if ENABLE_WPK
DOIT2 = wpk
endif
if ENABLE_XFCE4
DOIT3 = panel-plugin
endif
if ENABLE_GWPC
DOIT4 = gwpc
endif

SUBDIRS = src doc $(DOIT) $(DOIT2) $(DOIT3) $(DOIT4)


