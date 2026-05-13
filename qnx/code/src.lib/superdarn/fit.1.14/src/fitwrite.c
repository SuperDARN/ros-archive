/* fitwrite.c
   ========== 
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
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitblk.h"
#include "fitdata.h"

/*
 $Log: fitwrite.c,v $
 Revision 1.5  2006/01/06 18:15:23  barnes
 Changed microseconds to long integer.

 Revision 1.4  2004/08/03 22:07:36  barnes
 Modification to not add empty variables to the file.

 Revision 1.3  2004/06/22 17:39:34  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 18:26:11  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/01/24 19:54:35  barnes
 Initial revision

*/

int FitWrite(int fid,struct RadarParm *prm,
              struct FitData *fit) {

  int s,c;
  struct DataMap *data;
 
  char *cptr;
  int32 pnum;
  int32 lnum[2];
  int32 snum,xnum;
  int32 p0num;

  int16 lag[LAG_SIZE*2];

  float pwr0[MAX_RANGE];
  int16 slist[MAX_RANGE];
  int16 nlag[MAX_RANGE];

  char qflg[MAX_RANGE];
  char gflg[MAX_RANGE];

  float p_l[MAX_RANGE];
  float p_l_e[MAX_RANGE];
  float p_s[MAX_RANGE];
  float p_s_e[MAX_RANGE];

  float v[MAX_RANGE];
  float v_e[MAX_RANGE];

  float w_l[MAX_RANGE];
  float w_l_e[MAX_RANGE];
  float w_s[MAX_RANGE];
  float w_s_e[MAX_RANGE];

  float sd_l[MAX_RANGE];
  float sd_s[MAX_RANGE];
  float sd_phi[MAX_RANGE];

  char x_qflg[MAX_RANGE];
  char x_gflg[MAX_RANGE];

  float x_p_l[MAX_RANGE];
  float x_p_l_e[MAX_RANGE];
  float x_p_s[MAX_RANGE];
  float x_p_s_e[MAX_RANGE];

  float x_v[MAX_RANGE];
  float x_v_e[MAX_RANGE];

  float x_w_l[MAX_RANGE];
  float x_w_l_e[MAX_RANGE];
  float x_w_s[MAX_RANGE];
  float x_w_s_e[MAX_RANGE];

  float phi0[MAX_RANGE];
  float phi0_e[MAX_RANGE];
  float elv[MAX_RANGE];
  float elv_low[MAX_RANGE];
  float elv_high[MAX_RANGE];

  float x_sd_l[MAX_RANGE];
  float x_sd_s[MAX_RANGE];
  float x_sd_phi[MAX_RANGE];

  float sky_noise;
  float lag0_noise;
  float vel_noise;

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
    if ((fit->rng[c].qflg==1) || (fit->xrng[c].qflg==1)) {
      slist[snum]=c;
      nlag[snum]=fit->rng[c].nump;
      
      qflg[snum]=fit->rng[c].qflg;
      gflg[snum]=fit->rng[c].gsct;
        
      p_l[snum]=fit->rng[c].p_l;
      p_l_e[snum]=fit->rng[c].p_l_err;
      p_s[snum]=fit->rng[c].p_s;
      p_s_e[snum]=fit->rng[c].p_s_err;
        
      v[snum]=fit->rng[c].v;
      v_e[snum]=fit->rng[c].v_err;

      w_l[snum]=fit->rng[c].w_l;
      w_l_e[snum]=fit->rng[c].w_l_err;
      w_s[snum]=fit->rng[c].w_s;
      w_s_e[snum]=fit->rng[c].w_s_err;

      sd_l[snum]=fit->rng[c].sdev_l;
      sd_s[snum]=fit->rng[c].sdev_s;
      sd_phi[snum]=fit->rng[c].sdev_phi;

 
      x_qflg[snum]=fit->xrng[c].qflg;
      x_gflg[snum]=fit->xrng[c].gsct;
    
      x_qflg[snum]=fit->xrng[c].qflg;
      x_gflg[snum]=fit->xrng[c].gsct;
        
      x_p_l[snum]=fit->xrng[c].p_l;
      x_p_l_e[snum]=fit->xrng[c].p_l_err;
      x_p_s[snum]=fit->xrng[c].p_s;
      x_p_s_e[snum]=fit->xrng[c].p_s_err;
        
      x_v[snum]=fit->xrng[c].v;
      x_v_e[snum]=fit->xrng[c].v_err;

  
      x_w_l[snum]=fit->xrng[c].w_l;
      x_w_l_e[snum]=fit->xrng[c].w_l_err;
      x_w_s[snum]=fit->xrng[c].w_s;
      x_w_s_e[snum]=fit->xrng[c].w_s_err;

      phi0[snum]=fit->xrng[c].phi0;
      phi0_e[snum]=fit->xrng[c].phi0_err;
      elv[snum]=fit->elv[c].normal;
      elv_low[snum]=fit->elv[c].low;
      elv_high[snum]=fit->elv[c].high;

      x_sd_l[snum]=fit->xrng[c].sdev_l;
      x_sd_s[snum]=fit->xrng[c].sdev_s;
      x_sd_phi[snum]=fit->xrng[c].sdev_phi;
      snum++;
    }
  }      
  if (prm->xcf !=0) xnum=snum;
  else xnum=0;

  if (snum !=0) {
    DataMapAddArray(data,"slist",DATASHORT,1,&snum,slist);
    DataMapAddArray(data,"nlag",DATASHORT,1,&snum,nlag);

    DataMapAddArray(data,"qflg",DATACHAR,1,&snum,qflg); 
    DataMapAddArray(data,"gflg",DATACHAR,1,&snum,gflg); 
  
    DataMapAddArray(data,"p_l",DATAFLOAT,1,&snum,p_l);   
    DataMapAddArray(data,"p_l_e",DATAFLOAT,1,&snum,p_l_e); 
  
    DataMapAddArray(data,"p_s",DATAFLOAT,1,&snum,p_s); 
    DataMapAddArray(data,"p_s_e",DATAFLOAT,1,&snum,p_s_e); 
    DataMapAddArray(data,"v",DATAFLOAT,1,&snum,v); 
    DataMapAddArray(data,"v_e",DATAFLOAT,1,&snum,v_e); 
 
    DataMapAddArray(data,"w_l",DATAFLOAT,1,&snum,w_l); 
    DataMapAddArray(data,"w_l_e",DATAFLOAT,1,&snum,w_l_e); 
    DataMapAddArray(data,"w_s",DATAFLOAT,1,&snum,w_s); 
    DataMapAddArray(data,"w_s_e",DATAFLOAT,1,&snum,w_s_e); 
  
    DataMapAddArray(data,"sd_l",DATAFLOAT,1,&snum,sd_l); 
    DataMapAddArray(data,"sd_s",DATAFLOAT,1,&snum,sd_s); 
    DataMapAddArray(data,"sd_phi",DATAFLOAT,1,&snum,sd_phi); 
  
    if (prm->xcf !=0) {
      DataMapAddArray(data,"x_qflg",DATACHAR,1,&xnum,x_qflg); 
      DataMapAddArray(data,"x_gflg",DATACHAR,1,&xnum,x_gflg); 
  
      DataMapAddArray(data,"x_p_l",DATAFLOAT,1,&xnum,x_p_l);   
      DataMapAddArray(data,"x_p_l_e",DATAFLOAT,1,&xnum,x_p_l_e); 
      DataMapAddArray(data,"x_p_s",DATAFLOAT,1,&xnum,x_p_s); 
      DataMapAddArray(data,"x_p_s_e",DATAFLOAT,1,&xnum,x_p_s_e); 
  
      DataMapAddArray(data,"x_v",DATAFLOAT,1,&xnum,x_v); 
      DataMapAddArray(data,"x_v_e",DATAFLOAT,1,&xnum,x_v_e); 
 
      DataMapAddArray(data,"x_w_l",DATAFLOAT,1,&xnum,x_w_l); 
      DataMapAddArray(data,"x_w_l_e",DATAFLOAT,1,&xnum,x_w_l_e);   
      DataMapAddArray(data,"x_w_s",DATAFLOAT,1,&xnum,x_w_s); 
      DataMapAddArray(data,"x_w_s_e",DATAFLOAT,1,&xnum,x_w_s_e); 
  
      DataMapAddArray(data,"phi0",DATAFLOAT,1,&xnum,phi0); 
      DataMapAddArray(data,"phi0_e",DATAFLOAT,1,&xnum,phi0_e); 
      DataMapAddArray(data,"elv",DATAFLOAT,1,&xnum,elv); 
      DataMapAddArray(data,"elv_low",DATAFLOAT,1,&xnum,elv_low); 
      DataMapAddArray(data,"elv_high",DATAFLOAT,1,&xnum,elv_high); 

      DataMapAddArray(data,"x_sd_l",DATAFLOAT,1,&xnum,x_sd_l); 
      DataMapAddArray(data,"x_sd_s",DATAFLOAT,1,&xnum,x_sd_s); 
      DataMapAddArray(data,"x_sd_phi",DATAFLOAT,1,&xnum,x_sd_phi); 
    }
  }



  if (fid !=-1) s=DataMapWrite(fid,data);
  else s=DataMapSize(data);
  DataMapFree(data);
  return s;
}


int FitFwrite(FILE *fp,struct RadarParm *prm,
              struct FitData *fit) {
  return FitWrite(fileno(fp),prm,fit);
}


