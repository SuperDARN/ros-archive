/* fitacf.c
   ========
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2009 JHU/APL
 
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "rmath.h"
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "radar.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fitblk.h"
#include "dofit.h"
#include "fitacfversion.h"

#define GOOSEBAY 1

void FitACFEnd(struct FitBlock *fptr) {
  int i;
  for (i=0;i<MAX_RANGE;i++) {
    free(fptr->acfd[i]);
    free(fptr->xcfd[i]);
  }
}

 
void FitACFStart(struct RadarSite *hd,
	       int year,struct FitBlock *fptr) {
  int i;
  if (year < 1993) fptr->prm.old=1;
  for (i=0;i<3;i++) fptr->prm.interfer[i]=hd->interfer[i];
  fptr->prm.bmsep=hd->bmsep;
  fptr->prm.phidiff=hd->phidiff;
  fptr->prm.tdiff=hd->tdiff;
  fptr->prm.vdir=hd->vdir;
  fptr->prm.maxbeam=hd->maxbeam;
  for (i=0;i<MAX_RANGE;i++) {
    fptr->acfd[i]=malloc(sizeof(struct complex)*LAG_SIZE);
    fptr->xcfd[i]=malloc(sizeof(struct complex)*LAG_SIZE);
  }
}

void FitACF(struct RadarParm *prm,
            struct RawData *raw,struct FitBlock *input,
	    struct FitData *fit) {
  int i,j;
  int fnum,goose;
  if (prm->time.yr < 1993) input->prm.old=1;

  fit->revision.major=FITACF_MAJOR_REVISION;
  fit->revision.minor=FITACF_MINOR_REVISION;

  input->prm.xcf=prm->xcf;
  input->prm.tfreq=prm->tfreq;
  input->prm.noise=prm->noise.search;
  input->prm.nrang=prm->nrang;
  input->prm.smsep=prm->smsep;
  input->prm.nave=prm->nave;
  input->prm.mplgs=prm->mplgs;
  input->prm.mpinc=prm->mpinc;
  input->prm.txpl=prm->txpl;
  input->prm.lagfr=prm->lagfr;
  input->prm.mppul=prm->mppul;
  input->prm.bmnum=prm->bmnum;
  input->prm.cp=prm->cp;
  input->prm.channel=prm->channel;
  input->prm.offset=prm->offset; /* stereo offset */


  /* need to incorporate Sessai's code for setting the offset
     for legacy data here.
  */

  for (i=0;i<input->prm.mppul;i++) input->prm.pulse[i]=prm->pulse[i];
  for (i=0;i<input->prm.mplgs;i++) {
    input->prm.lag[0][i]=prm->lag[i][0];
    input->prm.lag[1][i]=prm->lag[i][1];
  }

  for (i=0;i<input->prm.nrang;i++) {
    input->prm.pwr0[i]=raw->pwr0[i];
   
    for (j=0;j<input->prm.mplgs;j++) {
      input->acfd[i][j].x=raw->acfd[i][j][0];
      input->acfd[i][j].y=raw->acfd[i][j][1];
    }
    if (input->prm.xcf) {
      for (j=0;j<input->prm.mplgs;j++) {
        input->xcfd[i][j].x=raw->xcfd[i][j][0];
        input->xcfd[i][j].y=raw->xcfd[i][j][1];
      }
    } 
  }   
  goose=((prm->stid)==GOOSEBAY);
  fnum=do_fit(input,5,goose,fit->rng,fit->xrng,fit->elv,&fit->noise);
}
