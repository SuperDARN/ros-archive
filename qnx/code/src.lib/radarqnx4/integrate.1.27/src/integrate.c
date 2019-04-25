/* integrate.c
   ===========
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
#include "ad.h"
#include "dio.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"
#include "iqcopy.h"
#include "rawfeed.h"
#include "integrate.h"
#include "pulse.h"




/*
 $Log: integrate.c,v $
 Revision 1.27  2008/03/20 20:38:14  code
 Added IQCopy to order the samples in the buffer.

 Revision 1.26  2008/03/18 21:39:55  code
 Removed redundant lagmax variable.

 Revision 1.25  2008/03/14 19:01:17  code
 Fixed bug in setting the IQ buffer size.

 Revision 1.24  2008/03/14 16:46:02  code
 Changed variable name.

 Revision 1.23  2008/03/14 16:01:22  code
 Added skip sample term.

 Revision 1.22  2008/03/14 15:36:43  code
 Added check for NULL pointer to I&Q sample buffer.

 Revision 1.21  2008/03/13 23:03:22  code
 Removed the "delay" term - see the ACF library for details.

 Revision 1.20  2008/03/13 22:24:35  code
 Fixed bug in selecting the buffer to copy.

 Revision 1.19  2008/03/13 21:33:45  code
 Added capture of I&Q samples.

 Revision 1.18  2008/03/13 19:06:16  code
 Added passing the noise to the user function.

 Revision 1.17  2006/09/22 17:43:32  code
 Added code to prevent overruns at the end of the intergration period.

 Revision 1.16  2006/08/23 19:34:38  code
 Added sample delay for digital receiver.

 Revision 1.15  2006/08/17 19:21:58  code
 Added support for extended lag tables.

 Revision 1.14  2006/07/24 14:18:43  code
 Passed in beam number and frequency.

 Revision 1.13  2006/04/07 14:52:48  barnes
 Replaced the prototype "monitor" system with RawFeed.

 Revision 1.12  2006/02/07 19:43:34  barnes
 Simon Shepherd's modification to the lag table.

 Revision 1.11  2005/07/26 19:55:06  barnes
 Fixed calculation of XCFs.

 Revision 1.10  2004/06/21 19:50:57  barnes
 Added the attenuation stepping factor as a parameter.

 Revision 1.9  2004/06/04 22:28:29  barnes
 Fixed bug that meant that the number of channels was being set too high.
 (The convention is that a channel has an I and Q component - previously
 the I and Q were treated as different channels).

 Revision 1.8  2004/05/11 17:28:18  barnes
 Isolated platform specific code.

 Revision 1.7  2004/05/08 16:29:51  barnes
 Changed buffer type.

 Revision 1.6  2004/05/05 01:22:53  barnes
 Removed redundant code.

 Revision 1.5  2004/05/04 18:53:21  barnes
 Renamed the type header to avoid confusion.

 Revision 1.4  2004/05/03 20:06:38  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.3  2004/05/03 18:01:16  barnes
 Reordered headers.

 Revision 1.2  2004/05/03 15:38:05  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/03/13 20:13:56  barnes
 Initial revision

*/


int IntegrateSetTock(struct timespec *tock,int sc,int us) {
#ifdef _QNX4
  if (tock==NULL) return -1;
  if (clock_gettime(CLOCK_REALTIME,tock)==-1) return -1;
  tock->tv_sec+=sc;
  tock->tv_nsec+=us*1000;
#endif
  return 0;
}

int Integrate(struct TSGprm *prm,int tsgid,int mplgs,
              int lagnum,int (*lags)[2],struct timespec *tock,
	      int badrng,int bmnum,int tfreq,
	      ipcid_t dioid,ipcid_t adid,ipcid_t rawid,
              ipcid_t resetid,int maxatten,float mxpwr,
              float atstp,
	      void **bufadr,int *atten,
	      float *noise,float *pwr0,
              float *acfd,float *xcfd,int *flg,
	      int thr,int lmt,
	      float *dco,
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
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
                           float *noise,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdata),void *usrdata) {
 
 
  struct timespec tick;
  struct timespec tack;
  int abflg,aflg,oaflg=0,catn=0;
  int nave=0;
  int i,j; 
  int abuf,buf;
  int iqsze,iqoff;
 
  int nchannel;
  int nsample;

  int smpcnt=0;
  int status=0;
  
  float *dptr=NULL;  

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
  } else;


  if (xcfd !=NULL) nchannel = 2;
  else nchannel = 1;

  buf=0;
  *flg=0;

  nsample=prm->samples; 

  *smpnum=nsample;
  *skpnum=0;
  *rxchn=nchannel;
  iqsze=0;
  iqoff=0;

  if (IntegratePulse(buf,nsample,nchannel,tsgid,dioid,
                     adid,resetid) != 0) {
    *flg|=INT_PULSE_FAIL;
    DIOSetGain(dioid,maxatten);
    return -1;
  }
#ifdef _QNX4
  clock_gettime(CLOCK_REALTIME,&tick);
  clock_gettime(CLOCK_REALTIME,&tack);
