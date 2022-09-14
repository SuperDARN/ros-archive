/* csdseek.c
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
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








