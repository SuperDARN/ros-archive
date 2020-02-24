/* raw_seek.c
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
 $Log: raw_seek.c,v $
 Revision 1.4  2001/06/27 20:50:05  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.2  1999/03/17 16:33:19  barnes
 Fixed bug in rewinding the file after the seek.

 Revision 1.1  1999/03/16 16:32:48  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cnv_time.h"
#include "rawdata.h"
#include "raw_read.h"

int raw_seek(struct rawfp *ptr,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  int status=0;
  double tval=0;
  int rjmp=0;
  struct rawdata *raw=NULL;
  tval=time_epoch(yr,mo,dy,hr,mt,sc);
 
  raw=malloc(sizeof(struct rawdata));
  if (raw==NULL) return -1;

  if (tval<ptr->ctime) {
    lseek(ptr->rawfp,ptr->frec,SEEK_SET);
    ptr->ptr=ptr->frec;
    status=raw_read(ptr,raw);
  }
 
  do {
    rjmp=ptr->rlen;
    status=raw_read(ptr,raw);
  } while ((tval>=ptr->ctime) && (status==0));
   

  lseek(ptr->rawfp,-(ptr->rlen+rjmp),SEEK_CUR);
  ptr->ptr-=ptr->rlen+rjmp;
  
  status=raw_read(ptr,raw);
  
  lseek(ptr->rawfp,-ptr->rlen,SEEK_CUR);
  ptr->ptr-=ptr->rlen;
  
  if (atme !=NULL) *atme=ptr->ctime;
  free(raw);
  return 4;
}


