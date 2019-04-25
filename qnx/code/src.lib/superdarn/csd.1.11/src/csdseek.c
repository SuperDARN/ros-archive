/* csdseek.c
   =========
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtime.h"
#include "rtypes.h"
#include "limit.h"
#include "csddata.h"
#include "csdread.h"

/*
 $Log: csdseek.c,v $
 Revision 1.3  2004/05/04 18:23:40  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:56:30  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:37:47  barnes
 Initial revision

*/



int CSDSeek(struct CSDfp *ptr,
		 int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct CSDdata *csd=NULL;
  int status=0;
  
  double tval;
  int bjmp=0;
  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  csd=malloc(sizeof(struct CSDdata));
  csd->frang=0;
  csd->rsep=0;
  if (csd==NULL) return -1;


  if (tval<ptr->ctime) {
    /* rewind the file */
    if (ptr->fp !=-1) lseek(ptr->fp,0,SEEK_SET);
    else ptr->fptr=0;
    ptr->ctime=-1;
    ptr->blen=0;
  }

  if (tval>ptr->ctime) {
    do {
     bjmp=ptr->blen;
     status=CSDRead(ptr,csd);
    } while ((tval>=ptr->ctime) && (status !=-1));

    if (status!=-1) {
      if (ptr->fp !=-1) lseek(ptr->fp,-(ptr->blen+bjmp),SEEK_CUR);
        else ptr->fptr-=ptr->blen+bjmp;
    }
  }
  /* get the actual time */

  do {  
    status=CSDRead(ptr,csd);
  } while (status !=0);

  if (ptr->fp !=-1) lseek(ptr->fp,-ptr->blen,SEEK_CUR);
  else ptr->fptr-=ptr->blen;

  if (atme !=NULL) {
    *atme=ptr->ctime;
  }

  free(csd);
  if (status==-1) return 0;
  return 0;
} 








