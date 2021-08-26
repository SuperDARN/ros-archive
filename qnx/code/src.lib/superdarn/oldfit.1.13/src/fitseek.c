/* fitseek.c
   ==========
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


/* seeks a record from a fitfile */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "fitstr.h"
#include "fitreadinx.h"

/*
 $Log: fitseek.c,v $
 Revision 1.4  2004/08/04 20:51:18  barnes
 Removed old code for memory buffering.

 Revision 1.3  2004/05/04 18:32:13  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:23:36  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

int OldFitSeek(struct OldFitFp *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
 
  int status=0;
  int bjmp=0;
 
  double tval;
  struct RadarParm prm;
  struct FitData fit;

  if (fp==NULL) return -1;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  if ((fp->stime !=-1) && (tval<fp->stime)) {
    lseek(fp->fitfp,fp->fit_recl,SEEK_SET);
    fp->ctime=fp->stime;
    if (atme !=NULL) *atme=fp->ctime;
    return 2;
  }

  if ((fp->etime !=-1) && (tval>fp->etime)) {

    int32 inx_rec[4];

    OldFitReadInx(fp,inx_rec,fp->inx_erec);

    lseek(fp->fitfp,(inx_rec[1]-1)*fp->fit_recl,SEEK_SET);
    fp->ctime=fp->etime;
    if (atme !=NULL) *atme=fp->ctime;
    return 3;
  }
 
  if (fp->inxfp !=-1) {
 
    /* search using the index record */

    int32 inx_rec[4];
    int prec=-1;
    int recno=0;
    int srec,erec;
    double stime,etime,sec,itme=0;
    int syrsec;
    stime=fp->stime;
    etime=fp->etime;
    srec=fp->inx_srec;
    erec=fp->inx_erec; 
   
    /* get the actual start time of the file */
    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sec);

    syrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sec);
  
    do { 
      prec=recno;
      recno=srec+(int) ((tval-stime)*(erec-srec))
            /(etime-stime); 
      if (OldFitReadInx(fp,inx_rec,recno) == -1) status=-1;
      if (status==0) {
        if (inx_rec[0]<syrsec) itme=TimeYMDHMSToEpoch(yr+1,1,1,0,0,0); 
        else itme=TimeYMDHMSToEpoch(yr,1,1,0,0,0); 
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
      OldFitReadInx(fp,inx_rec,recno);

      /* align the read fit routine */
      lseek(fp->fitfp,(inx_rec[1]-1)*fp->fit_recl,SEEK_SET);
      if (atme !=NULL) *atme=itme;
      if (itme==tval) return 0;
      else return 1;
    }
  } 

  /* try and do the skip by reading fit records */

  if (tval<fp->ctime) {
    /* rewind the file */
    lseek(fp->fitfp,fp->fit_recl,SEEK_SET);
    status=OldFitRead(fp,&prm,&fit);
  }
  if (tval>=fp->ctime) {
    do {
      bjmp=fp->blen;
      status=OldFitRead(fp,&prm,&fit);
    } while ((tval>=fp->ctime) && (status==0));
   
    if (status !=0) return -1;
    lseek(fp->fitfp,-fp->fit_recl*(fp->blen+bjmp),SEEK_CUR);
  }
  /* now determine the actual time of the record */

  status=OldFitRead(fp,&prm,&fit);

  /* rewind the file after we've read it so that fit_read gets this record */

  lseek(fp->fitfp,-fp->fit_recl*fp->blen,SEEK_CUR);

  if (atme !=NULL) *atme=fp->ctime;
  return 4;
  
}




