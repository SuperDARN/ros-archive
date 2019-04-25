Radar Operating System 1.25 (BETA)
==================================

Version Log
===========

1.25     Minor bug fixes to the I&Q code.
1.24     Added iqcopy library to store I&Q samples in a consistent fashion
         regardless of radar
1.23     Included I&Q sample capture via iqwrite
1.22     integrated R.Todd Parris versions of the digital receiver drivers,
         working version of stereo with digital receiver support, numerous bug
         fixes
1.21     initial inclusion of stereo digital receiver support
1.20     fixed bug in tdisplay the was causing problems for CUTLASS
1.19     minor bug fixes on the road to stereo
1.18     more bug fixes and new fitacf 2.00
1.17     numerous bug fixes, improvements to rawfeed and lagtable
1.16     added support for SPEAR
1.15     removed some installation dependencies
1.14     added the echoraw mechansim for accessing raw samples
1.13     integration of the stereo mode code
1.12     bug fixes and changes from various sources
1.11     general bug fixes and move to ksh enviroment
1.10     fixed bug in setting the permissions on the setup script
1.09     fixed bug in the setup script
1.08     integrated changes for Wallops and Saskatoon
1.07     digital receiver bug fixes.
1.06     fixed bug in getting buffer addresses for conventional radars
1.05     code audit.
1.04     first digital receiver release.
1.03     addition of John Hughes DC offset code.
1.02     numerous bug fixes.
1.01	 initial revision of the code.

                              

Before You Begin
================

IMPORTANT: This code is BETA code and under constant development. 

* Do not try and install this code over and existing RST-ROS disribution

     There are a number of fiddly little differences in environment
     variable definitions, filename conventions and the like. The two
     versions do not play well together yet. 
     Also large chunks of code have been re-written and the version
     numbers will cause problems with the old code.
 
     Install this version of RST-ROS in a nice clean new directory

* Environment variables have been changed

     The format of some of the radar tables has changed, and at
     an early stage is made sense to define new environment variables
     to reference these new files, so:

     SD_RADAR	  replaces SD_RADARNAME 
     SD_HDWPATH	  replace  SD_HARDWARE 

     Also note that SD_HDWPATH points to the directory containing the
     hardware files, NOT the filename prefix as before. (The hardware
     file is now referenced in "radar.dat").

* Data directories are different

     The "/data/dat" directory has become "/data/raw". 

* Some program names have been changed

     "dio_drive" becomes "diopio48c"
     "a_d_drive" becomes "dt2828"

     This is to reflect changes in hardware.

* Control program names are different

    normal_scan becomes normalscan
    fast_scan becomes fastscan

*  Filenames and station identifiers 

    Filenames are given with a full time specification.
    Three letter codes are used for the radar ID.

*  Script names use a "." rather than "_"

    start_radar becomes start.radar
    stop_radar becomes stop.radar
    start_driver becomes start.driver





Creating Accounts
-----------------

The Radar Operating system is run from its own private user account which you 
must create before installing the software. By convention the software is 
run from an account called "radar". To create this account, login as 
super-user (root) and use the passwd utility:

    passwd radar

You will be prompted for a user ID (UID) and group ID (GID). You can either 
use the suggested values by typing enter or you can choose your own. 
You probably will not want to mix the radar account with your regular
user accounts so a good choice is to create an account with UID of 1000 in 
its own group with GID of 1000. 
You will then need to add the following line to the /etc/group file:

    radar:*:1000:

Install the Support Scripts
---------------------------

Before you can install the Operating System you must install the support 
scripts which are stored in the archive "support.x.xx.tar.F", the ("x.xx" 
signifies the version number of the archive).
Copy this archive into the root directory and type:

    install -u support.tar.F
  
This will install the two scripts "rpkg" and "make_sudo" into the 
directory "/usr/local/bin".

Install the ROS software
=========================

You are now ready to install the ROS software. Login using the user 
name "radar". There are three ROS software packages; the main Operating System
("ros.qnx4.x.yy.sh"), the site package for your radar ("site.zzz.x.yy.sh"),
and the control program package ("cp.x.yy.sh"). Copy these three
files in to the home directory of the radar account ("/home/radar"). Make
sure that they are executable by typing:

chmod a+x ros.qnx4.x.yy.sh
chmod a+x site.zzz.x.yy.sh
chmod a+x cp.zzz.x.yy.sh

Now install the main Operating System by typing:

    ./ros.qnx4.x.yy.sh

The installation directory will be displayed on the console and you will be
asked whether you wish to proceed. Once the directorry structure is created
 and the software is installed you will be asked if you wish to compile 
the core software and create the symbolic links to the library headers.

Once this is complete you can install the site package by typing:

    ./site.zzz.x.yy.sh

The installation directory will be displayed on the console and you will be
asked whether you wish to proceed. Once the directorry structure is created
 and the software is installed you will be asked if you wish to create 
the symbolic links to the library headers.

Once you have installed the site library you can install the control programs
by typing:

   ./cp.x.yy.sh

Once you have installed the software  you must update your ".profile" to 
include the ROS environment variables.You must add the following lines
 to ".profile":

    export HOMEPATH=${HOME} 
    . profile.ros.ksh
    . profile.site.ksh

(Alternatively you can incorporate the contents of "profile.ros.ksh" and 
"profile.site.ksh" into ".profile".). 

Setting up the Operating System
===============================

Next update the environment by either logging out and logging back into the radar account or by typing:

    . .profile

Creating Data Directories
-------------------------

The data and log files created by the Operating System are stored in 
sub-directories of the "/data" directory. You can create the required directory
tree by typing:

    make.data

Compiling the Code
------------------

Type the command:

    make.all

This will compile the operating system, the site libaries and the control 
programs that you have installed.

Testing the software (Debug Mode)
---------------------------------

You should now be able to test that Operating System is working. Type the 
command:

    start.debug

This should run the software in debug mode using a short test file. You can
check that everything is working by typing:

    tdisplay

You can exit the display by pressing 'Q' or  "<ctrl-C>". Shutdown the 
Operating System by typing:

    stop.debug


Configuring the Timing Computer
-------------------------------

Login as super-user (root) and copy the file "/home/radar/bin/diopio48c" to
the root directory of the timing computer. Then add a line to the 
"/etc/config/sysinit.2" file to run this program at boot time.


Configuring the Main Computer
-----------------------------

Add the following lines to the end of the "/etc/config/sysinit.1" file:

    ./home/radar/script/start.driver
    su radar /home/radar/script/start.radar

Installing a Schedule
---------------------

Before you can run the radar you must install a schedule file, an example is
provided in the file "/home/radar/radar.scd". Login as "radar", edit this 
file and copy it to the directory "/data/scd".

Run the Radar
-------------

Now reboot the system. If everything works according to plan, the radar you 
should now be running the new software. 

Operations
----------

All of the operations for the radar are controlled from the "radar" account. 
To stop the radar type:

    stop.radar

To restart the radar type:
   
   start.radar







































































