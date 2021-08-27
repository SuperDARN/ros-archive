                             Radar Operating System
                             ======================

                              

                                2001 R.J.Barnes

                            Installation Instructions
                            -------------------------

Before You Begin
================

Removing Old Software
---------------------

If you have an old version of the Operating System you should either remove or 
disable it by editing the files "/etc/config/sysinit.1" on the main computer 
and "/etc/config/sysinit.2" on the timing computer. You must also remove any 
references to the old software from the file "/etc/profile" on both computers.

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

Install the Core Software
=========================

You are now ready to install the core software. Login using the user 
name "radar". Copy the core package, called "core.qnx.x.xx.rpkg" into the 
home directory of this account ("/home/radar"). 
Next install the core software package by typing:

    rpkg core.qnx.x.xx.rpkg

This will install the core software needed to install the operating system 
and will set up the appropriate directory structure.

The package defines some environment variables in the file "profile.core". 
You must add the following lines to the account profile file, ".profile":

    export HOMEPATH=${HOME} 
    . profile.core

(Alternatively you can incorporate the contents of "profile.core" into 
".profile".). 

Next update the environment by either logging out and logging back into the radar account or by typing:

    . .profile

Then compile the core software by typing:

    make.core


Installing the Operating System
===============================

You are now ready to install the operating system. Download or copy the 
following files into the home directory:

tables.x.xx.rpkg  - hardware data tables.
ros.x.xx.rpkg	  - operating system code.
site.x.xx.y.rpkg  - site library. Substitute your site ID letter for the "y".
cp.x.xx.rpkg      - control programs.

Next install the software by typing the following commands:

rpkg tables.x.xx.rpkg
rpkg ros.x.xx.rpkg
rpkg site.x.xx.y.rpkg
rpkg cp.x.xx.rpkg


The operating system defines some environment variables in the 
files "profile.base" and "profile.site". 
You must add the following lines to the account profile file, ".profile":

    . profile.base 
    . profile.site

(Alternatively these files can be incorporated directly into ".profile").



Setting up the Operating System
===============================

Next update the environment by either logging out and logging back into the radar account or by typing:

    . .profile

Creating Data Directories
-------------------------

The data and log files created by the Operating System are stored in 
sub-directories of the "/data" directory. You can create the required directory
tree by typing:

    make_data

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

    start_debug

This should run the software in debug mode using a short test file. You can
check that everything is working by typing:

    tdisplay

You can exit the display by pressing 'Q' or  "<ctrl-C>". Shutdown the 
Operating System by typing:

    stop_debug


Configuring the Timing Computer
-------------------------------

Login as super-user (root) and copy the file "/home/radar/bin/dio_drive" to
the root directory of the timing computer. Then add a line to the 
"/etc/config/sysinit.2" file to run this program at boot time.


Configuring the Main Computer
-----------------------------

Add the following lines to the end of the "/etc/config/sysinit.1" file:

    ./home/radar/script/start_driver
    su radar /home/radar/script/start_radar

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

    stop_radar

To restart the radar type:
   
   start_radar







































































