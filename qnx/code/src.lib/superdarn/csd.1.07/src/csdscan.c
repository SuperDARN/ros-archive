/* csdscan.c
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
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "csddata.h"
#include "scandata.h"
#include "csdread.h"

/*
 $Log: csdscan.c,v $
 Revision 1.4  2004/11/29 18:54:09  barnes
 Extended buffer to 1000 beams to deal with Stereo radars running with short
 integration times.

 Revision 1.3  2004/05/04 18:23:40  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:56:30  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:37:47  barnes
 Initial revision

*/



int CSDReadRadarScan(struct CSDfp *fp,int *state,
                 struct RadarScan *ptr,struct CSDdata *csd,int tlen,
                 int lock,int chn) {

  int fstatus=0;
  int flg=0;
  int n=0;
  int r;
  struct RadarBeam *tmp;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (csd==NULL) return -1;
  if (fp==NULL) return -1;
  if (state==NULL) return -1;
  if (*state!=1) {
    if (chn==0) {
      do {
        fstatus=CSDRead(fp,csd);
        if (fstatus==-1) break;
      } while (fstatus !=0);
    } else {
      do {
        fstatus=CSDRead(fp,csd); /* read first fit record */
        if (fstatus==-1) break;
      } while ((chn==2) && (csd->channel !=chn));
    }
    if (fstatus==-1) return -1;
  }
  if (*state !=2) {
    ptr->stid=csd->stid;
    ptr->version.major=csd->version.major;
    ptr->version.minor=csd->version.minor;
    ptr->st_time=csd->time;
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
 
    bm->time=csd->time;
    bm->scan=csd->scan;
    bm->bm=csd->bmnum;
    bm->cpid=csd->cp;
    bm->intt.sc=csd->intt.sc;
    bm->intt.us=csd->intt.us;
    bm->nave=csd->nave;
    bm->frang=csd->frang;
    bm->rsep=csd->rsep;
    bm->rxrise=csd->rxrise;
    bm->freq=csd->tfreq;
    bm->noise=csd->noise;
    bm->atten=csd->atten;
    bm->channel=csd->channel;
    bm->nrang=MAX_RANGE;

    for (r=0;r<MAX_RANGE;r++) {
      bm->sct[r]=0;
      if (csd->store[r] !=0) bm->sct[r]=1;
      bm->rng[r].gsct=csd->store[MAX_RANGE+r];
      if (csd->stored & 0x01) bm->rng[r].p_l=csd->pmax*
                                             csd->store[2*MAX_RANGE+r]/256.0;

      if (csd->stored & 0x02) bm->rng[r].v=csd->vmax*
                                     (csd->store[3*MAX_RANGE+r]-128.0)/128.0;

      if (csd->stored & 0x03) bm->rng[r].w_l=csd->wmax*
                                             csd->store[4*MAX_RANGE+r]/256.0;

      bm->rng[r].p_0=0;
      bm->rng[r].p_0_e=0;
      bm->rng[r].v_e=0;   
      bm->rng[r].p_l_e=0;   
      bm->rng[r].w_l_e=0;   

    }
    ptr->num++;
    ptr->ed_time=csd->time;
    if (ptr->num>1000) {
      flg=-1;
      break;
    }
    if (chn==0) {
      do { 
        fstatus=CSDRead(fp,csd);
        if (fstatus==-1) break;
      } while (fstatus !=0);
    } else {
      do { 
        fstatus=CSDRead(fp,csd);
        if (fstatus==-1) break;
      } while ((chn==2) && (csd->channel !=chn));
    }
    if (fstatus==-1) flg=2;
    else {
      if (tlen==0) {
        if (csd->scan==1) flg=1;
      } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;
    }
  } while (flg==0);
  if (flg>0) flg--;
  if (flg==1) *state=2;
  return flg;
}



