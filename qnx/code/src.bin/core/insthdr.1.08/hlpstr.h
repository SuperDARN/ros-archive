/* hlpstr.h
   ========
   Author: R.J.Barnes
*/


/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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


/*
 $Log: hlpstr.h,v $
 Revision 1.1  2004/07/14 16:08:38  barnes
 Initial revision

*/



char *hlpstr[]={
"insthdr - Install symbolic links to library headers.\n",
"          The program creates symbolic links to the headers of\n",
"          A library in the specified directory.\n\n",
"Usage: insthdr [--help]\n",
"       insthdr [-vb] [-a] [-q] src dst\n",
"       insthdr [-vb] [-a] [-q] src dst lst...\n",
"       insthdr [-vb] [-a] [-q] src dst stdin\n\n",
"--help  display this help message and exit.\n",
"-vb     verbose. Log status to standard error in addition to standard out.\n",
"-a      link all libraries found, do not perform version checking.\n",
"-q      quiet mode. Keep going if an error occurs.\n",
"src     source directory in which to search for libraries.\n",
"dst     destination directory in which to create links.\n\n",
"lst...  list of library subdirectories to create links for.\n\n",
"stdin   read library list from standard input.\n",
NULL};

