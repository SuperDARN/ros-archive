/* integrates.c
   ============
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
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "rtypes.h"
#include "ad.h"
#include "dio.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"
#include "rawfeed.h"
#include "integrate.h"
#include "integrates.h"
#include "pulses.h"




/*
 $Log: integrates.c,v $
 Revision 1.6  2007/03/27 19:35:08  code
 Added code to prevent overruns.

 Revision 1.5  2006/08/23 19:35:15  code
 Added sample delay for digital receiver.

 Revision 1.4  2006/08/17 19:30:51  code
 Added support for extended lag tables.

 Revision 1.3  2006/07/24 16:45:00  code
 Passed in beam number and frequency.

 Revision 1.2  2006/04/07 15:22:43  barnes
 Added rawfeed system.

 Revision 1.1  2006/03/29 13:27:33  barnes
 Initial revision

*/


int IntegrateS(struct timespec *tock,int *delay,	      
               ipcid_t dioid,ipcid_t *adid,ipcid_t *rawid,
               ipcid_t *resetid,void **bufadr,int bufoff,int thr,int lmt,
               struct TSGprm *prmA,int tsgidA,int mplgsA,int lagnumA,
               int (*lagsA)[2],
 	           int badrngA,int bmnumA,int tfreqA,
	       int maxattenA,float mxpwrA,
               float atstpA,int *attenA,
	           float *noiseA,float *pwr0A,
               float *acfdA,float *xcfdA,int *flgA,
	           float *dcoA,int *naveA,
               struct TSGprm *prmB,int tsgidB,int mplgsB,int lagnumB,
               int (*lagsB)[2],
 	           int badrngB,int bmnumB,int tfreqB,
	       int maxattenB,float mxpwrB,
               float atstpB,int *attenB,
	           float *noiseB,float *pwr0B,
               float *acfdB,float *xcfdB,int *flgB,
	           float *dcoB,int *naveB,
               void (*userA)(struct TSGprm *prm,
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdata),void *usrdataA,
               void (*userB)(struct TSGprm *prm,
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdata),void *usrdataB) {


  struct timespec tick;
  struct timespec tack;
  int abflgA,aflgA,oaflgA=0,catnA=0;
  int abflgB,aflgB,oaflgB=0,catnB=0;

  int i,j; 
  int abuf,buf;
 
  int nchannelA,nchannelB;
  int nsampleA,nsampleB;

  int smpcntA=0,smpcntB=0;
  int statusA=0,statusB=0;
  
  float *dptr=NULL;  

  int lagmaxA=0;   
  int lagsumA[LAG_SIZE]; 

  int lagmaxB=0;   
  int lagsumB[LAG_SIZE]; 

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
    lagmaxA=lagnumA;
    for (j=0;j<lagnumA;j++) {
      i=abs(lagsA[j][0]-lagsA[j][1]);
      lagsumA[i]++;
    }
  } else lagmaxA=mplgsA;

  if (lagnumB !=0) {
    lagmaxB=lagnumB;
    for (j=0;j<lagnumB;j++) {
      i=abs(lagsB[j][0]-lagsB[j][1]);
      lagsumB[i]++;
    }
  } else lagmaxB=mplgsB;

  if (xcfdA !=NULL) nchannelA = 2;
  else nchannelA = 1;

  if (xcfdB !=NULL) nchannelB = 2;
  else nchannelB = 1;




  buf=0;
  *flgA=0;
  *flgB=0;

  nsampleA=prmA->samples; 
  nsampleB=prmB->samples;
  if (IntegrateSPulse(buf,nsampleA,nsampleB,
                      nchannelA,nchannelB,tsgidA,tsgidB,
                      delay,dioid,adid,resetid) != 0) {
    *flgA|=INT_PULSE_FAIL;
    *flgB|=INT_PULSE_FAIL;
    DIOSetChannel(dioid,0);
    DIOSetGain(dioid,maxattenA);
    DIOSetChannel(dioid,1);
    DIOSetGain(dioid,maxattenB);
 
   return -1;
  }
#ifdef _QNX4
  clock_gettime(CLOCK_REALTIME,&tick);
  clock_gettime(CLOCK_REALTIME,&tack);
