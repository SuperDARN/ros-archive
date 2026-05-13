/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/


char *hlpstr[]={
"diopio48c - Device driver for the PIO48C DIO card.\n",
"diopio48c --help\n",
"diopio48c [options] [dioname]\n",
"diopio48c -db [options] [dioname]\n",
"diopio48c -d [options]\n",

"--help\tprint the help message and exit.\n",
"-cf configfile\tread the configuration from the file configfile.\n",
"-ft freqfile\tread the forbidden frequencies from the file freqfile.\n",
"-p port\tuse the comma separated list dio as the list of port addresses of DIO cards.\n",
"-wd watchdog\tuse watchdog as the port address of the watchdog device.\n",
"-q\toperate in quiet mode, do not log information to the terminal.\n",
"-c\tinclude a command ticker that lists the most recent operations.\n",
"-s\tstereo mode radar.\n",
"-ph\tphase coding radar.\n",
"dioname\tregister the process with the Operating System under the name dioname.\n",
"-db\trun in debug mode, do not operate the hardware.\n",
"-d\twrite the configuration to standard output and exit.\n",

NULL};
