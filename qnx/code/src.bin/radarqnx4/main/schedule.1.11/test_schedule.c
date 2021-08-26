/* test_schedule.c
   ===============
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rtime.h"
#include "log_info.h"
#include "schedule.h"

/*
 $Log: test_schedule.c,v $
 Revision 1.5  2004/06/22 13:12:00  barnes
 QNX4 Code audit.

 Revision 1.4  2004/05/10 15:19:06  barnes
 Modifcation to deal with new libraries.

 Revision 1.3  2001/06/27 20:56:24  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 1.1  1999/03/30 18:31:14  barnes
 Initial revision

 */

int test_schedule(struct scd_blk *ptr) {
  int yr,mo,dy,hr,mt,sc,us;
  double stime;
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
  stime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  if (ptr->cnt==0) return 0;  
  if (ptr->cnt==ptr->num) return 0;
  if (stime>=ptr->entry[ptr->cnt].stime) return 1;
  return 0;
}