#endif

  RawFeedSet(rawid[0],prmA,mplgsA,lagnumA,lagsA,0,(*attenA)*atstpA,*noiseA,
	     bmnumA,tfreqA,&tick);
  RawFeedSet(rawid[1],prmB,mplgsB,lagnumB,lagsB,0,(*attenB)*atstpB,*noiseB,
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

      statusA=ADGetScanStatus(adid[0])==SCAN_OK;
      statusB=ADGetScanStatus(adid[1])==SCAN_OK;

      if (statusA==0) smpcntA++;
      if (statusB==0) smpcntB++;

      if (statusA==1) RawFeedScan(rawid[0],abuf,nsampleA,nchannelA,
                                  (*attenA)*atstpA,&tick);
      if (statusB==1) RawFeedScan(rawid[1],abuf,nsampleB,nchannelB,
                                  (*attenB)*atstpB,&tick);
       

      if (IntegrateSPulse(buf,nsampleA,nsampleB,
                         nchannelA,nchannelB,tsgidA,tsgidB,delay,
                         dioid,adid,resetid) !=0)  {
        *flgA|=INT_PULSE_FAIL;
        *flgB|=INT_PULSE_FAIL;
        DIOSetChannel(dioid,0);
        DIOSetGain(dioid,maxattenA);
        DIOSetChannel(dioid,1);
        DIOSetGain(dioid,maxattenB);
        return -1;
      }

      if (statusA==1) {

        if (userA !=NULL) (userA)(prmA,&tick,tock,*naveA,(*attenA)*atstpA,
		                bufadr[abuf],abuf,nsampleA,nchannelA,
						      usrdataA);
          
        aflgA=ACFSumPower(prmA,lagmaxA,lagsA,pwr0A,
		         (int16 *) bufadr[abuf],2*nchannelA,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngA,
                         *noiseA,mxpwrA,catnA*atstpA,thr,lmt,&abflgA);
	
        if (abflgA==0) {

          (*naveA)++;

          if (dcoA !=NULL) dptr=&dcoA[4*(*attenA)];
          else dptr=NULL;


          ACFCalculate(prmA,(int16 *) bufadr[abuf],2*nchannelA,0,0,
                       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,lagnumA,
	  	       lagsA,acfdA,ACF_PART,2,badrngA,catnA*atstpA,dptr);

          if (xcfdA !=NULL) ACFCalculate(prmA,(int16 *) bufadr[abuf],
					2*nchannelA,0,0,
			    REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,lagnumA,
	  	            lagsA,xcfdA,XCF_PART,2,badrngA,
					catnA*atstpA,dptr);
          
          catnA = 0; 		 

          

          if  ((aflgA != -1) && (oaflgA == aflgA))  {
            aflgA = -1;
            if (*attenA < maxattenA) {
              ACFNormalize(pwr0A,acfdA,xcfdA,prmA->nrang,mplgsA,atstpA); 
              (*attenA)++;
              *noiseA=*noiseA/atstpA;
              if (*noiseA==0) *noiseA=1;
              ADGetScanStatus(adid[0]);
              DIOSetChannel(dioid,0);
              DIOSetGain(dioid,*attenA);
              catnA = 1;   
            } else *flgA|=INT_ATTEN_OVF;
       
          }
          oaflgA = aflgA;
	    }
      }

      if (statusB==1) {

        if (userB !=NULL) (userB)(prmB,&tick,tock,*naveB,(*attenB)*atstpB,
		                bufadr[bufoff+abuf],abuf,nsampleB,nchannelB,
						      usrdataB);
          
          
        aflgB=ACFSumPower(prmB,lagmaxB,lagsB,pwr0B,
		         (int16 *) bufadr[bufoff+abuf],2*nchannelB,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngB,
                         *noiseB,mxpwrB,catnB*atstpB,thr,lmt,&abflgB);
	
        if (abflgB==0) {

          (*naveB)++;

          if (dcoB !=NULL) dptr=&dcoB[4*(*attenB)];
          else dptr=NULL;


          ACFCalculate(prmB,(int16 *) bufadr[bufoff+abuf],2*nchannelB,0,0,
                       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,lagnumB,
	  	       lagsB,acfdB,ACF_PART,2,badrngB,catnB*atstpB,dptr);

          if (xcfdB !=NULL) ACFCalculate(prmB,(int16 *) bufadr[bufoff+abuf],
					2*nchannelB,0,0,
			    REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,lagnumB,
	  	            lagsB,xcfdB,XCF_PART,2,badrngB,
					catnB*atstpB,dptr);
          
          catnB = 0; 		 

          

          if  ((aflgB != -1) && (oaflgB == aflgB))  {
            aflgB = -1;
            if (*attenB < maxattenB) {
              ACFNormalize(pwr0B,acfdB,xcfdB,prmB->nrang,mplgsB,atstpB); 
              (*attenB)++;
              *noiseB=*noiseB/atstpB;
              if (*noiseB==0) *noiseB=1;
              ADGetScanStatus(adid[1]);
              DIOSetChannel(dioid,1);
              DIOSetGain(dioid,*attenB);
              catnB = 1;   
            } else *flgB|=INT_ATTEN_OVF;
       
          }
          oaflgB = aflgB;
	    }
      }



#ifdef _QNX4
      clock_gettime(CLOCK_REALTIME,&tick);
#endif
  }
  
  statusA=ADGetScanStatus(adid[0])==SCAN_OK;
  statusB=ADGetScanStatus(adid[1])==SCAN_OK;


  if (statusA==1) RawFeedScan(rawid[0],buf,nsampleA,nchannelA,
                              (*attenA)*atstpA,&tick);
  if (statusB==1) RawFeedScan(rawid[1],buf,nsampleB,nchannelB,
                              (*attenB)*atstpB,&tick);


  if (statusA==0) smpcntA++;
  if (statusB==0) smpcntB++;

  if (statusA==1) { 

     if (userA !=NULL) (userA)(prmA,&tick,tock,*naveA,(*attenA)*atstpA,
		             bufadr[buf],buf,nsampleA,nchannelA,
			     usrdataA);


     aflgA=ACFSumPower(prmA,lagmaxA,lagsA,pwr0A,
		         (int16 *) bufadr[buf],2*nchannelA,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngA,
                         *noiseA,mxpwrA,catnA*atstpA,thr,lmt,&abflgA);

     if (abflgA==0) {

        if (dcoA !=NULL) dptr=&dcoA[4*(*attenA)];
        else dptr=NULL;

        ACFCalculate(prmA,(int16 *) bufadr[buf],2*nchannelA,0,0,
                     REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,lagnumA,
	  	     lagsA,acfdA,ACF_PART,2,badrngA,catnA*atstpA,dptr);

        if (xcfdA !=NULL) ACFCalculate(prmA,(int16 *) bufadr[buf],
				      2*nchannelA,0,0,
			      REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,lagnumA,
	  	              lagsA,xcfdA,XCF_PART,2,badrngA,
				      catnA*atstpA,dptr);

        (*naveA)++;
     }
   }

  if (statusB==1) { 

     if (userB !=NULL) (userB)(prmB,&tick,tock,*naveB,(*attenB)*atstpB,
		             bufadr[bufoff+buf],buf,nsampleB,nchannelB,
			     usrdataB);


     aflgB=ACFSumPower(prmB,lagmaxB,lagsB,pwr0B,
		         (int16 *) bufadr[bufoff+buf],2*nchannelB,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngB,
                         *noiseB,mxpwrB,catnB*atstpB,thr,lmt,&abflgB);

     if (abflgB==0) {

        if (dcoB !=NULL) dptr=&dcoB[4*(*attenB)];
        else dptr=NULL;

        ACFCalculate(prmB,(int16 *) bufadr[bufoff+buf],2*nchannelB,0,0,
                     REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,lagnumB,
	  	     lagsB,acfdB,ACF_PART,2,badrngB,catnB*atstpB,dptr);

        if (xcfdB !=NULL) ACFCalculate(prmB,(int16 *) bufadr[bufoff+buf],
				      2*nchannelB,0,0,
			      REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,lagnumB,
	  	              lagsB,xcfdB,XCF_PART,2,badrngB,
				      catnB*atstpB,dptr);

        (*naveB)++;
     }
   }

   if (smpcntA>MAX_SAMPLE_ERR) *flgA|=INT_SAMPLE_FAIL;
   if (smpcntB>MAX_SAMPLE_ERR) *flgB|=INT_SAMPLE_FAIL;

   RawFeedTrigger(rawid[0]);
   RawFeedTrigger(rawid[1]);
   if (*naveA>0) {
      if (lagnumA==0) ACFAverage(pwr0A,acfdA,xcfdA,NULL,
				 *naveA,prmA->nrang,mplgsA);

      else ACFAverage(pwr0A,acfdA,xcfdA,lagsumA,
		      *naveA,prmA->nrang,mplgsA);
   }

   if (*naveB>0) {
      if (lagnumB==0) ACFAverage(pwr0B,acfdB,xcfdB,NULL,
				 *naveB,prmB->nrang,mplgsB);

      else ACFAverage(pwr0B,acfdB,xcfdB,lagsumB,
		      *naveB,prmB->nrang,mplgsB);
   }


   DIOSetChannel(dioid,0);
   DIOSetGain(dioid,maxattenA);
   DIOSetChannel(dioid,1);
   DIOSetGain(dioid,maxattenB);
 
   return 0;
} 





