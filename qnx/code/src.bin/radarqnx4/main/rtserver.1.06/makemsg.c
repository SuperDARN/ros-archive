/* makemsg.c
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
#include "types.h"
#include "rconvert.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cnxstr.h"

#define read_bit(t,n) ((t[n/8] & (1<<(n%8))) !=0)
#define set_bit(t,n)  t[n/8]=t[n/8] | (1<<(n%8))
#define set_top(t,n) t[9]=t[9] | (n & 0x0f)<<4
#define read_top(t)  ((t[9] >> 4) &0x0f)

/* 
 $Log: makemsg.c,v $
 Revision 1.1  2004/05/10 15:38:47  barnes
 Initial revision

*/

struct IPMsg {
  char msg;
  struct radar_parms prm;
  char gsct[10];
  char dflg[10];
  double store[ORIG_MAX_RANGE*4];
};


int makemsg(struct RadarParm *prm,struct FitData *fit,
		 unsigned char **buffer) {

  int pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
  int dpat[]={8,0,0,0};
  int ltable[ORIG_MAX_RANGE];
  float snr[MAX_RANGE];
  float maxval;
  int inx,step,s;

  unsigned char *ptr=NULL;                
  unsigned char *dflg;
  unsigned char *gsct;
  double *store;

  int rng_num,loop;

  struct radar_parms oldprm;

  oldprm.REV.MAJOR=prm->revision.major;
  oldprm.REV.MINOR=prm->revision.minor;
  oldprm.NPARM=48;
  oldprm.CP=prm->cp;
  oldprm.ST_ID=prm->stid;
  oldprm.YEAR=prm->time.yr;
  oldprm.MONTH=prm->time.mo;
  oldprm.DAY=prm->time.dy;
  oldprm.HOUR=prm->time.hr;
  oldprm.MINUT=prm->time.mt;
  oldprm.SEC=prm->time.sc;
   
  oldprm.TXPOW=prm->txpow;
  oldprm.NAVE=prm->nave;
  oldprm.ATTEN=prm->atten;
  oldprm.LAGFR=prm->lagfr;
  oldprm.SMSEP=prm->smsep;
  oldprm.ERCOD=prm->ercod;
  oldprm.AGC_STAT=prm->stat.agc;
  oldprm.LOPWR_STAT=prm->stat.lopwr;
  oldprm.NOISE=prm->noise.search;
  oldprm.NOISE_MEAN=prm->noise.mean;
      
  oldprm.CHANNEL=prm->channel;
  oldprm.BMNUM=prm->bmnum;
  oldprm.SCAN=prm->scan;
 
  oldprm.RXRISE=prm->rxrise;
  oldprm.INTT=prm->intt.sc;
  oldprm.TXPL=prm->txpl;
    
  oldprm.MPINC=prm->mpinc;
  oldprm.MPPUL=prm->mppul;
  oldprm.MPLGS=prm->mplgs;
  oldprm.NRANG=prm->nrang;
  oldprm.FRANG=prm->frang;
  oldprm.RSEP=prm->rsep;
  oldprm.XCF=prm->xcf; 
  oldprm.TFREQ=prm->tfreq;
  oldprm.MXPWR=prm->mxpwr;
  oldprm.LVMAX=prm->lvmax;   

  if (oldprm.NRANG>ORIG_MAX_RANGE) oldprm.NRANG=ORIG_MAX_RANGE;

  if (*buffer !=NULL) {
    free(*buffer);
    *buffer=NULL;
  }

  for (loop=0;loop<prm->nrang;loop++) {
    if (prm->noise.search>0) snr[loop]=fit->rng[loop].p_0/prm->noise.search;
    else snr[loop]=0;
  }
  if (prm->nrang>ORIG_MAX_RANGE) {
    step=prm->nrang/ORIG_MAX_RANGE;
    
    for (loop=0;loop<ORIG_MAX_RANGE;loop++) {
      maxval=0.0;
      inx=loop*step;
      for (s=0;s<step;s++) {
        if (fit->rng[loop*step+s].qflg==0) continue;
        if (fit->rng[loop*step+s].gsct !=0) continue;
        if (snr[loop*step+s]>maxval) { 
          maxval=snr[loop*step+s];
          inx=loop*step+s;
        }
      }
      ltable[loop]=inx;
    }
    oldprm.RSEP=oldprm.RSEP*step;
  } else { 
    for (loop=0;loop<ORIG_MAX_RANGE;loop++) ltable[loop]=loop;
  }
  
  rng_num=0;
  for (loop=0;loop<ORIG_MAX_RANGE;loop++) 
    if (fit->rng[ltable[loop]].qflg ==1) rng_num++;
 
  ptr=malloc(sizeof(struct radar_parms)+21+3*sizeof(double)*rng_num);
  if (ptr==NULL) return 0;

  ptr[0]='d';
  gsct=ptr+1+sizeof(struct radar_parms);
  dflg=gsct+10;
  store=(double *) (dflg+10);

  memcpy(ptr+1,&oldprm,sizeof(struct radar_parms));
  ConvertBlock(ptr+1,pat);
 
  memset(gsct,0,10);
  memset(dflg,0,10);

  /* work out which ranges to store */
   
  for (loop=0;loop<ORIG_MAX_RANGE;loop++) {
    if (fit->rng[ltable[loop]].gsct !=0) set_bit(gsct,loop);
    if (fit->rng[ltable[loop]].qflg ==1) set_bit(dflg,loop);
  }
 
  /* store the ranges */
      
  if (rng_num>0) {
    int index=0;
    for (loop=0;loop<ORIG_MAX_RANGE;loop++) {
      if (read_bit(dflg,loop) !=0) {
        store[index]=fit->rng[ltable[loop]].p_l;
        store[index+rng_num]=fit->rng[ltable[loop]].v;
        store[index+2*rng_num]=fit->rng[ltable[loop]].w_l;  
	/* store[index+2*rng_num]=fit->rng[ltable[loop]].v_err; */
        index++;
      } 
    }
  }

  /* convert the stored values */
  dpat[1]=3*rng_num;
  ConvertBlock( (unsigned char *) store,dpat);
  
  

  *buffer=ptr;
  return (sizeof(struct radar_parms)+21+3*sizeof(double)*rng_num);
}















