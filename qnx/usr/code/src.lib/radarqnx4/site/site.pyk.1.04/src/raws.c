/* raws.c
   ======
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
 "Radar Operating System - Site Files" (hereinafter "the Program").
 
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
#include "globals.h"
#include "version.h"


/*
 $Log: raws.c,v $
 Revision 1.1  2006/03/30 11:02:44  barnes
 Initial revision

*/

void OpsBuildRawS(int channel,struct RadarParm *prm,struct RawData *raw,
                 int *ptab,int (*lags)[2]) {
  int i,j;
  prm->revision.major = MAJOR_VERSION;
  prm->revision.minor = MINOR_VERSION;
  prm->stid = stid;
  prm->time.yr=yr;
  prm->time.mo=mo;
  prm->time.dy=dy;
  prm->time.hr=hr;
  prm->time.mt=mt;
  prm->time.sc=sc;
  prm->time.us=us;
  prm->intt.sc  = intsc;
  prm->intt.us  = intus;

   
  if (channel==0) {
    prm->channel=1; 
    prm->txpow = txpowA;
    prm->nave=naveA;
    prm->atten = attenA;
    prm->lagfr = lagfrA;
    prm->smsep = smsepA;
    prm->noise.search=noiseA;
    prm->noise.mean=noisestatA[0];
    prm->rxrise= rxriseA;
    prm->txpl  = txplA;
    prm->mpinc = mpincA;
    prm->mppul = mppulA;
    prm->mplgs = mplgsA;
    prm->nrang = nrangA;
    prm->frang = frangA;
    prm->rsep = rsepA;
    prm->bmnum = bmnumA;
    prm->xcf   = xcfA;
    prm->tfreq = tfreqA;
    prm->scan  = scanA;
    prm->mxpwr = mxpwrA;
    prm->lvmax = lvmaxA;
    prm->cp	= cpA;

    prm->stat.agc=agcstatA;
    prm->stat.lopwr=lopwrstatA;

    prm->offset=delays[0]*10;

    strncpy(prm->combf,combfA,COMBF_SIZE);

    for (i=0;i<nrang;i++) raw->pwr0[i]=pwr0A[i];
    for (i=0;i<nrang;i++) {
      for (j=0;j<mplgs;j++) {
        raw->acfd[i][j][0]=acfdA[(i*mplgs*2)+(2*j)];
        raw->acfd[i][j][1]=acfdA[(i*mplgs*2)+(2*j)+1];
        raw->xcfd[i][j][0]=xcfdA[(i*mplgs*2)+(2*j)];
        raw->xcfd[i][j][1]=xcfdA[(i*mplgs*2)+(2*j)+1];
      }
    }
  } else {
    prm->channel=2; 
    prm->txpow = txpowB;
    prm->nave=naveB;
    prm->atten = attenB;
    prm->lagfr = lagfrB;
    prm->smsep = smsepB;
    prm->noise.search=noiseB;
    prm->noise.mean=noisestatB[0];
    prm->rxrise= rxriseB;
    prm->txpl  = txplB;
    prm->mpinc = mpincB;
    prm->mppul = mppulB;
    prm->mplgs = mplgsB;
    prm->nrang = nrangB;
    prm->frang = frangB;
    prm->rsep = rsepB;
    prm->bmnum = bmnumB;
    prm->xcf   = xcfB;
    prm->tfreq = tfreqB;
    prm->scan  = scanB;
    prm->mxpwr = mxpwrB;
    prm->lvmax = lvmaxB;
    prm->cp	= cpB;

    prm->stat.agc=agcstatB;
    prm->stat.lopwr=lopwrstatB;

    prm->offset=delays[0]*10;


    strncpy(prm->combf,combfB,COMBF_SIZE);

    for (i=0;i<nrang;i++) raw->pwr0[i]=pwr0B[i];
    for (i=0;i<nrang;i++) {
      for (j=0;j<mplgs;j++) {
        raw->acfd[i][j][0]=acfdB[(i*mplgs*2)+(2*j)];
        raw->acfd[i][j][1]=acfdB[(i*mplgs*2)+(2*j)+1];
        raw->xcfd[i][j][0]=xcfdB[(i*mplgs*2)+(2*j)];
        raw->xcfd[i][j][1]=xcfdB[(i*mplgs*2)+(2*j)+1];
      }
    }
  }
  for (i=0;i<mppul;i++) prm->pulse[i]=ptab[i];
  for (i=0;i<2;i++) {
    for (j=0;j<mplgs;j++) prm->lag[j][i]=lags[j][i];
  } 


}
