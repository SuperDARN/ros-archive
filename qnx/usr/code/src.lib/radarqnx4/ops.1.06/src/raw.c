/* raw.c
   =====
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
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "rmsg.h"
#include "radarshell.h"
#include "global.h" 
#include "version.h"


/*
 $Log: raw.c,v $
 Revision 1.2  2006/02/07 19:50:44  barnes
 Simon Shepherd's modification to the lag table.

 Revision 1.1  2004/05/06 20:10:02  barnes
 Initial revision

*/

void OpsBuildRaw(struct RadarParm *prm,struct RawData *raw,
                 int *ptab,int (*lags)[2]) {
  int i,j;
  prm->revision.major = MAJOR_VERSION;
  prm->revision.minor = MINOR_VERSION;
  prm->stid = stid;
  prm->channel=0;
  prm->time.yr=yr;
  prm->time.mo=mo;
  prm->time.dy=dy;
  prm->time.hr=hr;
  prm->time.mt=mt;
  prm->time.sc=sc;
  prm->time.us=us;
  prm->txpow = txpow;
  prm->nave=nave;
  prm->atten = atten;
  prm->lagfr = lagfr;
  prm->smsep = smsep;
  prm->noise.search=noise;
  prm->noise.mean=noisestat[0];
  prm->rxrise= rxrise;
  prm->intt.sc  = intsc;
  prm->intt.us  = intus;
  prm->txpl  = txpl;
  prm->mpinc = mpinc;
  prm->mppul = mppul;
  prm->mplgs = mplgs;
  prm->nrang = nrang;
  prm->frang = frang;
  prm->rsep = rsep;
  prm->bmnum = bmnum;
  prm->xcf   = xcf;
  prm->tfreq = tfreq;
  prm->scan  = scan;
  prm->mxpwr = mxpwr;
  prm->lvmax = lvmax;
  prm->cp	= cp;

  prm->stat.agc=agcstat;
  prm->stat.lopwr=lopwrstat;

  for (i=0;i<mppul;i++) prm->pulse[i]=ptab[i];
  for (i=0;i<2;i++) {
    for (j=0;j<mplgs;j++) prm->lag[j][i]=lags[j][i];
  } 

  strncpy(prm->combf,combf,COMBF_SIZE);

  for (i=0;i<nrang;i++) raw->pwr0[i]=pwr0[i];
  for (i=0;i<nrang;i++) {
    for (j=0;j<mplgs;j++) {
      raw->acfd[i][j][0]=acfd[(i*mplgs*2)+(2*j)];
      raw->acfd[i][j][1]=acfd[(i*mplgs*2)+(2*j)+1];
      raw->xcfd[i][j][0]=xcfd[(i*mplgs*2)+(2*j)];
      raw->xcfd[i][j][1]=xcfd[(i*mplgs*2)+(2*j)+1];
    }
  }
}
