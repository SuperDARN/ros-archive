/* fitscan.c
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"

#include "oldfitread.h"

/*
 $Log: fitscan.c,v $
 Revision 1.2  2004/05/04 18:32:13  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/


int OldFitReadRadarScan(struct OldFitFp *fp,int *state,
                     struct RadarScan *ptr,
                     struct RadarParm *prm,struct FitData *fit,
                     int tlen,
                     int lock,int chn) {

  int fstatus=0;
  int flg=0;
  int n=0;
  int r;
  struct RadarBeam *tmp;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (fit==NULL) return -1;
  if (state==NULL) return -1;
  if (*state!=1) {
    if (chn==0) fstatus=OldFitRead(fp,prm,fit);
    else {
      do {
        fstatus=OldFitRead(fp,prm,fit); /* read first fit record */
        if (fstatus==-1) break;
      } while (prm->channel !=chn);
    }
    if (fstatus==-1) return -1;
  }
  if (*state !=2) {
    ptr->stid=prm->stid;
    ptr->version.major=prm->revision.major;
    ptr->version.minor=prm->revision.minor;
    ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
    if ((tlen !=0) && (lock !=0)) ptr->st_time=tlen*(int) (ptr->st_time/tlen);
    RadarScanReset(ptr);  
  }
  
  *state=1;
  do {

    n=ptr->num;
    if (ptr->bm==NULL) ptr->bm=malloc(sizeof(struct RadarBeam));
    else {
      tmp=realloc(ptr->bm,sizeof(struct RadarBeam)*(n+1));
      if (tmp==NULL) {
        flg=-1;        
        break;
      }
      ptr->bm=tmp;
    }
    
    bm=&ptr->bm[n];    
 
    bm->time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
  
    bm->bm=prm->bmnum;
    bm->cpid=prm->cp;
    bm->intt.sc=prm->intt.sc;
    bm->intt.us=prm->intt.us;
    bm->nave=prm->nave;
    bm->frang=prm->frang;
    bm->rsep=prm->rsep;
    bm->rxrise=prm->rxrise;
    bm->freq=prm->tfreq;
    bm->noise=prm->noise.search;
    bm->atten=prm->atten;
    bm->channel=prm->channel;
    bm->nrang=prm->nrang;

    for (r=0;r<MAX_RANGE;r++) bm->sct[r]=0;
    for (r=0;r<bm->nrang;r++) {
      bm->sct[r]=(fit->rng[r].qflg==1);
      bm->rng[r].gsct=fit->rng[r].gsct;
      bm->rng[r].p_0=fit->rng[r].p_0;
      bm->rng[r].p_0_e=0;
      bm->rng[r].v=fit->rng[r].v;
      bm->rng[r].p_l=fit->rng[r].p_l;
      bm->rng[r].w_l=fit->rng[r].w_l;
      bm->rng[r].v_e=fit->rng[r].v_err;    
    }
    
    ptr->num++;

    ptr->ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
    if (ptr->num>100) {
      flg=-1;
      break;
    }
    if (chn==0) fstatus=OldFitRead(fp,prm,fit);
    else {
      do { 
        fstatus=OldFitRead(fp,prm,fit);
        if (fstatus==-1) break;
      } while (prm->channel !=chn);
    }
    if (fstatus==-1) flg=2;
    else {
      if (tlen==0) {
        if (prm->scan==1) flg=1;
      } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;
    }
  } while (flg==0);
  if (flg>0) flg--;
  if (flg==1) *state=2;
  return flg;
}

