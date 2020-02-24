/* cmp_seek.c
   ==========
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
 $Log: cmp_seek.c,v $
 Revision 1.7  2001/06/27 20:46:56  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:11:51  barnes
 Added Author Name

 Revision 1.5  1999/07/26 16:50:51  barnes
 Modifications so that we don't tailspin on files with bad scan flags -
 The idea is to limit the number of beams in a single scan to 32, any more
 than that and we force a scan flag to be set.

 Revision 1.4  1999/07/23 22:26:48  barnes
 When the time requested is before the start of the file we need to make
 sure that the file pointer is not made negative.
 In another issue we need to make certain that when we identifier the
 actual record time, we read a record containing the time.

 Revision 1.3  1999/05/21 13:46:16  barnes
 Fixed bug in the rewind mechanism.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:24:26  barnes
 Initial revision

 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "limit.h"
#include "cmpdata.h"
#include "cmp_read.h"
#include "cnv_data.h"
#include "cnv_time.h"

int cmp_seek(struct cmpfp *ptr,
		 int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct cmpdata *cmp=NULL;
  int status=0;
  int recno;
  double tval;
  int bjmp=0;
  tval=time_epoch(yr,mo,dy,hr,mt,sc);
  cmp=malloc(sizeof(struct cmpdata));
  cmp->frang=0;
  cmp->rsep=0;
  if (cmp==NULL) return -1;


  if (tval<ptr->ctime) {
    /* rewind the file */
    if (ptr->fp !=-1) lseek(ptr->fp,0,SEEK_SET);
    else ptr->fptr=0;
    ptr->ctime=0;
    ptr->blen=0;
  }

  if (tval>=ptr->ctime) {
    do {
     bjmp=ptr->blen;
     status=cmp_read(ptr,cmp);
    } while ((tval>=ptr->ctime) && (status !=-1));

    if (status!=-1) {
      if (ptr->fp !=-1) lseek(ptr->fp,-(ptr->blen+bjmp),SEEK_CUR);
        else ptr->fptr-=ptr->blen+bjmp;
    }
  }
  /* get the actual time */

  do {  
    status=cmp_read(ptr,cmp);
  } while (status !=0);

  if (ptr->fp !=-1) lseek(ptr->fp,-ptr->blen,SEEK_CUR);
  else ptr->fptr-=ptr->blen;

  if (atme !=NULL) {
    *atme=ptr->ctime;
  }

  if (cmp->frang !=0) ptr->hdr.frang=cmp->frang;
  if (cmp->rsep !=0) ptr->hdr.rsep=cmp->rsep;

  free(cmp);
  if (status==-1) return 0;
  return 0;
} 






