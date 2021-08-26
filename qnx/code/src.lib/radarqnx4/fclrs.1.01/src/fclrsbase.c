/* fclrsbase.c
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
#include "rtypes.h"
#include "limit.h"
#include "dio.h"
#include "ad.h" 
#include "freq.h"
#include "tsg.h"
#include "acf.h"
#include "fclr.h"
#include "fclrs.h"

/*
 $Log: fclrsbase.c,v $
 Revision 1.1  2006/03/29 13:25:16  barnes
 Initial revision

*/

int FCLRSDoBase(int tsgid,int samples,int *delay,
               int startA,int endA,int stepA,
               int startB,int endB,int stepB,
               void **bufadr,int *bufsize,int bufoff,
               struct FreqTable *fptr,
               int *lowfrqA,float *statisticA,int *flgA,float *dcoA,
               int *lowfrqB,float *statisticB,int *flgB,float *dcoB,
 	           int numchn,ipcid_t dioid,ipcid_t *adid,ipcid_t *resetid) {

 
   int numwordsA,numwordsB;
   int inpwrdsze;
   
   int status;
   int16 *buf;
     
   float lowpwrA[NUM_FCLR_FREQ];  /* corresponding power of lowest_freq */
   float lowpwrB[NUM_FCLR_FREQ];
 
   char seqid[2];
   int atten;
   /*  unsigned int timeout; - redundant code */
   int freq;
   float maxpower;	/* max power in the noise */
   float avepower;	/* average power */
   int dterr;
   int i,j,f;
   
   int frqnumA,frqnumB;
   int *frqtableA=NULL;
   int *frqtableB=NULL;
   float *pwrtableA=NULL;
   float *pwrtableB=NULL;
   float pwrsum=0;
   
   float *dptr=NULL;

   int *fadr[2];
   int fnum[2];


   seqid[0]=tsgid;
   seqid[1]=tsgid;

   *flgA=0;
   *flgB=0;

  
   
   /* build an array of frequencies */
   /* zeroes the lowest freq buffer */
   memset(lowfrqA,0,NUM_FCLR_FREQ*sizeof(int));
   memset(lowfrqB,0,NUM_FCLR_FREQ*sizeof(int));
   
   /* assign lowpwr with the maximum number */
   for(i = 0; i< NUM_FCLR_FREQ; i++) {
     lowpwrA[i] = MAX_LONG;
     lowpwrB[i] = MAX_LONG;
   }

   /* claim memory to store the frequency table */
   if ((frqtableA=malloc(sizeof(int)+
       sizeof(int)*(endA-startA)/stepA)) ==NULL) {
      return -1;
   }
   if ((frqtableB=malloc(sizeof(int)+
       sizeof(int)*(endB-startB)/stepB)) ==NULL) {
      free(frqtableA);
      return -1;
   }
  


   frqnumA=0;

   for (freq=startA;freq<=endA;freq+=stepA) {
       if (FreqTest(fptr,freq)==0) {
       frqtableA[frqnumA]=freq;
       frqnumA++;
     }		 
   }
   if(frqnumA == 0) {
      free(frqtableA);
      free(frqtableB);
      return -1;
   }

   frqnumB=0;

   for (freq=startB;freq<=endB;freq+=stepB) {
       if (FreqTest(fptr,freq)==0) {
       frqtableB[frqnumB]=freq;
       frqnumB++;
     }		 
   }

   if(frqnumB == 0) {
      free(frqtableA);
      free(frqtableB);
      return -1;
   }
    
   if ((pwrtableA=malloc(sizeof(float)*frqnumA))==NULL) {
      free(frqtableA);
      free(frqtableB);
      return -1;
   }

   if ((pwrtableB=malloc(sizeof(float)*frqnumB))==NULL) {
      free(frqtableA);
      free(frqtableB);
      free(pwrtableA);
      return -1;
   }


   numwordsA = samples  * numchn * frqnumA;
   numwordsB = samples  * numchn * frqnumB;


 
   /* check to see if the number of words exceed the buffer limit */

   inpwrdsze = (int) (bufsize[0] / sizeof(int16));
   if (numwordsA > inpwrdsze ) {
      frqnumA = (inpwrdsze / (samples*numchn)) - 1;
      numwordsA = samples*numchn * frqnumA;
      *flgA|=FCLR_BUF_OVF;
   }

   inpwrdsze = (int) (bufsize[1] / sizeof(int16));
   if (numwordsB > inpwrdsze ) {
      frqnumB = (inpwrdsze / (samples*numchn)) - 1;
      numwordsB = samples*numchn * frqnumB;
      *flgB|=FCLR_BUF_OVF;
   }





       
   /* turn off all attenuator */
   atten = ATTEN_OFF;

   if ((DIOSetChannel(dioid,0) !=0) || (DIOSetGain(dioid,atten) != 0)) {
      /* failed to set gain */
      free(frqtableA);
      free(frqtableB);
      free(pwrtableA);
      free(pwrtableB);
      *flgA|=FCLR_GAIN_FAIL;
      return -1;
   }

   if ((DIOSetChannel(dioid,1) !=0) || (DIOSetGain(dioid,atten) != 0)) {
      /* failed to set gain */
      free(frqtableA);
      free(frqtableB);
      free(pwrtableA);
      free(pwrtableB);
      *flgB|=FCLR_GAIN_FAIL;
      return -1;
   }





 
   /* calculate how much time out to wait for response in sendfclr */

   /* Note: This code appears to be redundant?

   timeout = (((MIN_SMSEP_STEP * samples) / 1000) * frqnum) / 50;
   timeout++;

   */

   dterr = 0;     /* initialize dterror count */

   if (ADDoScan(adid[0],0,numwordsA,0,numchn) !=0) {
     /* failed to set scan */
     free(frqtableA);
     free(frqtableB);
     free(pwrtableA);
     free(pwrtableB);
     *flgA|=FCLR_SCAN_FAIL;
     return -1;
   }

  if (ADDoScan(adid[1],0,numwordsB,0,numchn) !=0) {
     /* failed to set scan */
     free(frqtableA);
     free(frqtableB);
     free(pwrtableA);
     free(pwrtableB);
     *flgB|=FCLR_SCAN_FAIL;
     return -1;
   }

   fnum[0]=frqnumA;
   fnum[1]=frqnumB;
   fadr[0]=frqtableA;
   fadr[1]=frqtableB;

   if ((status=DIOSendFCLR(dioid,1,&seqid,delay,fnum,fadr)) != 0) { 
     if (status !=FREQ_LOCAL) {
       free(frqtableA);
       free(frqtableB);
       free(pwrtableA);
       free(pwrtableB);
       /* cannot do fclr */
       ADScanReset(adid[0],resetid[0]);
       ADScanReset(adid[1],resetid[1]);
       *flgA|=FCLR_FCLR_FAIL;
       *flgB|=FCLR_FCLR_FAIL;
	   return -1;
	 } else {
       /* frequency synthesizer in local mode */
       *flgA|=FCLR_LOCAL_MODE;
       *flgB|=FCLR_LOCAL_MODE;
     }
   }

   if (ADGetScanStatus(adid[0]) !=0) {
      /* scan failed to complete */
      free(frqtableA);
      free(frqtableB);
      free(pwrtableA);
      free(pwrtableB);
      *flgA|=FCLR_AD_FAIL;
      return -1;
   }

   if (ADGetScanStatus(adid[1]) !=0) {
      /* scan failed to complete */
      free(frqtableA);
      free(frqtableB);
      free(pwrtableA);
      free(pwrtableB);
      *flgB|=FCLR_AD_FAIL;
      return -1;
   }
  
   buf=bufadr[0];
   for (f=0;f < frqnumA;f++) {

     if (dcoA !=NULL) dptr=&dcoA[4*atten];
     else dptr=NULL;

     ACFSumProduct(buf,&avepower,samples,&maxpower,dptr);

     pwrtableA[f]=avepower;
     pwrsum+=avepower;

     /* check if avepower is one of the lowest power seen so far */
  
     if (avepower < lowpwrA[NUM_FCLR_FREQ-1]) {
       for(i = 0; i < NUM_FCLR_FREQ; i++) 
         if (avepower < lowpwrA[i]) break;
       j = NUM_FCLR_FREQ - 1;

       /* move everything down one notch */
       while(j > i ) {
         lowpwrA[j] = lowpwrA[j-1];
         lowfrqA[j] = lowfrqA[j-1];
         j--;
       }   
       lowpwrA[i] = avepower; 
       lowfrqA[i] = frqtableA[f];
      }
      buf = buf + (samples * numchn);
   } 

   if (statisticA !=NULL) {
     /* calculate the power statistics */

     statisticA[0]=pwrsum/frqnumA; /* find the average power */
     
   }
   pwrsum=0;
   buf=bufadr[bufoff];
   for (f=0;f < frqnumB;f++) {

     if (dcoB !=NULL) dptr=&dcoB[4*atten];
     else dptr=NULL;

     ACFSumProduct(buf,&avepower,samples,&maxpower,dptr);

     pwrtableB[f]=avepower;
     pwrsum+=avepower;

     /* check if avepower is one of the lowest power seen so far */
  
     if (avepower < lowpwrB[NUM_FCLR_FREQ-1]) {
       for(i = 0; i < NUM_FCLR_FREQ; i++) 
         if (avepower < lowpwrB[i]) break;
       j = NUM_FCLR_FREQ - 1;

       /* move everything down one notch */
       while(j > i ) {
         lowpwrB[j] = lowpwrB[j-1];
         lowfrqB[j] = lowfrqB[j-1];
         j--;
       }   
       lowpwrB[i] = avepower; 
       lowfrqB[i] = frqtableB[f];
      }
      buf = buf + (samples * numchn);
   } 

   if (statisticB !=NULL) {
     /* calculate the power statistics */

     statisticB[0]=pwrsum/frqnumB; /* find the average power */
     
   }
   free(pwrtableA);
   free(pwrtableB);
   free(frqtableA);
   free(frqtableB);
   return 0;
} 

