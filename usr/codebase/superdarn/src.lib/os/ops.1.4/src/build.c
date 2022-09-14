/* build.c
   =======
   Author: R.J.Barnes, J.Spaleta
*/

/*
 $License$
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "iqdata.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "global.h"
#include "rosversion.h"


void OpsBuildPrm(struct RadarParm *prm,int *ptab,int (*lags)[2]) {
  int i,j;
  time_t ctime;
  float offset;
  
  prm->revision.major = MAJOR_VERSION;
  prm->revision.minor = MINOR_VERSION;

  prm->origin.code=0;
  
  ctime = time((time_t) 0);
  strcpy(prm->origin.command, (char *) command);
  strcpy(prm->origin.time,asctime(gmtime(&ctime)));
  prm->origin.time[24]=0;

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

  offset=site->maxbeam/2.0-0.5;
  prm->bmazm=site->boresite+site->bmsep*(bmnum-offset);

  prm->xcf   = xcf;
  prm->tfreq = tfreq;
  prm->scan  = scan;
  prm->mxpwr = mxpwr;
  prm->lvmax = lvmax;
  prm->cp	= cp;

  prm->stat.agc=agcstat;
  prm->stat.lopwr=lopwrstat;

  for (i=0;i<mppul;i++) prm->pulse[i]=ptab[i];

  if (lagnum !=0) {
    int otau=-1;
    int tau=0;
    int c=0;
    for (j=0;j<=lagnum;j++) {
      tau=abs(lags[j][0]-lags[j][1]);
      if (tau==otau) continue;
      prm->lag[c][0]=lags[j][0];
      prm->lag[c][1]=lags[j][1];
      otau=tau;
      c++;
    }
  } else {
    for (i=0;i<2;i++) {
      for (j=0;j<=mplgs;j++) prm->lag[j][i]=lags[j][i];
    } 
  }

  strncpy(prm->combf,combf,COMBF_SIZE);
}

void OpsBuildIQ(struct IQData *iq,unsigned int **badtr) {

  int i,badtrnum=0,badtroff=0;
  char *badtradr=NULL;
  iq->seqnum=nave;
  iq->chnnum=rxchn;
  iq->smpnum=smpnum;  
  iq->skpnum=skpnum;
  
  for (i=0;i<nave;i++) { 
    iq->tval[i].tv_sec=seqtval[i].tv_sec;
    iq->tval[i].tv_nsec=seqtval[i].tv_nsec;
    iq->atten[i]=seqatten[i];
    iq->noise[i]=seqnoise[i];
    iq->offset[i]=seqoff[i];
    iq->size[i]=seqsze[i];
    iq->badtr[i]=seqbadtr[i].num;
    badtrnum+=seqbadtr[i].num;
  }
  iq->tbadtr=badtrnum;
  if (*badtr !=NULL) free(*badtr);
  badtradr=malloc(sizeof(unsigned int)*2*badtrnum);
  *badtr=(unsigned int *) badtradr;

  for (i=0;i<nave;i++) { 
    memcpy(badtradr+badtroff,seqbadtr[i].start,
           sizeof(unsigned int)*seqbadtr[i].num); 
    badtroff+= sizeof(unsigned int)*seqbadtr[i].num; 
    memcpy(badtradr+badtroff,seqbadtr[i].length,
           sizeof(unsigned int)*seqbadtr[i].num); 
    badtroff+= sizeof(unsigned int)*seqbadtr[i].num; 
  }
}




void OpsBuildRaw(struct RawData *raw) {
  int i,j;
  
  for (i=0;i<nrang;i++) {
    raw->pwr0[i]=pwr0[i];
  }
  for (i=0;i<nrang;i++) {
    for (j=0;j<mplgs;j++) {
      
      raw->acfd[i][j][0]=acfd[(i*mplgs*2)+(2*j)];
      raw->acfd[i][j][1]=acfd[(i*mplgs*2)+(2*j)+1];
      raw->xcfd[i][j][0]=xcfd[(i*mplgs*2)+(2*j)];
      raw->xcfd[i][j][1]=xcfd[(i*mplgs*2)+(2*j)+1];

    }
  }
}