#endif

  RawFeedSet(rawid,prm,mplgs,lagnum,lags,0,(*atten)*atstp,*noise,
	     bmnum,tfreq,&tick);



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

      status=ADGetScanStatus(adid)==SCAN_OK;
      if (status==0) smpcnt++;
      if (status==1) RawFeedScan(rawid,abuf,nsample,nchannel,
                                 (*atten)*atstp,&tick);
       
      if ((status==1) && (samples !=NULL)) {
        seqtval[nave].tv_sec=tick.tv_sec;
        seqtval[nave].tv_nsec=tick.tv_nsec;
        seqatten[nave]=*atten;
        seqnoise[nave]=*noise;
        seqoff[nave]=iqsze/2; /* in words */
        seqsze[nave]=(nsample*nchannel)*2; /* in words */
      }

      if (IntegratePulse(buf,nsample,
                         nchannel,tsgid,dioid,adid,resetid) !=0)  {
         *flg|=INT_PULSE_FAIL;
         DIOSetGain(dioid,maxatten);
         return -1;
      }

      if (status==1) {
        /* copy into our I&Q save buffer */
        if ((samples !=NULL) && ((iqsze+(nsample*nchannel)*4)<IQBUFSIZE)) {
          IQCopy( (int16 *) bufadr[abuf],samples+iqoff,
                 nsample,nchannel,REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
                 2*nchannel,2);
          iqsze+=(nsample*nchannel)*4;
          iqoff=iqsze/2;
        }      
        /* call the user function */
        if (user !=NULL) (user)(prm,&tick,tock,nave,(*atten)*atstp,noise,
		                bufadr[abuf],abuf,nsample,nchannel,
						      usrdata);

        aflg=ACFSumPower(prm,mplgs,lags,pwr0,
		         (int16 *) bufadr[abuf],2*nchannel,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrng,
                         *noise,mxpwr,catn*atstp,thr,lmt,&abflg);
	
        if (abflg==0) {

          nave++;

          if (dco !=NULL) dptr=&dco[4*(*atten)];
          else dptr=NULL;


          ACFCalculate(prm,(int16 *) bufadr[abuf],2*nchannel,0,
                       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgs,lagnum,
	  	       lags,acfd,ACF_PART,2,badrng,catn*atstp,dptr);

          if (xcfd !=NULL) ACFCalculate(prm,(int16 *) bufadr[abuf],
					2*nchannel,0,
                                        REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgs,
	  	                        lagnum,lags,xcfd,XCF_PART,2,badrng,
					catn*atstp,dptr);
          
          catn = 0; 		 

          

          if  ((aflg != -1) && (oaflg == aflg))  {
            aflg = -1;
            if (*atten < maxatten) {
              ACFNormalize(pwr0,acfd,xcfd,prm->nrang,mplgs,atstp); 
              (*atten)++;
              *noise=*noise/atstp;
              if (*noise==0) *noise=1;
              ADGetScanStatus(adid);
              DIOSetGain(dioid,*atten);
              catn = 1;   
            } else *flg|=INT_ATTEN_OVF;
       
          }
          oaflg = aflg;
	}
      }
#ifdef _QNX4
      clock_gettime(CLOCK_REALTIME,&tick);
#endif
  }
  
  status=ADGetScanStatus(adid)==SCAN_OK;
  if (status==1) RawFeedScan(rawid,buf,nsample,nchannel,(*atten)*atstp,&tick);

  if ((samples !=NULL) && (status==1)) {
    seqtval[nave].tv_sec=tick.tv_sec;
    seqtval[nave].tv_nsec=tick.tv_nsec;
    seqatten[nave]=*atten;
    seqnoise[nave]=*noise;
    seqoff[nave]=iqsze/2;
    seqsze[nave]=(nsample*nchannel)*2; /* in words */
  }


  if (status==0) smpcnt++;
  if (status==1) { 

    /* copy into our I&Q save buffer */
    if ((samples !=NULL) && ((iqsze+(nsample*nchannel)*4)<IQBUFSIZE)) {
       IQCopy( (int16 *) bufadr[buf],samples+iqoff,
                 nsample,nchannel,REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
                 2*nchannel,2);
          iqsze+=(nsample*nchannel)*4;
          iqoff=iqsze/2;
    }     

    if (user !=NULL) (user)(prm,&tick,tock,nave,(*atten)*atstp,noise,
		             bufadr[buf],buf,nsample,nchannel,
			     usrdata);

     aflg=ACFSumPower(prm,mplgs,lags,pwr0,
		         (int16 *) bufadr[buf],2*nchannel,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrng,
                         *noise,mxpwr,catn*atstp,thr,lmt,&abflg);

     if (abflg==0) {

        if (dco !=NULL) dptr=&dco[4*(*atten)];
        else dptr=NULL;

        ACFCalculate(prm,(int16 *) bufadr[buf],2*nchannel,0,
                     REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgs,
	  	     lagnum,lags,acfd,ACF_PART,2,badrng,catn*atstp,dptr);

        if (xcfd !=NULL) ACFCalculate(prm,(int16 *) bufadr[buf],
				      2*nchannel,0,
                                      REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgs,
	  	                      lagnum,lags,xcfd,XCF_PART,2,badrng,
				      catn*atstp,dptr);

        nave++;
     }
   }



   if (smpcnt>MAX_SAMPLE_ERR) *flg|=INT_SAMPLE_FAIL;
   RawFeedTrigger(rawid);
   if (nave>0) {
     if (lagnum==0) ACFAverage(pwr0,acfd,xcfd,NULL,nave,prm->nrang,mplgs);
     else ACFAverage(pwr0,acfd,xcfd,lagsum,nave,prm->nrang,mplgs);
   } 
   DIOSetGain(dioid,maxatten); 
   return nave;
} 





