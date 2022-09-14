/* fitpacket.c
   =========== 
   Author: R.J.Barnes
*/


/*
 $License$
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"

unsigned char *fitpacket(struct RadarParm *prm,
              struct FitData *fit,int *size) {

  int c;
  int sze=0;
  unsigned char *buf;

  struct DataMap *data;
  
  float sky_noise;
  float lag0_noise;
  float vel_noise;

  char *cptr;

  int32 pnum;
  int32 lnum[2];
  int32 snum;
  int32 p0num;

  int16 lag[LAG_SIZE*2];

  float pwr0[MAX_RANGE];
  int16 slist[MAX_RANGE];

  char gflg[MAX_RANGE];

  float p_l[MAX_RANGE];
  float v[MAX_RANGE];
  float v_e[MAX_RANGE];
  float w_l[MAX_RANGE];
 
  float elv[MAX_RANGE];
  float elv_low[MAX_RANGE];
  float elv_high[MAX_RANGE];


  data=DataMapMake();
 
  DataMapAddScalar(data,"radar.revision.major",DATACHAR,&prm->revision.major);
  DataMapAddScalar(data,"radar.revision.minor",DATACHAR,&prm->revision.minor);
  DataMapAddScalar(data,"cp",DATASHORT,&prm->cp);
  DataMapAddScalar(data,"stid",DATASHORT,&prm->stid);
  DataMapAddScalar(data,"time.yr",DATASHORT,&prm->time.yr);
  DataMapAddScalar(data,"time.mo",DATASHORT,&prm->time.mo);
  DataMapAddScalar(data,"time.dy",DATASHORT,&prm->time.dy);
  DataMapAddScalar(data,"time.hr",DATASHORT,&prm->time.hr);
  DataMapAddScalar(data,"time.mt",DATASHORT,&prm->time.mt);
  DataMapAddScalar(data,"time.sc",DATASHORT,&prm->time.sc);
  DataMapAddScalar(data,"time.us",DATASHORT,&prm->time.us);
  DataMapAddScalar(data,"txpow",DATASHORT,&prm->txpow);
  DataMapAddScalar(data,"nave",DATASHORT,&prm->nave);
  DataMapAddScalar(data,"atten",DATASHORT,&prm->atten);
  DataMapAddScalar(data,"lagfr",DATASHORT,&prm->lagfr);
  DataMapAddScalar(data,"smsep",DATASHORT,&prm->smsep);
  DataMapAddScalar(data,"ercod",DATASHORT,&prm->ercod);
  DataMapAddScalar(data,"stat.agc",DATASHORT,&prm->stat.agc);
  DataMapAddScalar(data,"stat.lopwr",DATASHORT,&prm->stat.lopwr);
  DataMapAddScalar(data,"noise.search",DATAFLOAT,&prm->noise.search);
  DataMapAddScalar(data,"noise.mean",DATAFLOAT,&prm->noise.mean);

  DataMapAddScalar(data,"channel",DATASHORT,&prm->channel);
  DataMapAddScalar(data,"bmnum",DATASHORT,&prm->bmnum);
  DataMapAddScalar(data,"scan",DATASHORT,&prm->scan);
  DataMapAddScalar(data,"offset",DATASHORT,&prm->offset);
  DataMapAddScalar(data,"rxrise",DATASHORT,&prm->rxrise);
  DataMapAddScalar(data,"intt.sc",DATASHORT,&prm->intt.sc);
  DataMapAddScalar(data,"intt.us",DATASHORT,&prm->intt.us);

  DataMapAddScalar(data,"txpl",DATASHORT,&prm->txpl);
  DataMapAddScalar(data,"mpinc",DATASHORT,&prm->mpinc);
  DataMapAddScalar(data,"mppul",DATASHORT,&prm->mppul);
  DataMapAddScalar(data,"mplgs",DATASHORT,&prm->mplgs);

  DataMapAddScalar(data,"nrang",DATASHORT,&prm->nrang);
  DataMapAddScalar(data,"frang",DATASHORT,&prm->frang);
  DataMapAddScalar(data,"rsep",DATASHORT,&prm->rsep);
  DataMapAddScalar(data,"xcf",DATASHORT,&prm->xcf);
  DataMapAddScalar(data,"tfreq",DATASHORT,&prm->tfreq);

  DataMapAddScalar(data,"mxpwr",DATAINT,&prm->mxpwr);
  DataMapAddScalar(data,"lvmax",DATAINT,&prm->lvmax);

  DataMapAddScalar(data,"fitacf.revision.major",DATAINT,
		    &fit->revision.major);
  DataMapAddScalar(data,"fitacf.revision.minor",DATAINT,
		    &fit->revision.minor);

  sky_noise=fit->noise.skynoise;
  lag0_noise=fit->noise.lag0;
  vel_noise=fit->noise.vel;

  cptr=prm->combf;
  
  DataMapAddScalar(data,"combf",DATASTRING,&cptr);

  DataMapAddScalar(data,"noise.sky",DATAFLOAT,&sky_noise);
  DataMapAddScalar(data,"noise.lag0",DATAFLOAT,&lag0_noise);
  DataMapAddScalar(data,"noise.vel",DATAFLOAT,&vel_noise);

  DataMapAddArray(data,"ptab",DATASHORT,1,&pnum,prm->pulse);
  DataMapAddArray(data,"ltab",DATASHORT,2,lnum,lag);

  DataMapAddArray(data,"pwr0",DATAFLOAT,1,&p0num,pwr0);

  pnum=prm->mppul;
  lnum[0]=2;
  lnum[1]=prm->mplgs;

  p0num=prm->nrang;

  for (c=0;c<prm->mplgs;c++) {
    lag[2*c]=prm->lag[c][0];
    lag[2*c+1]=prm->lag[c][1];
  }

  for (c=0;c<p0num;c++) pwr0[c]=fit->rng[c].p_0;
  snum=0;
  for (c=0;c<prm->nrang;c++) {
    if (fit->rng[c].qflg==1) {
      slist[snum]=c;
      gflg[snum]=fit->rng[c].gsct;
      p_l[snum]=fit->rng[c].p_l;
      v[snum]=fit->rng[c].v;
      v_e[snum]=fit->rng[c].v_err;
      w_l[snum]=fit->rng[c].w_l;
      elv[snum]=fit->elv[c].normal;
      elv_low[snum]=fit->elv[c].low;
      elv_high[snum]=fit->elv[c].high;
      snum++;
    }
  }
  if (snum !=0) {
    DataMapAddArray(data,"slist",DATASHORT,1,&snum,slist);
    DataMapAddArray(data,"gflg",DATACHAR,1,&snum,gflg); 
    DataMapAddArray(data,"p_l",DATAFLOAT,1,&snum,p_l);   
    DataMapAddArray(data,"w_l",DATAFLOAT,1,&snum,w_l); 
    DataMapAddArray(data,"v",DATAFLOAT,1,&snum,v); 
    DataMapAddArray(data,"v_e",DATAFLOAT,1,&snum,v_e); 
    if (prm->xcf !=0) {
      DataMapAddArray(data,"elv",DATAFLOAT,1,&snum,elv); 
      DataMapAddArray(data,"elv_low",DATAFLOAT,1,&snum,elv_low); 
      DataMapAddArray(data,"elv_high",DATAFLOAT,1,&snum,elv_high); 
    }
  }
  buf=DataMapEncodeBuffer(data,&sze);
  if (size !=NULL) *size=sze;
  DataMapFree(data);
  return buf;
}
