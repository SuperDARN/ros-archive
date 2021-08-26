/* fitread.c
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
#include "fitblk.h"
#include "fitdata.h"


/*
 $Log: fitread.c,v $
 Revision 1.7  2006/01/31 19:26:08  barnes
 Fixed bug in recording the time.

 Revision 1.6  2006/01/06 18:15:23  barnes
 Changed microseconds to long integer.

 Revision 1.5  2005/01/31 17:01:11  barnes
 Put in code to recover the quality flag if it is missing from the feed.

 Revision 1.4  2004/10/27 22:08:30  barnes
 Created the FitDecode function that can decode a DataMap structure from
 either a file or from the TCP/IP stream.

 Revision 1.3  2004/06/22 17:39:34  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 18:26:11  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/01/25 23:25:46  barnes
 Initial revision

*/

int FitDecode(struct DataMap *ptr,struct RadarParm *prm,
              struct FitData *fit) {
  int c,x;
  int snum=0;
  int16 slist[MAX_RANGE];
  struct DataMapScalar *s;
  struct DataMapArray *a;

  int qflg=0,xqflg=0;

  for (c=0;c<MAX_RANGE;c++) {
    slist[c]=0;
    fit->rng[c].qflg=0;
    fit->xrng[c].qflg=0;
    fit->rng[c].gsct=0;
    fit->xrng[c].gsct=0;

  }
  memset(prm,0,sizeof(struct RadarParm));
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
   
    if ((strcmp(s->name,"radar.revision.major")==0) && (s->type==DATACHAR))
      prm->revision.major=*(s->data.cptr);
    if ((strcmp(s->name,"radar.revision.minor")==0) && (s->type==DATACHAR))
      prm->revision.minor=*(s->data.cptr);
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
      prm->lvmax=*(s->data.sptr);

    if ((strcmp(s->name,"fitacf.revision.major")==0) && (s->type==DATAINT))
      fit->revision.major=*(s->data.lptr);
    if ((strcmp(s->name,"fitacf.revision.minor")==0) && (s->type==DATAINT))
      fit->revision.minor=*(s->data.sptr);

    if ((strcmp(s->name,"combf")==0) && (s->type==DATASTRING))
      strncpy(prm->combf,*((char **) s->data.vptr),COMBF_SIZE-1);

    if ((strcmp(s->name,"noise.sky")==0) && (s->type==DATAFLOAT))
      fit->noise.skynoise=*(s->data.fptr);
    if ((strcmp(s->name,"noise.lag0")==0) && (s->type==DATAFLOAT))
      fit->noise.lag0=*(s->data.fptr);
    if ((strcmp(s->name,"noise.vel")==0) && (s->type==DATAFLOAT))
      fit->noise.vel=*(s->data.fptr);


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

    if ((strcmp(a->name,"pwr0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[x].p_0=a->data.fptr[x];
    }
    if ((strcmp(a->name,"slist")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) slist[x]=a->data.sptr[x];
      snum=a->rng[0];
    }
  }
  
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    if ((strcmp(a->name,"nlag")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].nump=a->data.sptr[x];
    }
    
    if ((strcmp(a->name,"qflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      qflg=1;
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].qflg=a->data.cptr[x];
    }

    if ((strcmp(a->name,"gflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].gsct=a->data.cptr[x];
    }

    if ((strcmp(a->name,"p_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].p_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"p_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->rng[slist[x]].p_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"p_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].p_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"p_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->rng[slist[x]].p_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"v")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].v=a->data.fptr[x];
    }
    if ((strcmp(a->name,"v_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].v_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"w_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].w_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"w_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->rng[slist[x]].w_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"w_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->rng[slist[x]].w_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"w_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->rng[slist[x]].w_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"sd_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->rng[slist[x]].sdev_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"sd_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->rng[slist[x]].sdev_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"sd_phi")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
	fit->rng[slist[x]].sdev_phi=a->data.fptr[x];
    }
    
    

    if ((strcmp(a->name,"x_qflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) { 
      xqflg=1;
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].qflg=a->data.cptr[x];
    }
   
    if ((strcmp(a->name,"x_gflg")==0) && (a->type==DATACHAR) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].gsct=a->data.cptr[x];
    }
    
    if ((strcmp(a->name,"x_p_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].p_l=a->data.fptr[x];
    }
    
    if ((strcmp(a->name,"x_p_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].p_l_err=a->data.fptr[x];
    }
    
    if ((strcmp(a->name,"x_p_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].p_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_p_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
         fit->xrng[slist[x]].p_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_v")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].v=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_v_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].v_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_w_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].w_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_w_l_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].w_l_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_w_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].w_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_w_s_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].w_s_err=a->data.fptr[x];
    }

    if ((strcmp(a->name,"phi0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->xrng[slist[x]].phi0=a->data.fptr[x];
    }

    if ((strcmp(a->name,"phi0_e")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].phi0_err=a->data.fptr[x];
    }


    if ((strcmp(a->name,"elv")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->elv[slist[x]].normal=a->data.fptr[x];
    }

    if ((strcmp(a->name,"elv_low")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->elv[slist[x]].low=a->data.fptr[x];
    }

    if ((strcmp(a->name,"elv_high")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) fit->elv[slist[x]].high=a->data.fptr[x];
    }

    if ((strcmp(a->name,"x_sd_l")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].sdev_l=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_sd_s")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].sdev_s=a->data.fptr[x];
    }
    if ((strcmp(a->name,"x_sd_phi")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      for (x=0;x<a->rng[0];x++) 
        fit->xrng[slist[x]].sdev_phi=a->data.fptr[x];
    }
    
  }
 
  if (qflg==0) for (x=0;x<snum;x++) fit->rng[slist[x]].qflg=1;
  if (xqflg==0) for (x=0;x<snum;x++) fit->xrng[slist[x]].qflg=1;


  return 0;

}


int FitRead(int fid,struct RadarParm *prm,
              struct FitData *fit) {
  int s;
  struct DataMap *ptr;

  ptr=DataMapRead(fid);
  if (ptr==NULL) return -1;
  s=FitDecode(ptr,prm,fit);
  DataMapFree(ptr);
  return s;
}


int FitFread(FILE *fp,struct RadarParm *prm,
              struct FitData *fit) {
  return FitRead(fileno(fp),prm,fit);
}

