<html>
<head>
<title>WPD - Willy's Power Daemon</title>
</head>

<body>
<font size="10" color="#FF5500"><CENTER>WPD - WPC - WPK - Xfce4 plugin</CENTER>
</font>
<br>
<br>
WPD is a featureful deamon for Linux whose aim is to allow an easy management of the
cpufreq frequency scaling capabilties of modern Centrino, AMD and similar CPUs and an easy
reading of ACPI data.

WPD is under active and heavy development but it is quite stable, what i need is
more people to test it with different hardware configurations and help in the
development process (specially in the plugin business because i am no good in GUI's).

<br>
<br>
<table border="1"><tr><td>
<font size="6" color="#FF5500"><center>WPD LOGO and ICON CONTEST</center>
</font>
<br>
<br>
The WPD icon contest is open!
<br>
WPD needs YOUR contribution!
<br>
What is needed is:
<br>
<ul>
<li> A great LOGO which fully represent the name WPD and the purpose (power, ACPI, cpufreq management).
<li> A great set of icons:
<ul>
<li> A "frequency" icon which must represent the cpu frequency 
<li> a "rate" icon which must represent the current discharging rate of the battery
<li> a "charge" icon which must represent the battery remaining charge
</ul>
</ul>
<br>
<br>
The LOGO can be of any size but it must be scalable down to 320x80 without loosing too much detail.
<br>
The icons must be sized 48x48 pixel.
<br>
<br>
If you like to partecipate just email your creations to <a href="mailto:wpd@gardiol.org">official WPD mailing list</a>
<br>
<br>
</td></tr></table>
<br>
<br>
WPD is a single daemon which allows the user to:
<ul>
<li> Completely idle daemon (no background activity if not requested)
<li> Autoconfigures itself at startup with available supported capabilities
<li> Support for configuration via /etc file
<li> Uses Linux 2.6 cpufreq
<li> Retrieve ac status
<li> Check if battery is installed
<li> Retrieve battery remaining capacity
<li> Retrieve battery discharge rate
<li> Retrieve battery maximum capacity
<li> Retrieve cpu current frequency
<li> Retrieve cpu current frequency governor
<li> Support for ASUS 4 ACPI extensions
<li> Turn ON/OFF lcd only, external display only, both displays
<li> Calculate time left when in battery mode
<li> Retrieve the CPU (thermal zone) temperature
<li> Set cpu desired governor (powersave, performance, ondemand)
<li> Comes with a easy to use library
<li> Includes a Gkrellm2 plugin
<li> Includes a Xfce4 panel plugin (xfce 4.2.0)
<li> Retrieve complete list of available CPU frequencies
<li> Present a visual alert when battery is empty (Gkrellm2 and Xfce4 plugins)
<li> Collect many ACPI related data and present them with unified interface
</ul>
<br>
This is achieved using the Linux 2.6.x cpu frequency and ACPI functionalities. WPD is untested
under 2.4.x kernels, please feel free to try it and report your mileage on the official Mailing List.
<br>
WPD consist in a very lightweight daemon which sits in the background and a client side 
easy to use library. 
<br>
WPC is a command line client which can contol easily the daemon
<br>
WPK is a <a href="http://www.gkrellm.net">GKrellm2</a> plugin which collect all the data
retrieved by WPD and shows them in an ordered fashion.
<br>
The Xfce4 panel plugin is a <a href="http://www.xfce.org">Xfce4 (>=4.2.0)</a> panel plugin which collect all the data retrieved by WPD and shows them in an ordered fashion.
<br>
<br>
<i><u>Even if WPD is already fully functional and operational a LOT of work still has to be done.
WPD is open source software, anybody who would like to became a developer of WPD is really
invited to do so.</u></i>

<br>
<br>
A few things which will be added in a future:
<br><ul>
<li> Support for security and privileges
<li> Scripting customization
<li> More data collection
<li> hard disk / PCMCIA / audio powersaving
<li> More advanced ACPI management
<li> Tons of other things
<li> Ondemand governor tuning
</ul>
<br>
<br>
<center>Here are some screen-shoots for your pleasure:</center>
<br>
<table border="0"><tr><td>
<img src="gkrellm-scr1.jpg">
</td><td>
This is the GKrellm2 plugin. You can see, from top to bottom: active governor, cpu frequency, discharge rate, remaining charge, remaining time, temperature.
</td></tr></table>
<table>
<tr><td><img src="xfce4-scr1.jpg"></td></tr>
<tr><td>
<br> This is the XFCE4 plugin you can see from left to right: current frequency, current rate, remaining charge, temperature, remaining time. Holding the mouse over the plugin will show a exaustive tooltip. Currently the icon visualization is off: check the logo and icon contest!
</td></tr></table>
<table><tr><td>
<img src="wpc-scr1.jpg"></td>
</tr><tr><td>
These are all the options of the WPC command line interface.
</td></tr></table>
<br>
<br>
<br>

<br>
<center>INSTALLATION</center>
<br>
Unpack the sources and follow instruction in the README. In the package you will find
the WPD daemon, the WPC client and the WPK gkrellm2 plugin and the Xfce4 plugin.
<br>
<br>
<center>UTILIZATION NOTES - VERY IMPORTANT</center>
<br>
Since version 0.7.2 WPD support autoconfiguration, so you DO NOT NEED a configuration file anymore. But if you have any troubles or you need to switch off any feature you still need to edit the wpd.conf configuration file which usually should be located under /etc/wpd.conf.
<br>
<br>
<br>
Troubleshooting:
<br>
<br>
Please do not use any " in the configuration file. Also please uncomment ALL the configuration options
<br>
A read FAQ will be added in the future. Send your questions to:
<br>
Willy Gardiol <a href="mailto://willy@gardiol.org">willy@gardiol.org</a>
<br>
<br>
<table border="1"><tr><td>
<center><font color="#ff5500" size="4">WPD mailinglist</font></center>
<br>
The mailinglist discuss the future and the present of WPD use and development. To subscribe please 
send an email to:
<br>
<a href="mailto://wpd-subscribe@gardiol.org">wpd-subscribe@gardiol.org</a>
<br>
<br>
Posting to the list is open, if you do not want to subscribe you can just write to:
<br>
<a href="mailto://wpd@gardiol.org">wpd@gardiol.org</a>
<br>
</td></tr></table>
<br>
<br>
<br><center>Dowload source code: <a href="http://www.gardiol.org/wpd/wpd-0.7.5.tar.gz">wpd-0.7.5.tar.gz (330kb)</a></center>
<br>
<br>
<br><center>Dowload Gentoo EBUILD: <a href="http://www.gardiol.org/wpd/wpd-0.7.5.ebuild">wpd-0.7.5.tar.ebuild (1kb)</a></center>
<br>
<br>
<br>

<br>
<br>
<br>
<?php
	$file = fopen("contatore.txt", "r");
	$n = fgets( $file );
	fclose($file);
	$file = fopen("contatore.txt", "w");
	echo "(Visits: ".$n.")";
	fwrite( $file, $n+1 );
	fclose($file);
?>
<br>

</body>
</html>

