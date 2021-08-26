/* raw_seek.c 
   ========== */

/*
 $Log: raw_seek.c,v $
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


