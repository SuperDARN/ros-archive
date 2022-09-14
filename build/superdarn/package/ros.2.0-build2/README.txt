Radar Operating System $version$ $sys$
========================================


Version Log
===========

2.0	initial version of the code.

Installation
============

IMPORTANT: This code is BETA code and under constant development.

The Radar Operating System (ROS) is a component of the Radar Analysis Toolkit (RST). The operating system consists of the software necessary to operate the Radar hardware. 

To install and compile the software, download the self extracting archive "superdrn-ros.${sys}.x.y.sh". Make the archive executable by typing:

chmod a+x superdrn-ros.${sys}.x.y.sh

Execute the archive:

./superdarn-ros.${sys}.x.y.sh

The archive will do some initial setup. Incorporate the superdarn profile in with the by either copying to or referencing "profile.superdarn-ros.${sys}.zzzz" in the account profile. Refresh the profile to make sure the environment variables are up to date.

Compile the build code:

make.build

Finally compile the rest of the code using the make.code script:

make.code superdarn ros

Running the code
================

Data directories should be created in "/data/ros". The directories are:

errlogs
fit
iq
raw


The software can be started by typing "start.radar" and stopped by typing "stop.radar". By default control programs are not automatically started and should be run from the command line.

What is in the Operating System?
================================


radar	
-----

A "dummy" radar simulator that implements the radar server without requiring radar hardware. In an operational radar the "radar" task would be replaced by the embeded radar device driver on the QNX6 master machine.

errlog
------

The error logging task. This task logs error messages from the other software and from the control program to a daily file.

schedule
--------

The control program scheduler. The schedule file controls when control programs are started and stopped. The format of this file is the same as in previouse versions of the Operating System.

fitacfwrite
-----------

Writes fitACF data to a file.

rawacfwrite
-----------

Writes rawACF data to a file.

iqwrite
-------

Write raw I&Q sample data to a file.

rtserver
--------

Serves fitACF data in real-time to multiple clients.

shellserver
-----------

Provides the interface to the control program for the radarshell task.

radarshell
----------

An interactive tool for changing control program operating parameters dynamically.







