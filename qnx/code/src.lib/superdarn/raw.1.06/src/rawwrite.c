/* rawwrite.c
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"

/*
 $Log: rawwrite.c,v $
 Revision 1.3  2004/06/22 17:42:20  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 18:37:48  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/01/24 19:59:28  barnes
 Initial revision

*/

int RawWrite(int fid,struct RadarParm *prm,
             struct RawData *raw) {

  int s,c,d;
  int32 p0num,snum,pnum,lnum[2];
  int32 anum[3],xnum[3];
  struct DataMap *data;

  char *cptr;

  int16 lag[LAG_SIZE*2];
  int16 slist[MAX_RANGE];
  float pwr0[MAX_RANGE];
  float acfd[MAX_RANGE*LAG_SIZE*2];
  float xcfd[MAX_RANGE*LAG_SIZE*2];
  float tx;  

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

  DataMapAddScalar(data,"rawacf.revision.major",DATAINT,
		    &raw->revision.major);

  DataMapAddScalar(data,"rawacf.revision.minor",DATAINT,
		    &raw->revision.minor);



  cptr=prm->combf;
  
  DataMapAddScalar(data,"combf",DATASTRING,&cptr);
 
  DataMapAddArray(data,"ptab",DATASHORT,1,&pnum,prm->pulse);
  DataMapAddArray(data,"ltab",DATASHORT,2,lnum,lag);

  DataMapAddScalar(data,"thr",DATAFLOAT,&raw->thr);

  DataMapAddArray(data,"slist",DATASHORT,1,&snum,slist);
  DataMapAddArray(data,"pwr0",DATAFLOAT,1,&p0num,pwr0);
  DataMapAddArray(data,"acfd",DATAFLOAT,3,anum,acfd);
  DataMapAddArray(data,"xcfd",DATAFLOAT,3,xnum,xcfd);

  snum=0;
  for (c=0;c<prm->mplgs;c++) {
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
  lnum[1]=prm->mplgs;
  
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

  if (fid !=-1) s=DataMapWrite(fid,data);
  else s=DataMapSize(data);
  DataMapFree(data);

  return s;
}



int RawFwrite(FILE *fp,struct RadarParm *prm,
              struct RawData *raw) {
  return RawWrite(fileno(fp),prm,raw);
}
