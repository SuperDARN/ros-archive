/* smr_seek.c
   ========== */


/*
 $Log: smr_seek.c,v $
 Revision 1.1  1999/03/16 20:09:54  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

int smr_seek(FILE *fp,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct fitdata *fit=NULL;
  int status=0,sjmp=0;
  int ctime=0;
  int tval;
  tval=time_yrsec(yr,mo,dy,hr,mt,sc);
  fit=malloc(sizeof(struct fitdata));
  if (fit==NULL) return -1;
  fseek(fp,0,SEEK_SET);
 
  do {
    sjmp=status;
    status=smr_fread(fp,fit,0);
    if (status !=-1) ctime=time_yrsec(fit->prms.YEAR,
				    fit->prms.MONTH,
				    fit->prms.DAY,
				    fit->prms.HOUR,
				    fit->prms.MINUT,
				    fit->prms.SEC);
  } while ((tval>=ctime) && (status !=-1));
  free(fit);
  if (status==-1) return -1;
  fseek(fp,-(status+sjmp),SEEK_CUR);
  if (atme !=NULL) {
    status=smr_fread(fp,fit,0);
    *atme=time_epoch(fit->prms.YEAR,
		     fit->prms.MONTH,
		     fit->prms.DAY,
		     fit->prms.HOUR,
		     fit->prms.MINUT,
		     fit->prms.SEC);
    fseek(fp,-status,SEEK_CUR);
  }
  return 0;
}











