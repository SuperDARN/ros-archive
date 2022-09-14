/* rawwrite.c
   ========== 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"



int RawWrite(int fid,struct RadarParm *prm,
             struct RawData *raw) {

  int s,c,d;
  int32 p0num,snum,pnum,lnum[2];
  int32 anum[3],xnum[3];
  struct DataMap *data;

  char *cmbptr;
  char *cmdptr;
  char *tmeptr;


  int16 lag[LAG_SIZE*2];
  int16 slist[MAX_RANGE];
  float pwr0[MAX_RANGE];
  float acfd[MAX_RANGE*LAG_SIZE*2];
  float xcfd[MAX_RANGE*LAG_SIZE*2];
  float tx;  

  data=DataMapMake();

  DataMapAddScalar(data,"radar.revision.major",DATACHAR,&prm->revision.major);
  DataMapAddScalar(data,"radar.revision.minor",DATACHAR,&prm->revision.minor);

  tmeptr=prm->origin.time;
  cmdptr=prm->origin.command;

  DataMapAddScalar(data,"origin.code",DATACHAR,&prm->origin.code);   
  DataMapAddScalar(data,"origin.time",DATASTRING,&tmeptr);
  DataMapAddScalar(data,"origin.command",DATASTRING,&cmdptr);

  DataMapAddScalar(data,"cp",DATASHORT,&prm->cp);
  DataMapAddScalar(data,"stid",DATASHORT,&prm->stid);
  DataMapAddScalar(data,"time.yr",DATASHORT,&prm->time.yr);
  DataMapAddScalar(data,"time.mo",DATASHORT,&prm->time.mo);
  DataMapAddScalar(data,"time.dy",DATASHORT,&prm->time.dy);
  DataMapAddScalar(data,"time.hr",DATASHORT,&prm->time.hr);
  DataMapAddScalar(data,"time.mt",DATASHORT,&prm->time.mt);
  DataMapAddScalar(data,"time.sc",DATASHORT,&prm->time.sc);
  DataMapAddScalar(data,"time.us",DATAINT,&prm->time.us);
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
  DataMapAddScalar(data,"bmazm",DATAFLOAT,&prm->bmazm);

  DataMapAddScalar(data,"scan",DATASHORT,&prm->scan);
  DataMapAddScalar(data,"offset",DATASHORT,&prm->offset);
  DataMapAddScalar(data,"rxrise",DATASHORT,&prm->rxrise);
  DataMapAddScalar(data,"intt.sc",DATASHORT,&prm->intt.sc);
  DataMapAddScalar(data,"intt.us",DATAINT,&prm->intt.us);

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

  DataMapAddScalar(data,"rawacf.revision.major",DATAINT,
		    &raw->revision.major);

  DataMapAddScalar(data,"rawacf.revision.minor",DATAINT,
		    &raw->revision.minor);

  cmbptr=prm->combf;
  
  DataMapAddScalar(data,"combf",DATASTRING,&cmbptr);
 
  DataMapAddArray(data,"ptab",DATASHORT,1,&pnum,prm->pulse);
  DataMapAddArray(data,"ltab",DATASHORT,2,lnum,lag);

  DataMapAddScalar(data,"thr",DATAFLOAT,&raw->thr);

  DataMapAddArray(data,"slist",DATASHORT,1,&snum,slist);
  DataMapAddArray(data,"pwr0",DATAFLOAT,1,&p0num,pwr0);
 
  snum=0;
  for (c=0;c<=prm->mplgs;c++) {
    lag[2*c]=prm->lag[c][0];
    lag[2*c+1]=prm->lag[c][1];
  }
  tx=(int) ((raw->thr*prm->noise.search)/2.0);
  for (c=0;c<prm->nrang;c++) {
    pwr0[c]=raw->pwr0[c];
    if (raw->pwr0[c]<tx) continue;
    slist[snum]=c;
    for (d=0;d<prm->mplgs;d++) {
      acfd[snum*prm->mplgs*2+2*d]=raw->acfd[c][d][0];
      acfd[snum*prm->mplgs*2+2*d+1]=raw->acfd[c][d][1];
    }
    if (prm->xcf !=0) for (d=0;d<prm->mplgs;d++) {
      xcfd[snum*prm->mplgs*2+2*d]=raw->xcfd[c][d][0];
      xcfd[snum*prm->mplgs*2+2*d+1]=raw->xcfd[c][d][1];
    }
    snum++; 
  }


  
  pnum=prm->mppul;
  lnum[0]=2;
  lnum[1]=prm->mplgs+1;
  
  p0num=prm->nrang;

  anum[0]=2;
  anum[1]=prm->mplgs;
  anum[2]=snum;
 
  if (prm->xcf !=0) {
    xnum[0]=2;
    xnum[1]=prm->mplgs;
    xnum[2]=snum;
  } else {
    xnum[0]=0;
    xnum[1]=0;
    xnum[2]=0;
  }

  if (snum !=0) {
    DataMapAddArray(data,"acfd",DATAFLOAT,3,anum,acfd);
    if (prm->xcf==1) DataMapAddArray(data,"xcfd",DATAFLOAT,3,xnum,xcfd);
  }

  if (fid !=-1) s=DataMapWrite(fid,data);
  else s=DataMapSize(data);

  DataMapFree(data);

  return s;
}



int RawFwrite(FILE *fp,struct RadarParm *prm,
              struct RawData *raw) {
  return RawWrite(fileno(fp),prm,raw);
}
