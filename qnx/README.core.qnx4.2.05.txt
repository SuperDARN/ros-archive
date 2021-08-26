RST - Core for qnx4 2.05
============================================
R.J.Barnes

Version Log
===========

2.05     fixed bug in makepatch
2.04     added ksh environment as an option
2.03     fixed a problem introduced by the XML documentation
2.02     further fix of the makefiles
2.01     fixed some of the makefiles
2.00     code adopted as official release
1.11     sanity check release
1.10     incoprorated full help and error message descriptions
         based on the XML documentation.
1.09     incorporated outline XML documentation in the binary directories.
1.08     moved to a self-extracting archive. Included a copy of the license
         in the distribution along with a README file. Added support for 
         Mac OS X (Darwin).
1.07	 fixed bugs in the installation script that prevented the software
	 from beining installed in arbitraty directories.
1.06	 fixed a bug in the master makefiles that meant that graphics programs
         would not compile if the X libraries were missing.
1.05	 placed headers under RCS and incorporated a mechanism for including
         documentation in packages.
1.04	 general improvements  and switch to safer temporary filename creation.
1.02	 added patch code and some file manipulation utilities.
1.01	 initial revision of the code.

Overview
========

The RST Core is the essential package that must be installed before other
RST components. It contains the code management software, scripts, 
environment variables,and outline makefiles for compiling code. 
Installing the package also sets up the directory structure for the RST.
These installation instructions assume that the RST is being installed on
a Linux system using the "tcsh" shell. 

The software should be installed under a regular user account, not as 
the superuser.


System Requirements
===================

	+ Any modern Linux distribution (RedHat, Suse, Debian)

   or

	+ QNX4.24 or higher

   or

	+ MacOS X 10.3, X11, Xcode development tools.

Installation
============

Copy the self extraing archive to the installation directory. (In this example "/rst"):

cp core.qnx4.2.05.sh /rst

Next, make the archive executable:

cd /rst
chmod a+x core.qnx4.2.05.sh 

Finally run the self-extracting archive:

./core.qnx4.2.05.sh

This should unpack the core software and compile the package managment utilities into the "/rst/bin" directory. 

Before continuing, you must set up some environment variables. You can do this by modifying your account profile, (normally stored in the file ".cshrc" in your home directory).
Open ".cshrc" in a text editor and add the following lines:


setenv OSTYPE "qnx4"
setenv HOMEPATH "/rst"
source ${HOMEPATH}/profile.core.tcsh

The environment variable "HOMEPATH" points to the directory that the software is installed in and is used to set up the rest of the environment.
The "source" statement tells the shell to read the file "profile.core.tcsh" and include the contents in your profile. (As an alternative, you could just append "profile.core" to your existing profile.

Next refresh your profile. You can either do this by logging out and logging back in, or by typing:

source ~/.cshrc

Bugs/Comments
=============

Please Email bugs and/or comments to robin.barnes@jhuapl.edu





