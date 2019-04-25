/* integratesgc214ex.c
   ===================
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
#include "gc214.h"
#include "limit.h"
#include "tsg.h"
#include "acfex.h"
#include "iqcopy.h"
#include "rawfeed.h"

#include "integratesgc214ex.h"
#include "pulse.h"

/*
 $Log: integratesgc214ex.c,v $
 Revision 1.1  2008/03/21 14:44:17  code
 Initial revision

*/

int IntegrateSGC214exSetTock(struct timespec *tock,int sc,int us) {
#ifdef _QNX4
  if (tock==NULL) return -1;
  if (clock_gettime(CLOCK_REALTIME,tock)==-1) return -1;
  tock->tv_sec+=sc;
  tock->tv_nsec+=us*1000;
#endif
  return 0;
}




int IntegrateSGC214ex(struct timespec *tock,int *delay,	      
               ipcid_t dioid,ipcid_t *drid,ipcid_t *rawid,
               void **bufadr,int bufoff,int thr,int lmt,
               int roff,int ioff,
               struct TSGprm *prmA,int tsgidA,int mplgsA,int lagnumA,
               int (*lagsA)[2],
 	           int badrngA,int bmnumA,int tfreqA,
	           int maxattenA,
	           float *noiseA,float *pwr0A,
               float *acfdA,float *xcfdA,int *flgA,
	           int *naveA,
		    int *rxchnA,
		    int *smpnumA,
                    int *skpnumA,
		    struct timespec *seqtvalA,
		    int *seqattenA,
		    float *seqnoiseA,
		    int *seqoffA,
		    int *seqszeA,
		    int16 *samplesA,
               struct TSGprm *prmB,int tsgidB,int mplgsB,int lagnumB,
               int (*lagsB)[2],
 	           int badrngB,int bmnumB,int tfreqB,
	       int maxattenB,
	           float *noiseB,float *pwr0B,
               float *acfdB,float *xcfdB,int *flgB,
	           int *naveB,
		    int *rxchnB,
		    int *smpnumB,
                    int *skpnumB,
		    struct timespec *seqtvalB,
		    int *seqattenB,
		    float *seqnoiseB,
		    int *seqoffB,
		    int *seqszeB,
		    int16 *samplesB,
               void (*userA)(struct TSGprm *prm,
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
			   float *noise,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdata),void *usrdataA,
               void (*userB)(struct TSGprm *prm,
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
			   float *noise,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdata),void *usrdataB) {


  struct timespec tick;
  struct timespec tack;


  int i,j,s; 
  int abuf,buf;
 
  int nchannelA,nchannelB;
  int nsampleA,nsampleB;

  int smpcntA=0,smpcntB=0;
  int statusA=0,statusB=0;
  float srateA=3333,srateB=3333; 


  unsigned char *iqbufA=NULL,*iqbufB=NULL;
  unsigned char *iqtmpA=NULL,*iqtmpB=NULL;

  int iqszeA=0;
  int iqoffA=0;
  int iqnumA=0;

  int iqszeB=0;
  int iqoffB=0;
  int iqnumB=0;

  int lagsumA[LAG_SIZE]; 
  int lagsumB[LAG_SIZE]; 

  struct mark {
    struct timespec tval;
    int atten;
    int chn;
    int off;
    int sze;
  };

  struct mark markA[1024];
  struct mark markB[1024];

  int snumA=0;
  int snumB=0;

  double tval=0,tavg=0;
 
 
  *naveA=0;
  *naveB=0;

  for (j=0;j<LAG_SIZE;j++) {
     lagsumA[j]=0;
     lagsumB[j]=0;
  }

  for (i=0;i<MAX_RANGE;i++) {
     pwr0A[i]=0;
     pwr0B[i]=0;
     for (j=0;j<LAG_SIZE*2;j++) {
       acfdA[i*LAG_SIZE*2+j]=0;
       if (xcfdA !=NULL) xcfdA[i*LAG_SIZE*2+j]=0;
       acfdB[i*LAG_SIZE*2+j]=0;
       if (xcfdB !=NULL) xcfdB[i*LAG_SIZE*2+j]=0;

     }
  }

  if (lagnumA !=0) {
    for (j=0;j<lagnumA;j++) {
      i=abs(lagsA[j][0]-lagsA[j][1]);
      lagsumA[i]++;
    }
  }

  if (lagnumB !=0) {
    for (j=0;j<lagnumB;j++) {
      i=abs(lagsB[j][0]-lagsB[j][1]);
      lagsumB[i]++;
    }
  } 


  /* Not implemented for XCFs

  if (xcfdA !=NULL) nchannelA = 2;
  else nchannelA = 1;

  if (xcfdB !=NULL) nchannelB = 2;
  else nchannelB = 1;

  */

  nchannelA=1;
  nchannelB=1;


  buf=0;
  *flgA=0;
  *flgB=0;

  nsampleA=prmA->samples;
  nsampleA+=prmA->smdelay-1;
 
  nsampleB=prmB->samples;
  nsampleB+=prmB->smdelay-1; 


  *smpnumA=nsampleA;
  *skpnumA=prmA->smdelay;
  *rxchnA=nchannelA;
  iqszeA=0;
  iqoffA=0;

  *smpnumB=nsampleB;
  *skpnumB=prmB->smdelay;
  *rxchnB=nchannelB;
  iqszeB=0;
  iqoffB=0;

  DIOSetChannel(dioid,0);
  DIOSetGain(dioid,0);
  DIOSetChannel(dioid,1);
  DIOSetGain(dioid,0);



  srateA=300000.0/(2.0*prmA->rsep); /* 1/smsep = sample frequency */
  if ((s=GC214SetSRate(drid[0],srateA)) !=0) return (-100+s);

  srateB=300000.0/(2.0*prmB->rsep); /* 1/smsep = sample frequency */
  if ((s=GC214SetSRate(drid[1],srateB)) !=0) return (-100+s);


 


  if (IntegrateSGC214exPulse(buf,nsampleA,nsampleB,
                      nchannelA,nchannelB,tsgidA,tsgidB,
                      delay,dioid,drid) != 0) {
    *flgA|=INT_PULSE_FAIL;
    *flgB|=INT_PULSE_FAIL;
/*
    DIOSetChannel(dioid,0);
    DIOSetGain(dioid,maxattenA);
    DIOSetChannel(dioid,1);
    DIOSetGain(dioid,maxattenB);
 */
   return -1;
  }
#ifdef _QNX4
  clock_gettime(CLOCK_REALTIME,&tick);
  clock_gettime(CLOCK_REALTIME,&tack);
#endif

  RawFeedSet(rawid[0],prmA,mplgsA,lagnumA,lagsA,
             0,0,*noiseA,
	     bmnumA,tfreqA,&tick);
  RawFeedSet(rawid[1],prmB,mplgsB,lagnumB,lagsB,
             0,0,*noiseB,
	     bmnumB,tfreqB,&tick);


  while (1) {

      tval=(tick.tv_sec+tick.tv_nsec/1.0e9)-
           (tack.tv_sec+tack.tv_nsec/1.0e9);

      if (*naveA>0) tavg=tval/(*naveA); 
     
      tick.tv_sec+=floor(tavg);
      tick.tv_nsec+=1.0e9*(tavg-floor(tavg));

      if (tick.tv_sec>tock->tv_sec) break;
      if ((tick.tv_sec>=tock->tv_sec) && (tick.tv_nsec>tock->tv_nsec)) break;

      abuf = buf;
      buf = (buf + 1) % 2 ;

      statusA=GC214GetScanStatus(drid[0])==SCAN_OK;
      statusB=GC214GetScanStatus(drid[1])==SCAN_OK;

      if (statusA==0) smpcntA++;
      if (statusB==0) smpcntB++;

      if (statusA==1) RawFeedScan(rawid[0],abuf,nsampleA,
                                  nchannelA,0,&tick);
      if (statusB==1) RawFeedScan(rawid[1],abuf,nsampleB,
                                  nchannelB,0,&tick);


      if (statusA==1) {
	markA[snumA].tval.tv_sec=tick.tv_sec;
        markA[snumA].tval.tv_nsec=tick.tv_nsec;
        markA[snumA].chn=nchannelA;
        markA[snumA].off=iqszeA;
        markA[snumA].sze=(nsampleA*nchannelA)*4;
        markA[snumA].atten=0;
        snumA++;
      }

      if (statusB==1) {
	markB[snumB].tval.tv_sec=tick.tv_sec;
        markB[snumB].tval.tv_nsec=tick.tv_nsec;
        markB[snumB].chn=nchannelB;
        markB[snumB].off=iqszeA;
        markB[snumB].sze=(nsampleB*nchannelB)*4;
        markB[snumB].atten=0;
        snumB++;
      }

       
      if ((samplesA !=NULL) && (statusA==1)) {
        seqtvalA[*naveA].tv_sec=tick.tv_sec;
        seqtvalA[*naveA].tv_nsec=tick.tv_nsec;
        seqattenA[*naveA]=0;
        seqnoiseA[*naveA]=*noiseA;
        seqoffA[*naveA]=iqszeA/2; /* in words */
        seqszeA[*naveA]=(nsampleA*nchannelA)*2; /* in words */
      }

      if ((samplesB !=NULL) && (statusB==1)) {
        seqtvalB[*naveB].tv_sec=tick.tv_sec;
        seqtvalB[*naveB].tv_nsec=tick.tv_nsec;
        seqattenB[*naveB]=0;
        seqnoiseB[*naveB]=*noiseB;
        seqoffB[*naveB]=iqszeB/2; /* in words */
        seqszeB[*naveB]=(nsampleB*nchannelB)*2; /* in words */
      }



      if (IntegrateSGC214exPulse(buf,nsampleA,nsampleB,
                         nchannelA,nchannelB,tsgidA,tsgidB,delay,
                         dioid,drid) !=0)  {
        *flgA|=INT_PULSE_FAIL;
        *flgB|=INT_PULSE_FAIL;
/*
        DIOSetChannel(dioid,0);
        DIOSetGain(dioid,maxattenA);
        DIOSetChannel(dioid,1);
        DIOSetGain(dioid,maxattenB);
*/
        return -1;
      }

      if (statusA==1) {

	if (iqbufA==NULL) iqbufA=malloc(4*(nsampleA*nchannelA));
        else {
	  iqtmpA=realloc(iqbufA,iqszeA+4*(nsampleA*nchannelA));
          if (iqtmpA==NULL) {
            free(iqbufA);
            return -1;
	  }
          iqbufA=iqtmpA;
	}
        memcpy(iqbufA+iqszeA,bufadr[abuf],4*(nsampleA*nchannelA));

        if ((samplesA !=NULL) && 
            ((iqszeA+(nsampleA*nchannelA)*4)<(IQBUFSIZE/2))) {

          IQCopy( (int16 *) bufadr[abuf],samplesA+iqoffA,
                 nsampleA,nchannelA,roff,ioff,2,2*nsampleA);
         }

         if (userA !=NULL) (userA)(prmA,&tick,tock,*naveA,0,noiseA,
		                bufadr[abuf],abuf,nsampleA,
                        nchannelA,usrdataA);

        iqszeA+=(nsampleA*nchannelA)*4;
        iqoffA=iqszeA/2;
        iqnumA+=nsampleA;
        (*naveA)++;
      }

      if (statusB==1) {

        if (iqbufB==NULL) iqbufB=malloc(4*(nsampleB*nchannelB));
        else {
	  iqtmpB=realloc(iqbufB,iqszeB+4*(nsampleB*nchannelB));
          if (iqtmpB==NULL) {
            free(iqbufB);
            return -1;
	  }
          iqbufB=iqtmpB;
	}
        memcpy(iqbufB+iqszeB,bufadr[bufoff+abuf],4*(nsampleB*nchannelB));

        if ((samplesB !=NULL) && 
            ((iqszeB+(nsampleB*nchannelB)*4)<(IQBUFSIZE/2))) {

          IQCopy( (int16 *) bufadr[bufoff+abuf],samplesB+iqoffB,
                 nsampleB,nchannelB,roff,ioff,2,2*nsampleB);
        }

        if (userB !=NULL) (userB)(prmB,&tick,tock,*naveB,0,noiseB,
		                bufadr[bufoff+abuf],abuf,nsampleB,
                        nchannelB,usrdataB);
          
        iqszeB+=(nsampleB*nchannelB)*4;
        iqoffB=iqszeB/2;
        iqnumB+=nsampleB;         
        (*naveB)++;
      }



#ifdef _QNX4
      clock_gettime(CLOCK_REALTIME,&tick);
#endif
  }
  
  statusA=GC214GetScanStatus(drid[0])==SCAN_OK;
  statusB=GC214GetScanStatus(drid[1])==SCAN_OK;


  if (statusA==1) RawFeedScan(rawid[0],buf,nsampleA,nchannelA,
                              0,&tick);
  if (statusB==1) RawFeedScan(rawid[1],buf,nsampleB,nchannelB,
                              0,&tick);


  if (statusA==1) {
    markA[snumA].tval.tv_sec=tick.tv_sec;
    markA[snumA].tval.tv_nsec=tick.tv_nsec;
    markA[snumA].chn=nchannelA;
    markA[snumA].off=iqszeA;
    markA[snumA].sze=(nsampleA*nchannelA)*4;
    markA[snumA].atten=0;
    snumA++;
  }

  if (statusB==1) {
    markB[snumB].tval.tv_sec=tick.tv_sec;
    markB[snumB].tval.tv_nsec=tick.tv_nsec;
    markB[snumB].chn=nchannelB;
    markB[snumB].off=iqszeB;
    markB[snumB].sze=(nsampleB*nchannelB)*4;
    markB[snumB].atten=0;
    snumB++;
  }

  if ((samplesA !=NULL) && (statusA==1)) {
    seqtvalA[*naveA].tv_sec=tick.tv_sec;
    seqtvalA[*naveA].tv_nsec=tick.tv_nsec;
    seqattenA[*naveA]=0;
    seqnoiseA[*naveA]=*noiseA;
    seqoffA[*naveA]=iqszeA/2; /* in words */
    seqszeA[*naveA]=(nsampleA*nchannelA)*2; /* in words */
  }

  if ((samplesA !=NULL) && (statusB==1)) {
    seqtvalB[*naveB].tv_sec=tick.tv_sec;
    seqtvalB[*naveB].tv_nsec=tick.tv_nsec;
    seqattenB[*naveB]=0;
    seqnoiseB[*naveB]=*noiseB;
    seqoffB[*naveB]=iqszeB/2; /* in words */
    seqszeB[*naveB]=(nsampleB*nchannelB)*2; /* in words */
  }


  if (statusA==0) smpcntA++;
  if (statusB==0) smpcntB++;

  if (statusA==1) { 

    if (iqbufA==NULL) iqbufA=malloc(4*nsampleA*nchannelA);
    else {
      iqtmpA=realloc(iqbufA,iqszeA+4*(nsampleA*nchannelA));
      if (iqtmpA==NULL) {
        free(iqbufA);
        return -1;
      }
      iqbufA=iqtmpA;
    }
    memcpy(iqbufA+iqszeA,bufadr[buf],4*(nsampleA*nchannelA));

    if ((samplesA !=NULL) && 
       ((iqszeA+(nsampleA*nchannelA)*4)<(IQBUFSIZE/2))) {

          IQCopy( (int16 *) bufadr[buf],samplesA+iqoffA,
                 nsampleA,nchannelA,roff,ioff,2,2*nsampleA);
    }

    if (userA !=NULL) (userA)(prmA,&tick,tock,*naveA,0,noiseA,
		             bufadr[buf],buf,nsampleA,nchannelA,
			         usrdataA);

    iqszeA+=4*(nsampleA*nchannelA);
    iqoffA=iqszeA/2;
    iqnumA+=nsampleA;
    (*naveA)++;
   }

  if (statusB==1) { 

    if (iqbufB==NULL) iqbufB=malloc(4*nsampleB*nchannelB);
    else {
      iqtmpB=realloc(iqbufB,iqszeB+4*(nsampleB*nchannelB));
      if (iqtmpB==NULL) {
        free(iqbufB);
        return -1;
      }
      iqbufB=iqtmpB;
    }
    memcpy(iqbufB+iqszeB,bufadr[bufoff+buf],4*(nsampleB*nchannelB));

    if ((samplesB !=NULL) && 
        ((iqszeB+(nsampleB*nchannelB)*4)<(IQBUFSIZE/2))) {

          IQCopy( (int16 *) bufadr[bufoff+buf],samplesB+iqoffB,
                 nsampleB,nchannelB,roff,ioff,2,2*nsampleB);
    }
    if (userB !=NULL) (userB)(prmB,&tick,tock,*naveB,0,noiseB,
		             bufadr[bufoff+buf],buf,nsampleB,nchannelB,
			         usrdataB);


    iqszeB+=4*(nsampleB*nchannelB);
    iqoffB=iqszeB/2;
    iqnumB+=nsampleB;
    (*naveB)++;
   }

   if (smpcntA>MAX_SAMPLE_ERR) *flgA|=INT_SAMPLE_FAIL;
   if (smpcntB>MAX_SAMPLE_ERR) *flgB|=INT_SAMPLE_FAIL;

   RawFeedTrigger(rawid[0]);
   RawFeedTrigger(rawid[1]);
   if (*naveA>0) {
     ACFexCalculate(prmA,(int16 *) iqbufA,iqnumA,*naveA,nsampleA,
                   roff,ioff,
		   mplgsA,lagnumA,lagsA,lagsumA,
		   pwr0A,acfdA,noiseA);

     free(iqbufA);
   }

   if (*naveB>0) {
     ACFexCalculate(prmB,(int16 *) iqbufB,iqnumB,*naveB,nsampleB,
                   roff,ioff,
		   mplgsB,lagnumB,lagsB,lagsumB,
		   pwr0B,acfdB,noiseB);

     free(iqbufA);
   }

   /*
   DIOSetChannel(dioid,0);
   DIOSetGain(dioid,maxattenA);
   DIOSetChannel(dioid,1);
   DIOSetGain(dioid,maxattenB);
   */
   return 0;
} 





