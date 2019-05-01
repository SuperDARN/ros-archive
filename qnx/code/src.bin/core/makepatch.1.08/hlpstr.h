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
 Revision 1.1  2004/07/14 16:16:44  barnes
 Initial revision

*/


char *hlpstr[]={
"makepatch - Build a patch distribution tree from a source code archive.\n",
"            The program searches a directory tree and builds a copy of it.\n",
"            If a source code directory managed under RCS is discovered,\n",
"            the program will check out the most recent version of the code\n",
"            into the destination directory.\n",
"            A comparison is made with an existing distribution and only\n"
"            updated parts of the tree are copied.\n\n",
"Usage: makepatch [--help]\n",
"       makepatch [-vb] [-p post] src dst cmp lst...\n",
"       makepatch [-vb] [-p post] src dst cmp stdin\n\n",
"--help   display this help message and exit.\n",
"-vb      verbose. Log status to standard error in addition to standard\n",
"         out.\n",
"-p post  post process the source code with the command given by post.\n",
"src      source directory in which to search.\n",
"dst      destination directory in which to create distribution.\n",
"cmp      comparison directory containing the existing distribution.\n",
"lst...   list of subdirectories to include in the distribution.\n\n",
"stdin    read the source list from standard input.\n",
NULL};

