/* integrategc214.c
   ================
   Author: R.J.Barnes & R.Todd Parris
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
#include "gc214.h"
#include "dio.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"
#include "iqcopy.h"
#include "rawfeed.h"
#include "integrategc214.h"


#include "pulse.h"

/*
 $Log: integrategc214.c,v $
 Revision 1.29  2008/03/20 20:52:57  code
 Added IQCopy.

 Revision 1.28  2008/03/18 21:42:02  code
 Removed redundant lagmax variable.

 Revision 1.27  2008/03/14 19:03:33  code
 Fixed bug in setting the I&Q buffer sizes.

 Revision 1.26  2008/03/14 16:49:29  code
 Changed variable name.

 Revision 1.25  2008/03/14 16:06:11  code
 Added sample skip number.

 Revision 1.24  2008/03/14 15:49:24  code
 Added check for NULL pointer to the I&Q sample buffer.

 Revision 1.23  2008/03/13 23:46:15  code
 Changed noise into a pointer.

 Revision 1.22  2008/03/13 23:07:51  code
 Removed "delay" - see ACF library for details.

 Revision 1.21  2008/03/13 22:28:35  code
 Added capture of I&Q samples.

 Revision 1.20  2007/03/28 15:56:24  code
 Changed sample rate to a float and removed attenuation settings.

 Revision 1.19  2006/09/22 17:42:54  code
 Added code to prevent overruns at the end of the integration period.

 Revision 1.18  2006/08/23 19:32:53  code
 Added sample delay for digital receiver.

 Revision 1.17  2006/08/17 19:16:34  code
 Added support for extended lag tables.

 Revision 1.16  2006/07/24 14:53:50  code
 Applied Dieter Andre's fix for XCFs.

 Revision 1.15  2006/07/24 14:32:11  code
 Passed in beam number and frequency.

 Revision 1.14  2006/04/07 15:01:43  barnes
 Incorporated the RawFeed system.

 Revision 1.13  2006/02/07 19:48:51  barnes
 Simon Shepherd's lag table modification.

 Revision 1.12  2006/02/07 17:41:34  barnes
 Fixed missing tests of error conditions.

 Revision 1.11  2006/02/07 17:02:54  barnes
 Dieter Andre's modification to return error codes to the control program
 and his bug fix for calculating the lag zero power.

 Revision 1.10  2005/07/26 19:53:12  barnes
 Fixed XCF calculation.

 Revision 1.9  2005/07/19 18:19:17  barnes
 Modification to move the sample delay and I & Q sample ordering out
 of the library.

 Revision 1.8  2004/05/11 17:31:16  barnes
 Isolated platform specific code.

 Revision 1.7  2004/05/11 15:29:56  barnes
 Changed buffer type.

 Revision 1.6  2004/05/05 01:22:09  barnes
 Removed redundant code.

 Revision 1.5  2004/05/04 18:54:08  barnes
 Renamed the type header to avoid confusion.

 Revision 1.4  2004/05/03 20:07:07  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.3  2004/05/03 18:02:02  barnes
 Reordered headers.

 Revision 1.2  2004/05/03 15:30:09  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/03/13 19:54:01  barnes
 Initial revision

*/

int IntegrateGC214SetTock(struct timespec *tock,int sc,int us) {
#ifdef _QNX4
  if (tock==NULL) return -1;
  if (clock_gettime(CLOCK_REALTIME,tock)==-1) return -1;
  tock->tv_sec+=sc;
  tock->tv_nsec+=us*1000;
#endif
  return 0;
}
   
