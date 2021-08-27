/* fit_seek.c
   ========== */

/* 
 $Log: fit_seek.c,v $
 Revision 1.4  1999/03/16 19:13:49  barnes
 Fixed possible problem with sequential seek.

 Revision 1.3  1999/03/16 16:14:25  barnes
 Fixed possible problem when doing a sequential search and an exact
 match is found.

 Revision 1.2  1999/03/16 15:02:08  barnes
 Added check for null file pointer.

 Revision 1.1  1999/03/16 14:36:30  barnes
 Initial revision

 */


/* seeks a record from a fitfile */

#include <stdio.h>
#include <stdlib.h>

#include "cnv_data.h"
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "fit_str.h"
#include "fit_read_inx.h"

int fit_seek(struct fitfp *ptr,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  double sec;
  struct fitdata *fit=NULL;
  int status=0;
  int bjmp=0;
  int recno;
  double tval;

  if (ptr==NULL) return -1;

  tval=time_epoch(yr,mo,dy,hr,mt,sc);
  if ((ptr->stime !=-1) && (tval<ptr->stime)) {
    if (ptr->fitfp !=-1) lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);
    else ptr->fptr=ptr->fit_recl;
    ptr->ctime=ptr->stime;
    if (atme !=NULL) *atme=ptr->ctime;
    return 2;
  }

  if ((ptr->etime !=-1) && (tval>ptr->etime)) {
    if (ptr->fitfp !=-1) 
      lseek(ptr->fitfp,(ptr->inx_erec-1)*ptr->fit_recl,SEEK_SET);
    else ptr->fptr=(ptr->inx_erec-1)*ptr->fit_recl;
    ptr->ctime=ptr->etime;
    if (atme !=NULL) *atme=ptr->ctime;
    return 3;
  }
 
  if ((ptr->ibuf !=NULL) || (ptr->inxfp !=-1)) {
 
    /* search using the index record */

    int32 inx_rec[4];
    int prec=-1;
    int recno=0;
    int srec,erec;
    double stime,etime,sec,itme;
    int syrsec;
    stime=ptr->stime;
    etime=ptr->etime;
    srec=ptr->inx_srec;
    erec=ptr->inx_erec; 
   
    /* get the actual start time of the file */
    epoch_time(stime,&yr,&mo,&dy,&hr,&mt,&sec);

    syrsec=time_yrsec(yr,mo,dy,hr,mt,sec);
  
    do { 
      prec=recno;
      recno=srec+(int) ((tval-stime)*(erec-srec))
            /(etime-stime); 
      if (fit_read_inx(ptr,inx_rec,recno) == -1) status=-1;
      if (status==0) {
        if (inx_rec[0]<syrsec) itme=time_epoch(yr+1,1,1,0,0,0); 
        else itme=time_epoch(yr,1,1,0,0,0); 
        itme+=inx_rec[0];
	if (itme==tval) break;
        else if (itme<tval) {
          srec=recno;
          stime=itme;
        } else {
          erec=recno;
          etime=itme;
        }
      }
    } while ((prec != recno) && (status==0));
    
    if (status==0) {

      if (itme !=tval) {
        recno=srec;
        if (recno>erec) recno=erec; 

      }  
      fit_read_inx(ptr,inx_rec,recno);

      /* align the read fit routine */
      if (ptr->fitfp !=-1) 
         lseek(ptr->fitfp,(inx_rec[1]-1)*ptr->fit_recl,SEEK_SET);
      else ptr->fptr=(inx_rec[1]-1)*ptr->fit_recl;
      if (atme !=NULL) *atme=itme;
      if (itme==tval) return 0;
      else return 1;
    }
  } 

  /* try and do the skip by reading fit records */

  fit=malloc(sizeof(struct fitdata));
  if (fit==NULL) return -1;

  if (tval<ptr->ctime) {
    /* rewind the file */
    if (ptr->fitfp !=-1) lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);
    else ptr->fptr=ptr->fit_recl;
    status=fit_read(ptr,fit);
  }
  if (tval>=ptr->ctime) {
    do {
      bjmp=ptr->blen;
      status=fit_read(ptr,fit);
    } while ((tval>=ptr->ctime) && (status==0));
   
    if (status !=0) {
      free(fit);
      return -1;
    }
    if (ptr->fitfp !=-1) lseek(ptr->fitfp,
                             -ptr->fit_recl*(ptr->blen+bjmp),SEEK_CUR);
    else ptr->fptr-=ptr->fit_recl*(ptr->blen+bjmp);
  }
  /* now determine the actual time of the record */

  status=fit_read(ptr,fit);

  /* rewind the file after we've read it so that fit_read gets this record */

  if (ptr->fitfp !=-1) lseek(ptr->fitfp,
                             -ptr->fit_recl*ptr->blen,SEEK_CUR);

  else ptr->fptr-=ptr->fit_recl*ptr->blen;

  if (atme !=NULL) *atme=ptr->ctime;
  free(fit);   
  return 4;
  
}




