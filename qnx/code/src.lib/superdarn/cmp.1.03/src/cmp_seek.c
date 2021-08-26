/* cmp_seek.c
   ========== */

/*
 $Log: cmp_seek.c,v $
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






