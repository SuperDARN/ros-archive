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
#include "rtypes.h"
#include "ad.h"
#include "dio.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"
#include "monitor.h"
#include "integrate.h"
#include "integrates.h"
#include "pulses.h"




/*
 $Log: integrates.c,v $
 Revision 1.1  2006/03/29 13:27:33  barnes
 Initial revision

*/


int IntegrateS(struct timespec *tock,int *delay,	      
               ipcid_t dioid,ipcid_t *adid,ipcid_t *monid,
               ipcid_t *resetid,void **bufadr,int bufoff,int thr,int lmt,
               struct TSGprm *prmA,int tsgidA,int mplgsA,
               int (*lagsA)[2],
 	           int badrngA,int maxattenA,float mxpwrA,
               float atstpA,int *attenA,
	           float *noiseA,float *pwr0A,
               float *acfdA,float *xcfdA,int *flgA,
	           float *dcoA,int *naveA,
               struct TSGprm *prmB,int tsgidB,int mplgsB,
               int (*lagsB)[2],
 	           int badrngB,int maxattenB,float mxpwrB,
               float atstpB,int *attenB,
	           float *noiseB,float *pwr0B,
               float *acfdB,float *xcfdB,int *flgB,
	           float *dcoB,int *naveB) {


  struct timespec tick;
  int abflgA,aflgA,oaflgA=0,catnA=0;
  int abflgB,aflgB,oaflgB=0,catnB=0;

  int i,j; 
  int abuf,buf;
 
  int nchannelA,nchannelB;
  int nsampleA,nsampleB;

  int smpcntA=0,smpcntB=0;
  int statusA=0,statusB=0;
  
  float *dptr=NULL;  
 
  *naveA=0;
  *naveB=0;
   
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
#endif
  while (1) {

      if (tick.tv_sec>tock->tv_sec) break;
      if ((tick.tv_sec==tock->tv_sec) && (tick.tv_nsec>tock->tv_nsec)) break;


      abuf = buf;
      buf = (buf + 1) % 2 ;

      statusA=ADGetScanStatus(adid[0])==SCAN_OK;
      statusB=ADGetScanStatus(adid[1])==SCAN_OK;

      if (statusA==0) smpcntA++;
      if (statusB==0) smpcntB++;

      if (statusA==1) MonitorScan(monid[0],abuf,nsampleA,nchannelA);
      if (statusB==1) MonitorScan(monid[1],abuf,nsampleB,nchannelB);
       

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
          
        aflgA=ACFSumPower(prmA,mplgsA,lagsA,pwr0A,
		         (int16 *) bufadr[abuf],2*nchannelA,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngA,
                         *noiseA,mxpwrA,catnA*atstpA,thr,lmt,&abflgA);
	
        if (abflgA==0) {

          (*naveA)++;

          if (dcoA !=NULL) dptr=&dcoA[4*(*attenA)];
          else dptr=NULL;


          ACFCalculate(prmA,(int16 *) bufadr[abuf],2*nchannelA,0,0,
                       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,
	  	       lagsA,acfdA,ACF_PART,2,badrngA,catnA*atstpA,dptr);

          if (xcfdA !=NULL) ACFCalculate(prmA,(int16 *) bufadr[abuf],
					2*nchannelA,0,0,
                    REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,
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
          
        aflgB=ACFSumPower(prmB,mplgsB,lagsB,pwr0B,
		         (int16 *) bufadr[bufoff+abuf],2*nchannelB,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngB,
                         *noiseB,mxpwrB,catnB*atstpB,thr,lmt,&abflgB);
	
        if (abflgB==0) {

          (*naveB)++;

          if (dcoB !=NULL) dptr=&dcoB[4*(*attenB)];
          else dptr=NULL;


          ACFCalculate(prmB,(int16 *) bufadr[bufoff+abuf],2*nchannelB,0,0,
                       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,
	  	       lagsB,acfdB,ACF_PART,2,badrngB,catnB*atstpB,dptr);

          if (xcfdB !=NULL) ACFCalculate(prmB,(int16 *) bufadr[bufoff+abuf],
					2*nchannelB,0,0,
                    REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,
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


  if (statusA==1) MonitorScan(monid[0],buf,nsampleA,nchannelA);
  if (statusB==1) MonitorScan(monid[1],buf,nsampleB,nchannelB);


  if (statusA==0) smpcntA++;
  if (statusB==0) smpcntB++;

  if (statusA==1) { 

     aflgA=ACFSumPower(prmA,mplgsA,lagsA,pwr0A,
		         (int16 *) bufadr[buf],2*nchannelA,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngA,
                         *noiseA,mxpwrA,catnA*atstpA,thr,lmt,&abflgA);

     if (abflgA==0) {

        if (dcoA !=NULL) dptr=&dcoA[4*(*attenA)];
        else dptr=NULL;

        ACFCalculate(prmA,(int16 *) bufadr[buf],2*nchannelA,0,0,
                     REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,
	  	     lagsA,acfdA,ACF_PART,2,badrngA,catnA*atstpA,dptr);

        if (xcfdA !=NULL) ACFCalculate(prmA,(int16 *) bufadr[buf],
				      2*nchannelA,0,0,
                      REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsA,
	  	              lagsA,xcfdA,XCF_PART,2,badrngA,
				      catnA*atstpA,dptr);

        (*naveA)++;
     }
   }

  if (statusB==1) { 

     aflgB=ACFSumPower(prmB,mplgsB,lagsB,pwr0B,
		         (int16 *) bufadr[bufoff+buf],2*nchannelB,0,0,
                         REAL_BUF_OFFSET,IMAG_BUF_OFFSET,badrngB,
                         *noiseB,mxpwrB,catnB*atstpB,thr,lmt,&abflgB);

     if (abflgB==0) {

        if (dcoB !=NULL) dptr=&dcoB[4*(*attenB)];
        else dptr=NULL;

        ACFCalculate(prmB,(int16 *) bufadr[bufoff+buf],2*nchannelB,0,0,
                     REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,
	  	     lagsB,acfdB,ACF_PART,2,badrngB,catnB*atstpB,dptr);

        if (xcfdB !=NULL) ACFCalculate(prmB,(int16 *) bufadr[bufoff+buf],
				      2*nchannelB,0,0,
                      REAL_BUF_OFFSET,IMAG_BUF_OFFSET,mplgsB,
	  	              lagsB,xcfdB,XCF_PART,2,badrngB,
				      catnB*atstpB,dptr);

        (*naveB)++;
     }
   }

   if (smpcntA>MAX_SAMPLE_ERR) *flgA|=INT_SAMPLE_FAIL;
   if (smpcntB>MAX_SAMPLE_ERR) *flgB|=INT_SAMPLE_FAIL;

   MonitorTrigger(monid[0]);
   MonitorTrigger(monid[1]);
   if (*naveA>0) ACFAverage(pwr0A,acfdA,xcfdA,*naveA,prmA->nrang,mplgsA);
   if (*naveB>0) ACFAverage(pwr0B,acfdB,xcfdB,*naveB,prmB->nrang,mplgsB);

   DIOSetChannel(dioid,0);
   DIOSetGain(dioid,maxattenA);
   DIOSetChannel(dioid,1);
   DIOSetGain(dioid,maxattenB);
 
   return 0;
} 





