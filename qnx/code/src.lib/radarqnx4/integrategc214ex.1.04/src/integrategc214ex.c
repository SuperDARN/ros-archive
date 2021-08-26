/* integrategc214ex.c
   ==================
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
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "gc214.h"
#include "dio.h"
#include "limit.h"
#include "tsg.h"
#include "rawfeed.h"
#include "integrategc214ex.h"
#include "acfex.h"

#include "pulse.h"

/*
 $Log: integrategc214ex.c,v $
 Revision 1.4  2007/07/09 20:34:37  code
 Added rawdump features to write out the raw samples.

 Revision 1.3  2007/02/19 15:31:22  code
 Passed roff and ioff to the ACF library.

 Revision 1.2  2007/02/13 16:36:42  code
 Fixed bugs.

 Revision 1.1  2007/02/12 18:06:47  code
 Initial revision

*/

int IntegrateGC214exSetTock(struct timespec *tock,int sc,int us) {
#ifdef _QNX4
  if (tock==NULL) return -1;
  if (clock_gettime(CLOCK_REALTIME,tock)==-1) return -1;
  tock->tv_sec+=sc;
  tock->tv_nsec+=us*1000;
#endif
  return 0;
}


int IntegrateGC214ex(struct TSGprm *prm,int tsgid,int mplgs,
                   int lagnum,int (*lags)[2],
                   struct timespec *tock,int badrng,
                   int bmnum,int tfreq,
		   ipcid_t dioid,ipcid_t drid,ipcid_t rawid,int maxatten,
                   int smpdelay,int roff,int ioff,
		   void **bufadr,float *noise,float *pwr0,
                   float *acfd,float *xcfd,int *flg,
	           int thr,int lmt,
		   void (*user)(struct TSGprm *prm,
			        struct timespec *tick,struct timespec *tock,
			        int nave,int atten,
		                unsigned char *buffer,int bufnum,
			        int numwords,int numchannel,
			        void *usrdata),void *usrdata) {
 
  struct timespec tick;
  struct timespec tack;
  struct timespec stval;
  int nave=0;
  int i,j; 
  int abuf=0,buf=0;
 
  int nchannel;
  int nsample;
 
  int smpcnt=0;
  int status=0;
  int srate=3333;
  int s;

  int lagmax=0;

  double tval=0,tavg=0;

  char *rawfname;
  FILE *rawfp;

  unsigned char *iqbuf=NULL;
  unsigned char *iqtmp=NULL;

  int iqsze=0;
  int iqnum=0;
 
  int lagsum[LAG_SIZE]; 


  struct mark {
    struct timespec tval;
    int atten;
    int chn;
    int off;
    int sze;
  };

  struct mark mark[1024];
  int snum=0;

  for (j=0;j<LAG_SIZE;j++) lagsum[j]=0;

  for (i=0;i<MAX_RANGE;i++) {
     pwr0[i]=0;
     for (j=0;j<LAG_SIZE*2;j++) {
       acfd[i*LAG_SIZE*2+j]=0;
       if (xcfd !=NULL) xcfd[i*LAG_SIZE*2+j]=0;
     }
  }

  if (lagnum !=0) {
    lagmax=lagnum;
    for (j=0;j<lagnum;j++) {
      i=abs(lags[j][0]-lags[j][1]);
      lagsum[i]++;
    }
  } else lagmax=mplgs;
 
 /* Not implemented for XCFs 

  if (xcfd !=NULL) nchannel = 2;
  else nchannel = 1;

 */

  rawfname=(char *) usrdata;

  nchannel=1;

  buf=0;
  *flg=0;

  nsample=prm->samples;
  nsample+=prm->smdelay-1;

  srate=300000/(2*prm->rsep); /* 1/smsep = sample frequency */

  DIOSetGain(dioid,0);

  if ((s=GC214SetSRate(drid,srate)) !=0) { 
    DIOSetGain(dioid,maxatten);
    return (-100+s);
  }
  
  if ((s=IntegrateGC214exPulse(buf,nsample+smpdelay,
                          nchannel,tsgid,dioid,drid)) != 0) {
    *flg|=INT_PULSE_FAIL;
    DIOSetGain(dioid,maxatten);
    return (-200+s);
  }
#ifdef _QNX4 
  clock_gettime(CLOCK_REALTIME,&tick);
  clock_gettime(CLOCK_REALTIME,&tack);
#endif  

  RawFeedSet(rawid,prm,mplgs,lagnum,lags,smpdelay,0,*noise,bmnum,tfreq,&tick);
  stval.tv_sec=tick.tv_sec;
  stval.tv_nsec=tick.tv_nsec;
  
  

  while (1) {
     

      tval=(tick.tv_sec+tick.tv_nsec/1.0e9)-
           (tack.tv_sec+tack.tv_nsec/1.0e9);


      if (nave>0) tavg=tval/nave; 
     
      tick.tv_sec+=floor(tavg);
      tick.tv_nsec+=1.0e9*(tavg-floor(tavg));

      if (tick.tv_sec>tock->tv_sec) break;
      if ((tick.tv_sec>=tock->tv_sec) && (tick.tv_nsec>tock->tv_nsec)) break;
      abuf = buf;
      buf = (buf + 1) % 2 ;

      status=GC214GetScanStatus(drid)==SCAN_OK;
      if (status==0) smpcnt++;
      if (status==1) RawFeedScan(rawid,abuf,nsample+smpdelay,nchannel,0,&tick);
      if (status==1) {
	mark[snum].tval.tv_sec=tick.tv_sec;
        mark[snum].tval.tv_nsec=tick.tv_nsec;
        mark[snum].chn=nchannel;
        mark[snum].off=iqsze;
        mark[snum].sze=(nsample+smpdelay)*4;
        mark[snum].atten=0;
        snum++;
      }

      if ((s=IntegrateGC214exPulse(buf,nsample+smpdelay,
                              nchannel,tsgid,dioid,drid)) !=0)  {
         *flg|=INT_PULSE_FAIL;  
         DIOSetGain(dioid,maxatten);
         return (-300+s);
      }
      if (status==1) {
     
     
        /* call the user function */
        if (user !=NULL) (user)(prm,&tick,tock,nave,0,
		                bufadr[abuf],abuf,nsample+smpdelay,
                                nchannel,usrdata);

         /* copy the data into the buffer here */

	 if (iqbuf==NULL) iqbuf=malloc(4*(nsample+smpdelay));
         else {
	   iqtmp=realloc(iqbuf,iqsze+4*(nsample+smpdelay));
           if (iqtmp==NULL) {
             free(iqbuf);
             return -1;
	   }
           iqbuf=iqtmp;
	 }
         memcpy(iqbuf+iqsze,bufadr[abuf],4*(nsample+smpdelay));
         iqsze+=4*(nsample+smpdelay);
         iqnum+=nsample+smpdelay;

         
         nave++;
      }
#ifdef _QNX4
    clock_gettime(CLOCK_REALTIME,&tick);
#endif
  }

  status=GC214GetScanStatus(drid)==SCAN_OK;
  if (status==1) RawFeedScan(rawid,buf,nsample+smpdelay,nchannel,0,&tick);
  if (status==1) {
    mark[snum].tval.tv_sec=tick.tv_sec;
    mark[snum].tval.tv_nsec=tick.tv_nsec;
    mark[snum].chn=nchannel;
    mark[snum].off=iqsze;
    mark[snum].sze=(nsample+smpdelay)*4;
    mark[snum].atten=0;
    snum++;
  }

  if (status==0) smpcnt++;
  if (status==1) { 
    if (user !=NULL) (user)(prm,&tick,tock,nave,0,
		            bufadr[buf],buf,nsample+smpdelay,
                            nchannel,usrdata);

    /* last copy of data into buffer */
    if (iqbuf==NULL) iqbuf=malloc(4*nsample+smpdelay);
      else {
        iqtmp=realloc(iqbuf,iqsze+4*(nsample+smpdelay));
        if (iqtmp==NULL) {
          free(iqbuf);
          return -1;
	}
        iqbuf=iqtmp;
      }
      memcpy(iqbuf+iqsze,bufadr[buf],4*(nsample+smpdelay));
      iqsze+=4*(nsample+smpdelay);
      iqnum+=nsample+smpdelay;
      nave++;
  }

  if (smpcnt>MAX_SAMPLE_ERR) *flg|=INT_SAMPLE_FAIL;

  RawFeedTrigger(rawid);
  if (nave>0) {
    ACFexCalculate(prm,(int16 *) iqbuf,iqnum,nave,nsample+smpdelay,smpdelay,
                   roff,ioff,
		   mplgs,lagnum,lags,lagsum,
		   pwr0,acfd,noise);

    if (rawfname !=NULL) {
        struct DataMap *data;
        int yr,mo,dy,hr,mt,sc,us,s,n;
        int16 *sptr;
        double dsc;
        int32 pnum;
        int32 lnum[2];
        int32 tnum,smnum;

        int32 tsc[1024];
        int32 tus[1024];
        int32 tatten[1024];
        int32 tchn[1024];
        int32 toff[1024];
        int32 tsze[1024];
        
        int32 pat[PULSE_SIZE];
        int32 lagtab[LAG_SIZE*2];
       
        int32 iatten=0;

        fprintf(stderr,"got here...%s\n",rawfname);

        for (n=0;n<prm->mppul;n++) pat[n]=prm->pat[n];
        for (n=0;n<=lagnum;n++) {
          lagtab[2*n]=lags[n][0];
          lagtab[2*n+1]=lags[n][1];
        }


        data=DataMapMake();
        sptr=(int16 *) iqbuf;

        TimeEpochToYMDHMS((int) stval.tv_sec,&yr,&mo,&dy,&hr,&mt,&dsc);
        sc=(int) dsc;
        us=(int) stval.tv_nsec/1000.0;
        
        DataMapAddScalar(data,"time.yr",DATAINT,&yr);
        DataMapAddScalar(data,"time.mo",DATAINT,&mo);
        DataMapAddScalar(data,"time.dy",DATAINT,&dy);
        DataMapAddScalar(data,"time.hr",DATAINT,&hr);
        DataMapAddScalar(data,"time.mt",DATAINT,&mt);
        DataMapAddScalar(data,"time.sc",DATAINT,&sc);
        DataMapAddScalar(data,"time.us",DATAINT,&us);
        DataMapAddScalar(data,"nrang",DATAINT,&prm->nrang);
        DataMapAddScalar(data,"frang",DATAINT,&prm->frang);
        DataMapAddScalar(data,"rsep",DATAINT,&prm->rsep);
        DataMapAddScalar(data,"smsep",DATAINT,&prm->smsep);
        DataMapAddScalar(data,"lagfr",DATAINT,&prm->lagfr);
        DataMapAddScalar(data,"txpl",DATAINT,&prm->txpl);
        DataMapAddScalar(data,"mppul",DATAINT,&prm->mppul);
        DataMapAddScalar(data,"mpinc",DATAINT,&prm->mpinc);
        DataMapAddScalar(data,"mlag",DATAINT,&prm->mlag);
        DataMapAddScalar(data,"samples",DATAINT,&prm->samples);
        DataMapAddScalar(data,"smdelay",DATAINT,&prm->smdelay);
        DataMapAddScalar(data,"rtoxmin",DATAINT,&prm->rtoxmin);
        DataMapAddScalar(data,"mplgs",DATAINT,&mplgs);
        DataMapAddScalar(data,"lagnum",DATAINT,&lagnum); 
        DataMapAddScalar(data,"rsmdelay",DATAINT,&smpdelay);

        DataMapAddScalar(data,"iatten",DATAINT,&iatten);
        DataMapAddScalar(data,"noise",DATAFLOAT,&noise);
        DataMapAddScalar(data,"bmnum",DATAINT,&bmnum);
        DataMapAddScalar(data,"tfreq",DATAINT,&tfreq);

        DataMapAddArray(data,"ptab",DATAINT,1,&pnum,pat);
        DataMapAddArray(data,"ltab",DATAINT,2,lnum,lagtab);

        DataMapAddArray(data,"tsc",DATAINT,1,&tnum,tsc);
        DataMapAddArray(data,"tus",DATAINT,1,&tnum,tus);
        DataMapAddArray(data,"tatten",DATAINT,1,&tnum,tatten);
        DataMapAddArray(data,"tchn",DATAINT,1,&tnum,tchn);
        DataMapAddArray(data,"toff",DATAINT,1,&tnum,toff);
        DataMapAddArray(data,"tsze",DATAINT,1,&tnum,tsze);

        DataMapAddArray(data,"data",DATASHORT,1,&smnum,sptr);
	
        pnum=prm->mppul;
        lnum[0]=2;
        lnum[1]=lagnum+1;
	tnum=snum;
        smnum=iqsze/2;

        for (n=0;n<snum;n++) {
          tsc[n]=(int) mark[n].tval.tv_sec;
          tus[n]=(int) mark[n].tval.tv_nsec/1000.0;
          tatten[n]=mark[n].atten;
          tchn[n]=mark[n].chn;
          toff[n]=mark[n].off;
          tsze[n]=mark[n].sze;
	}

      
        rawfp=fopen(rawfname,"a+");
        s=DataMapWrite(fileno(rawfp),data);

        DataMapFree(data);
        fclose(rawfp);
    }


    free(iqbuf);
  }
  DIOSetGain(dioid,maxatten);
  return nave;
} 








