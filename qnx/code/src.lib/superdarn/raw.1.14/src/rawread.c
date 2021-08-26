/* rawread.c
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"

/*
 $Log: rawread.c,v $
 Revision 1.7  2006/07/11 15:49:02  code
 Modification to include beam azimuth and origin data.

 Revision 1.6  2006/01/31 19:27:10  barnes
 Zero the paramter block.

 Revision 1.5  2006/01/06 17:01:03  barnes
 Changed microseconds to long integers.

 Revision 1.4  2004/06/22 17:42:20  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/05/04 18:37:48  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:30:56  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/25 23:26:44  barnes
 Initial revision

*/

int RawRead(int fid,struct RadarParm *prm,
              struct RawData *raw) {

  int c,x,y,n;

  int16 slist[MAX_RANGE];
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;

  ptr=DataMapRead(fid);

  if (ptr==NULL) return -1;

  for (c=0;c<MAX_RANGE;c++) {
     slist[c]=0;
     raw->pwr0[c]=0;
     for (x=0;x<LAG_SIZE;x++) {
       raw->acfd[c][x][0]=0;
       raw->acfd[c][x][1]=0;
       raw->xcfd[c][x][0]=0;
       raw->xcfd[c][x][1]=0;
     }
  }
  memset(prm,0,sizeof(struct RadarParm));
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
   
    if ((strcmp(s->name,"radar.revision.major")==0) && (s->type==DATACHAR))
      prm->revision.major=*(s->data.cptr);
    if ((strcmp(s->name,"radar.revision.minor")==0) && (s->type==DATACHAR))
      prm->revision.minor=*(s->data.cptr);

    if ((strcmp(s->name,"origin.code")==0) && (s->type==DATACHAR))
      prm->origin.code=*(s->data.cptr);

    if ((strcmp(s->name,"origin.time")==0) && (s->type==DATASTRING))
      strncpy(prm->origin.time,*((char **) s->data.vptr),ORIGIN_TIME_SIZE);

    if ((strcmp(s->name,"origin.command")==0) && (s->type==DATASTRING))
      strncpy(prm->origin.command,*((char **) s->data.vptr),
              ORIGIN_COMMAND_SIZE);


    if ((strcmp(s->name,"cp")==0) && (s->type==DATASHORT))
      prm->cp=*(s->data.sptr);
    if ((strcmp(s->name,"stid")==0) && (s->type==DATASHORT))
      prm->stid=*(s->data.sptr);
    if ((strcmp(s->name,"time.yr")==0) && (s->type==DATASHORT))
      prm->time.yr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mo")==0) && (s->type==DATASHORT))
      prm->time.mo=*(s->data.sptr);
    if ((strcmp(s->name,"time.dy")==0) && (s->type==DATASHORT))
      prm->time.dy=*(s->data.sptr);
    if ((strcmp(s->name,"time.hr")==0) && (s->type==DATASHORT))
      prm->time.hr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mt")==0) && (s->type==DATASHORT))
      prm->time.mt=*(s->data.sptr);
    if ((strcmp(s->name,"time.sc")==0) && (s->type==DATASHORT))
      prm->time.sc=*(s->data.sptr);
    if ((strcmp(s->name,"time.us")==0) && (s->type==DATAINT))
      prm->time.us=*(s->data.lptr);
    if ((strcmp(s->name,"txpow")==0) && (s->type==DATASHORT))
      prm->txpow=*(s->data.sptr);
    if ((strcmp(s->name,"nave")==0) && (s->type==DATASHORT))
      prm->nave=*(s->data.sptr);
    if ((strcmp(s->name,"atten")==0) && (s->type==DATASHORT))
      prm->atten=*(s->data.sptr);
    if ((strcmp(s->name,"lagfr")==0) && (s->type==DATASHORT))
      prm->lagfr=*(s->data.sptr);
    if ((strcmp(s->name,"smsep")==0) && (s->type==DATASHORT))
      prm->smsep=*(s->data.sptr);
    if ((strcmp(s->name,"ercod")==0) && (s->type==DATASHORT))
      prm->ercod=*(s->data.sptr);
    if ((strcmp(s->name,"stat.agc")==0) && (s->type==DATASHORT))
      prm->stat.agc=*(s->data.sptr);
    if ((strcmp(s->name,"stat.lopwr")==0) && (s->type==DATASHORT))
      prm->stat.lopwr=*(s->data.sptr);
    if ((strcmp(s->name,"noise.search")==0) && (s->type==DATAFLOAT))
      prm->noise.search=*(s->data.fptr);
    if ((strcmp(s->name,"noise.mean")==0) && (s->type==DATAFLOAT))
      prm->noise.mean=*(s->data.fptr);
    if ((strcmp(s->name,"channel")==0) && (s->type==DATASHORT))
      prm->channel=*(s->data.sptr);
    if ((strcmp(s->name,"bmnum")==0) && (s->type==DATASHORT))
      prm->bmnum=*(s->data.sptr);
    if ((strcmp(s->name,"bmazm")==0) && (s->type==DATAFLOAT))
      prm->bmazm=*(s->data.fptr);
    if ((strcmp(s->name,"scan")==0) && (s->type==DATASHORT))
      prm->scan=*(s->data.sptr);
   if ((strcmp(s->name,"offset")==0) && (s->type==DATASHORT))
      prm->offset=*(s->data.sptr);
    if ((strcmp(s->name,"rxrise")==0) && (s->type==DATASHORT))
      prm->rxrise=*(s->data.sptr);
    if ((strcmp(s->name,"intt.sc")==0) && (s->type==DATASHORT))
      prm->intt.sc=*(s->data.sptr);
    if ((strcmp(s->name,"intt.us")==0) && (s->type==DATAINT))
      prm->intt.us=*(s->data.lptr);
    if ((strcmp(s->name,"txpl")==0) && (s->type==DATASHORT))
      prm->txpl=*(s->data.sptr);
    if ((strcmp(s->name,"mpinc")==0) && (s->type==DATASHORT))
      prm->mpinc=*(s->data.sptr);
    if ((strcmp(s->name,"mppul")==0) && (s->type==DATASHORT))
      prm->mppul=*(s->data.sptr);
    if ((strcmp(s->name,"mplgs")==0) && (s->type==DATASHORT))
      prm->mplgs=*(s->data.sptr);
    if ((strcmp(s->name,"nrang")==0) && (s->type==DATASHORT))
      prm->nrang=*(s->data.sptr);
    if ((strcmp(s->name,"frang")==0) && (s->type==DATASHORT))
      prm->frang=*(s->data.sptr);
    if ((strcmp(s->name,"rsep")==0) && (s->type==DATASHORT))
      prm->rsep=*(s->data.sptr);
    if ((strcmp(s->name,"xcf")==0) && (s->type==DATASHORT))
      prm->xcf=*(s->data.sptr);
    if ((strcmp(s->name,"tfreq")==0) && (s->type==DATASHORT))
      prm->tfreq=*(s->data.sptr);

    if ((strcmp(s->name,"mxpwr")==0) && (s->type==DATAINT))
      prm->mxpwr=*(s->data.lptr);
    if ((strcmp(s->name,"lvmax")==0) && (s->type==DATAINT))
      prm->lvmax=*(s->data.lptr);

    if ((strcmp(s->name,"rawacf.revision.major")==0) && (s->type==DATAINT))
      raw->revision.major=*(s->data.lptr);
    if ((strcmp(s->name,"rawacf.revision.minor")==0) && (s->type==DATAINT))
      raw->revision.minor=*(s->data.lptr);

    if ((strcmp(s->name,"combf")==0) && (s->type==DATASTRING))
      strncpy(prm->combf,*((char **) s->data.vptr),COMBF_SIZE-1);

    if ((strcmp(s->name,"thr")==0) && (s->type==DATAFLOAT))
      raw->thr=*(s->data.fptr);
  }
 
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
   if ((strcmp(a->name,"ptab")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) prm->pulse[x]=a->data.sptr[x];
    }
    if ((strcmp(a->name,"ltab")==0) && (a->type==DATASHORT) &&
        (a->dim==2)) {
      for (x=0;x<a->rng[1];x++) {
        prm->lag[x][0]=a->data.sptr[2*x];
        prm->lag[x][1]=a->data.sptr[2*x+1];
      }
    }
  
    if ((strcmp(a->name,"slist")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) slist[x]=a->data.sptr[x];
    }
    if ((strcmp(a->name,"pwr0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) raw->pwr0[x]=a->data.fptr[x];
    }
  }

  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    
    if ((strcmp(a->name,"acfd")==0) && (a->type==DATAFLOAT) &&
        (a->dim==3)) {
      n=0;
      for (x=0;x<a->rng[2];x++) {
        for (y=0;y<a->rng[1];y++) {
          raw->acfd[slist[x]][y][0]=a->data.fptr[n];
          raw->acfd[slist[x]][y][1]=a->data.fptr[n+1];
          n+=2;
	}
      }
    }

    if ((strcmp(a->name,"xcfd")==0) && (a->type==DATAFLOAT) &&
        (a->dim==3)) {
      n=0;
      for (x=0;x<a->rng[2];x++) {
        for (y=0;y<a->rng[1];y++) {
          raw->xcfd[slist[x]][y][0]=a->data.fptr[n];
          raw->xcfd[slist[x]][y][1]=a->data.fptr[n+1];
          n+=2;
	}
      }
    }



  }

  DataMapFree(ptr);

  return 0;

}

int RawFread(FILE *fp,struct RadarParm *prm,
              struct RawData *raw) {
  return RawRead(fileno(fp),prm,raw);
}
