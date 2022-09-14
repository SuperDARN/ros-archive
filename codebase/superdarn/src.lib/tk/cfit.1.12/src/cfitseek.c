/* cfitseek.c
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
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"





int CFitSeek(struct CFitfp *ptr,
		 int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct CFitdata *cfit=NULL;
  int status=0;
  double tval;
  int bjmp=0;
  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  cfit=malloc(sizeof(struct CFitdata));
  cfit->frang=0;
  cfit->rsep=0;
  if (cfit==NULL) return -1;


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
     status=CFitRead(ptr,cfit);
    } while ((tval>=ptr->ctime) && (status !=-1));

    if (status!=-1) {
      if (ptr->fp !=-1) lseek(ptr->fp,-(ptr->blen+bjmp),SEEK_CUR);
        else ptr->fptr-=ptr->blen+bjmp;
    } else return -1;
  }
  /* get the actual time */

  do {  
    status=CFitRead(ptr,cfit);
  } while (status !=0);

  if (ptr->fp !=-1) lseek(ptr->fp,-ptr->blen,SEEK_CUR);
  else ptr->fptr-=ptr->blen;

  if (atme !=NULL) {
    *atme=ptr->ctime;
  }

  free(cfit);
  if (status==-1) return 0;
  return 0;
} 








