/* cfitscan.c
   ==========
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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
#include "limit.h"
#include "cfitdata.h"
#include "scandata.h"
#include "cfitread.h"

/*
 $Log: cfitscan.c,v $
 Revision 1.3  2004/05/04 18:18:23  barnes
  Renamed the type header to avoid confusion.

 Revision 1.2  2003/09/13 22:36:31  barnes
 Modifcation to use the new data structures.

 Revision 1.1  2003/05/11 20:04:43  barnes
 Initial revision

*/



int CFitReadRadarScan(struct CFitfp *fp,int *state,
                 struct RadarScan *ptr,struct CFitdata *cfit,int tlen,
                 int lock,int chn) {

  int fstatus=0;
  int flg=0;
  int n=0;
  int r,rng;
  struct RadarBeam *tmp;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (cfit==NULL) return -1;
  if (fp==NULL) return -1;
  if (state==NULL) return -1;
  if (*state!=1) {
    if (chn==0) fstatus=CFitRead(fp,cfit);
    else {
      do {
        fstatus=CFitRead(fp,cfit); /* read first fit record */
        if (fstatus==-1) break;
      } while (cfit->channel !=chn);
    }
    if (fstatus==-1) return -1;
  }
  if (*state !=2) {
    ptr->stid=cfit->stid;
    ptr->version.major=cfit->version.major;
    ptr->version.minor=cfit->version.minor;
    ptr->st_time=cfit->time;
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
 
    bm->time=cfit->time;
    bm->scan=cfit->scan;
    bm->bm=cfit->bmnum;
    bm->cpid=cfit->cp;
    bm->intt.sc=cfit->intt.sc;
    bm->intt.us=cfit->intt.us;
    bm->nave=cfit->nave;
    bm->frang=cfit->frang;
    bm->rsep=cfit->rsep;
    bm->rxrise=cfit->rxrise;
    bm->freq=cfit->tfreq;
    bm->noise=cfit->noise;
    bm->atten=cfit->atten;
    bm->channel=cfit->channel;
    bm->nrang=cfit->nrang;

    for (r=0;r<MAX_RANGE;r++) bm->sct[r]=0;
    for (r=0;r<cfit->num;r++) {
      rng=cfit->rng[r];
      bm->sct[rng]=1;
      bm->rng[rng].gsct=cfit->data[r].gsct;
      bm->rng[rng].p_0=cfit->data[r].p_0;
      bm->rng[rng].p_0_e=cfit->data[r].p_0_e;
      bm->rng[rng].v=cfit->data[r].v;
      bm->rng[rng].p_l=cfit->data[r].p_l;
      bm->rng[rng].w_l=cfit->data[r].w_l;
      bm->rng[rng].v_e=cfit->data[r].v_e;   
      bm->rng[rng].p_l_e=cfit->data[r].p_l_e;   
      bm->rng[rng].w_l_e=cfit->data[r].w_l_e;   
    }
    
    ptr->num++;
    ptr->ed_time=cfit->time;
    if (ptr->num>100) {
      flg=-1;
      break;
    }
    if (chn==0) fstatus=CFitRead(fp,cfit);
    else {
      do { 
        fstatus=CFitRead(fp,cfit);
        if (fstatus==-1) break;
      } while (cfit->channel !=chn);
    }
    if (fstatus==-1) flg=2;
    else {
      if (tlen==0) {
        if (cfit->scan==1) flg=1;
      } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;
    }
  } while (flg==0);
  if (flg>0) flg--;
  if (flg==1) *state=2;
  return flg;
}


int CFitToRadarScan(struct RadarScan *ptr,struct CFitdata *cfit) {

  int n=0;
  int r,rng;
  struct RadarBeam *tmp;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (cfit==NULL) return -1;
  
  ptr->stid=cfit->stid;
  
  n=ptr->num;
  if (ptr->bm==NULL) ptr->bm=malloc(sizeof(struct RadarBeam));
  else {
    tmp=realloc(ptr->bm,sizeof(struct RadarBeam)*(n+1));
    if (tmp==NULL) {       
      return -1;
    }
    ptr->bm=tmp;
  }
    
  bm=&ptr->bm[n];    
 
  bm->time=cfit->time;
  
  bm->bm=cfit->bmnum;
  bm->cpid=cfit->cp;
  bm->intt.sc=cfit->intt.sc;
  bm->intt.us=cfit->intt.us;
  bm->nave=cfit->nave;
  bm->frang=cfit->frang;
  bm->rsep=cfit->rsep;
  bm->rxrise=cfit->rxrise;
  bm->freq=cfit->tfreq;
  bm->noise=cfit->noise;
  bm->atten=cfit->atten;
  bm->channel=cfit->channel;
  bm->nrang=cfit->nrang;

  for (r=0;r<MAX_RANGE;r++) bm->sct[r]=0;
  for (r=0;r<cfit->num;r++) {
    rng=cfit->rng[r];
    bm->sct[rng]=1;
    bm->rng[rng].gsct=cfit->data[r].gsct;
    bm->rng[rng].v=cfit->data[r].v;
    bm->rng[rng].p_l=cfit->data[r].p_l;
    bm->rng[rng].w_l=cfit->data[r].w_l;
    bm->rng[rng].v_e=cfit->data[r].v_e;   
    bm->rng[rng].p_l_e=cfit->data[r].p_l_e;   
    bm->rng[rng].w_l_e=cfit->data[r].w_l_e;   
  }  
  ptr->num++;
  return 0;
}



