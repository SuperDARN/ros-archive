/* iqwrite.c
   ========= 
   Author R.J.Barnes
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
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "iqdata.h"


/*
 $Log: iqwrite.c,v $
 Revision 1.7  2008/03/25 15:32:45  code
 Fixed problem with reading files with empty records.

 Revision 1.6  2008/03/14 20:05:54  code
 Fixed bug in writing out the sample data.

 Revision 1.5  2008/03/14 18:03:36  code
 Added missing header.

 Revision 1.4  2008/03/14 16:40:57  code
 Fixed typo.

 Revision 1.3  2008/03/14 16:15:37  code
 Added a variable that indicates number of samples to skip before performin calculations.

 Revision 1.2  2008/03/13 19:22:18  code
 Changed format to include receiver channel.

 Revision 1.1  2008/03/13 17:33:02  code
 Initial revision

*/



int IQWrite(int fid,struct RadarParm *prm,
              struct IQData *iqdata,int16 *samples) {

  int s,c;
  struct DataMap *data;
 
  char *cmbptr;
  char *cmdptr;
  char *tmeptr;


  int32 pnum;
  int32 lnum[2];
  int32 tnum,snum;
  int32 seqnum,smpnum,skpnum,chnnum;

  int16 lag[LAG_SIZE*2];

  int32 tsc[MAXNAVE];
  int32 tus[MAXNAVE];
  int16 tatten[MAXNAVE];
  float tnoise[MAXNAVE];
  int32 toff[MAXNAVE];
  int32 tsze[MAXNAVE];

  
  seqnum=iqdata->seqnum;
  chnnum=iqdata->chnnum;
  smpnum=iqdata->smpnum;
  skpnum=iqdata->skpnum;

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

  DataMapAddScalar(data,"iqdata.revision.major",DATAINT,
		    &iqdata->revision.major);
  DataMapAddScalar(data,"iqdata.revision.minor",DATAINT,
		    &iqdata->revision.minor);



  cmbptr=prm->combf;
  
  DataMapAddScalar(data,"combf",DATASTRING,&cmbptr);

  DataMapAddScalar(data,"seqnum",DATAINT,&seqnum);
  DataMapAddScalar(data,"chnnum",DATAINT,&chnnum);
  DataMapAddScalar(data,"smpnum",DATAINT,&smpnum);
  DataMapAddScalar(data,"skpnum",DATAINT,&skpnum);


  DataMapAddArray(data,"ptab",DATASHORT,1,&pnum,prm->pulse);
  DataMapAddArray(data,"ltab",DATASHORT,2,lnum,lag);

  pnum=prm->mppul;
  lnum[0]=2;
  lnum[1]=prm->mplgs+1;

  for (c=0;c<=prm->mplgs;c++) {
    lag[2*c]=prm->lag[c][0];
    lag[2*c+1]=prm->lag[c][1];
  }

  if (iqdata->seqnum !=0) {
    DataMapAddArray(data,"tsc",DATAINT,1,&tnum,tsc);
    DataMapAddArray(data,"tus",DATAINT,1,&tnum,tus);
    DataMapAddArray(data,"tatten",DATASHORT,1,&tnum,tatten);
    DataMapAddArray(data,"tnoise",DATAFLOAT,1,&tnum,tnoise);
    DataMapAddArray(data,"toff",DATAINT,1,&tnum,toff);
    DataMapAddArray(data,"tsze",DATAINT,1,&tnum,tsze);

    DataMapAddArray(data,"data",DATASHORT,1,&snum,samples);

    tnum=iqdata->seqnum;
    snum=iqdata->seqnum*iqdata->chnnum*iqdata->smpnum*2;  

    /* each sample is an I&Q pair of 16 bit numbers */

    for (c=0;c<tnum;c++) {
      tsc[c]=(int) iqdata->tval[c].tv_sec;
      tus[c]=(int) iqdata->tval[c].tv_nsec/1000.0;
      tatten[c]=iqdata->atten[c];
      tnoise[c]=iqdata->noise[c];
      toff[c]=iqdata->offset[c];
      tsze[c]=iqdata->size[c];
    }
  }
  if (fid !=-1) s=DataMapWrite(fid,data);
  else s=DataMapSize(data);
  DataMapFree(data);
  return s;
}


int IQFwrite(FILE *fp,struct RadarParm *prm,
              struct IQData *iqdata,int16 *samples) {
  return IQWrite(fileno(fp),prm,iqdata,samples);
}