int IntegrateGC214(struct TSGprm *prm,int tsgid,int mplgs,
                   int lagnum,int (*lags)[2],
                   struct timespec *tock,int badrng,
                   int bmnum,int tfreq,
		   ipcid_t dioid,ipcid_t drid,ipcid_t rawid,int maxatten,
                   int roff,int ioff,
		   void **bufadr,float *noise,float *pwr0,
                   float *acfd,float *xcfd,int *flg,
	           int thr,int lmt,
                   int *rxchn,
                   int *smpnum,
                   int *skpnum,
                   struct timespec *seqtval,
                   int *seqatten,
                   float *seqnoise,
                   int *seqoff,
                   int *seqsze,
                   int16 *samples,
		   void (*user)(struct TSGprm *prm,
			        struct timespec *tick,struct timespec *tock,
			        int nave,int atten,
                                float *noise,
		                unsigned char *buffer,int bufnum,
			        int numwords,int numchannel,
			        void *usrdata),void *usrdata) {
 
  struct timespec tick;
  struct timespec tack;
  int abflg;
  int nave=0;
  int i,j; 
  int abuf,buf;
  int iqsze,iqoff; 
  int nchannel;
  int nsample;
 
  int smpcnt=0;
  int status=0;
  float srate=3333;
  int s;

  double tval=0,tavg=0;
   
  int lagsum[LAG_SIZE]; 
  for (j=0;j<LAG_SIZE;j++) lagsum[j]=0;

  for (i=0;i<MAX_RANGE;i++) {
     pwr0[i]=0;
     for (j=0;j<LAG_SIZE*2;j++) {
       acfd[i*LAG_SIZE*2+j]=0;
       if (xcfd !=NULL) xcfd[i*LAG_SIZE*2+j]=0;
     }
  }

  if (lagnum !=0) {
    
    for (j=0;j<lagnum;j++) {
      i=abs(lags[j][0]-lags[j][1]);
      lagsum[i]++;
    }
  } 

  if (xcfd !=NULL) nchannel = 2;
  else nchannel = 1;

  buf=0;
  *flg=0;

  nsample=prm->samples;
  nsample+=prm->smdelay-1;

  *smpnum=nsample;
  *skpnum=prm->smdelay;
  *rxchn=nchannel;
  iqsze=0;
  iqoff=0;

  srate=300000.0/(2.0*prm->rsep); /* 1/smsep = sample frequency */
  /*
  DIOSetGain(dioid,0);
  */
  if ((s=GC214SetSRate(drid,srate)) !=0) {
    /* 
    DIOSetGain(dioid,maxatten);
    */
    return (-100+s);
  }
  
  if ((s=IntegrateGC214Pulse(buf,nsample,
                          nchannel,tsgid,dioid,drid)) != 0) {
    *flg|=INT_PULSE_FAIL;
    /*
    DIOSetGain(dioid,maxatten);
    */
    return (-200+s);
  }
#ifdef _QNX4 
  clock_gettime(CLOCK_REALTIME,&tick);
  clock_gettime(CLOCK_REALTIME,&tack);
#endif  

  RawFeedSet(rawid,prm,mplgs,lagnum,lags,0,0,*noise,bmnum,tfreq,&tick);


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
      if (status==1) RawFeedScan(rawid,abuf,nsample,nchannel,0,&tick);

      if ((samples !=NULL) && (status==1)) {
        seqtval[nave].tv_sec=tick.tv_sec;
        seqtval[nave].tv_nsec=tick.tv_nsec;
        seqatten[nave]=0;
        seqnoise[nave]=*noise;
        seqoff[nave]=iqsze/2; /* in words */
        seqsze[nave]=(nsample*nchannel)*2; /* in words */
      }


     
      if ((s=IntegrateGC214Pulse(buf,nsample,
                              nchannel,tsgid,dioid,drid)) !=0)  {
         *flg|=INT_PULSE_FAIL;
	 /*  
         DIOSetGain(dioid,maxatten);
	 */
         return (-300+s);
      }
      if (status==1) {
        /* copy into our I&Q save buffer */
        if ((samples !=NULL) && ((iqsze+(nsample*nchannel)*4)<IQBUFSIZE)) {
          IQCopy( (int16 *) bufadr[abuf],samples+iqoff,
                 nsample,nchannel,roff,ioff,2,2*nsample);
          iqsze+=(nsample*nchannel)*4;
          iqoff=iqsze/2;
        }          
     
        /* call the user function */
        if (user !=NULL) (user)(prm,&tick,tock,nave,0,noise,
		                bufadr[abuf],abuf,nsample,
                                nchannel,usrdata);

        ACFSumPower(prm,mplgs,lags,pwr0,
	            (int16 *) bufadr[abuf],2,1,roff,
		    ioff,badrng,*noise,MAXPWR,0.0,
                    thr,lmt,&abflg);

        if (abflg==0) {

          nave++;

          ACFCalculate(prm,(int16 *) bufadr[abuf],2,1,
                       roff,ioff,mplgs,lagnum,
	  	       lags,acfd,ACF_PART,nsample,badrng,0.0,NULL);

          /* Dieter Andre fixed the XCF code */


          if (xcfd !=NULL) ACFCalculate(prm,(int16 *) bufadr[abuf],2,1,
				        roff,ioff,mplgs,lagnum,
		                        lags,xcfd,XCF_PART,
                                        2*nsample,
                                        badrng,0.0,NULL);
     	 
	}
      }
#ifdef _QNX4
    clock_gettime(CLOCK_REALTIME,&tick);
#endif
  }
  status=(GC214GetScanStatus(drid)==SCAN_OK);
  if (status==1) RawFeedScan(rawid,buf,nsample,nchannel,0,&tick);

  if ((samples !=NULL) && (status==1)) {
    seqtval[nave].tv_sec=tick.tv_sec;
    seqtval[nave].tv_nsec=tick.tv_nsec;
    seqatten[nave]=0;
    seqnoise[nave]=*noise;
    seqoff[nave]=iqsze/2;
    seqsze[nave]=(nsample*nchannel)*2; /* in words */
  }

  if (status==0) smpcnt++;
  if (status==1) { 

    /* copy into our I&Q save buffer */
    if ((samples !=NULL) && ((iqsze+(nsample*nchannel)*4)<IQBUFSIZE)) {
      IQCopy( (int16 *) bufadr[buf],samples+iqoff,
              nsample,nchannel,roff,ioff,2,2*nsample);
      iqsze+=(nsample*nchannel)*4;
      iqoff=iqsze/2;
    }     

    if (user !=NULL) (user)(prm,&tick,tock,nave,0,noise,
		            bufadr[buf],buf,nsample,
                            nchannel,usrdata);
    ACFSumPower(prm,mplgs,lags,pwr0,
	        (int16 *) bufadr[buf],2,1,roff,
		ioff,badrng,*noise,MAXPWR,0.0,
                thr,lmt,&abflg);
    if (abflg==0) {
      ACFCalculate(prm,(int16 *) bufadr[buf],2,1,roff,
		   ioff,mplgs,lagnum,
 	           lags,acfd,ACF_PART,nsample,badrng,0.0,NULL);

      /* Dieter Andre fixed the XCF code */

      if (xcfd !=NULL) ACFCalculate(prm,(int16 *) bufadr[buf],2,1,
				    roff,ioff,mplgs,lagnum,
			            lags,xcfd,XCF_PART,2*nsample,
                                    badrng,0.0,NULL);
      nave++;
    }
  }
  if (smpcnt>MAX_SAMPLE_ERR) *flg|=INT_SAMPLE_FAIL;
  RawFeedTrigger(rawid);
  if (nave>0) {
    if (lagnum==0) ACFAverage(pwr0,acfd,xcfd,NULL,nave,prm->nrang,mplgs);
    else ACFAverage(pwr0,acfd,xcfd,lagsum,nave,prm->nrang,mplgs);
  }
  /*
  DIOSetGain(dioid,maxatten);
  */

  return nave;
} 





