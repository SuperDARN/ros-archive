/* logger.c
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
 $Log: logger.c,v $
 Revision 1.4  2001/06/27 20:55:38  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:12:06  barnes
 Added Author Name

 Revision 1.2  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.1  1996/04/29 16:27:53  root
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int yday=-1;

FILE *open_log(void) {
  char log_name[12];
  time_t log_clock;
  struct tm *gmt; 

  /* open a log file */
 
  time(&log_clock);
  gmt=gmtime(&log_clock);

  if ((gmt->tm_yday+1) != yday) {   
     sprintf(log_name,"dio.log.%3.3d",gmt->tm_yday+1);
     yday=gmt->tm_yday+1;
  }
  return fopen(log_name,"a");    
}

