/* fclrbase.c
   ==========
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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

/*
 $Log: fclrbase.c,v $
 Revision 1.6  2004/06/16 22:03:08  barnes
 Added missing header.

 Revision 1.5  2004/05/11 15:38:18  barnes
 Changed buffer type.

 Revision 1.4  2004/05/07 22:28:44  barnes
 Reordered the calling sequence.

 Revision 1.3  2004/05/07 21:52:17  barnes
 Removed redundant code.

 Revision 1.2  2004/05/04 18:50:46  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/05/03 22:31:23  barnes
 Initial revision

*/

int FCLRDoBase(int tsgid,int samples,int start,
              int end,
              int step,
              void **bufadr,int bufsize,
              struct FreqTable *fptr,
              int *lowfrq,float *statistic,int *flg,float *dco,
	      int numchn,ipcid_t dioid,ipcid_t adid,ipcid_t resetid) {

 
   int numwords;
   int inpwrdsze;
   
   int status;
   int16 *ptr;
     
   float lowpwr[NUM_FCLR_FREQ];  /* corresponding power of lowest_freq */
  
   char seqid;
   int atten;
   unsigned int timeout;
   int freq;
   float maxpower;	/* max power in the noise */
   float avepower;	/* average power */
   int dterr;
   int i,j,f;
   
   int frqnum;
   int *frqtable=NULL;
   float *pwrtable=NULL;
   float pwrsum=0;
   
   float *dptr=NULL;

   seqid=tsgid;
   *flg=0;

   ptr=(int16 *) bufadr[0];
   
   /* build an array of frequencies */
   /* zeroes the lowest freq buffer */
   memset(lowfrq,0,NUM_FCLR_FREQ*sizeof(int));
   
   /* assign lowpwr with the maximum number */
   for(i = 0; i< NUM_FCLR_FREQ; i++)
     lowpwr[i] = MAX_LONG;

   /* claim memory to store the frequency table */
   if ((frqtable=malloc(sizeof(int)+
       sizeof(int)*(end-start)/step)) ==NULL) {
      return -1;
   }

   frqnum=0;
 
   for (freq=start;freq<=end;freq+=step) {
       if (FreqTest(fptr,freq)==0) {
       frqtable[frqnum]=freq;
       frqnum++;
     }		 
   }


   if(frqnum == 0) {
      free(frqtable);
      return -1;
   }

   if ((pwrtable=malloc(sizeof(float)*frqnum))==NULL) {
      free(frqtable);
      return -1;
   }

   numwords = samples  * numchn * frqnum;

 
   /* check to see if the number of words exceed the buffer limit */

   inpwrdsze = (int) (bufsize / sizeof(int16));
   if (numwords > inpwrdsze ) {
      frqnum = (inpwrdsze / (samples*numchn)) - 1;
      numwords = samples*numchn * frqnum;
      *flg|=FCLR_BUF_OVF;
   }
       
   /* turn off all attenuator */
   atten = ATTEN_OFF;
   if (DIOSetGain(dioid,atten) != 0) {
      /* failed to set gain */
      free(frqtable);
      *flg|=FCLR_GAIN_FAIL;
      return -1;
   }
 
   /* calculate how much time out to wait for response in sendfclr */

   timeout = (((MIN_SMSEP_STEP * samples) / 1000) * frqnum) / 50;
   timeout++;

   dterr = 0;     /* initialize dterror count */

   if (ADDoScan(adid,0,numwords,0,numchn) !=0) {
     /* failed to set scan */
     free(frqtable);
     *flg|=FCLR_SCAN_FAIL;
     return -1;
   }

   if ((status=DIOSendFCLR(dioid,0,&seqid,NULL,&frqnum,&frqtable)) != 0) { 
     if (status !=FREQ_LOCAL) {
       free(frqtable);
       /* cannot do fclr */
       ADScanReset(adid,resetid);
       *flg|=FCLR_FCLR_FAIL;
	   return -1;
	 } else {
       /* frequency synthesizer in local mode */
       *flg|=FCLR_LOCAL_MODE;
     }
   }

   if (ADGetScanStatus(adid) !=0) {
      /* scan failed to complete */
      free(frqtable);
      *flg|=FCLR_AD_FAIL;
      return -1;
   }


   for (f=0;f < frqnum;f++) {

     if (dco !=NULL) dptr=&dco[4*atten];
     else dptr=NULL;

     ACFSumProduct(ptr,&avepower,samples,&maxpower,dptr);

     pwrtable[f]=avepower;
     pwrsum+=avepower;

     /* check if avepower is one of the lowest power seen so far */
  
     if (avepower < lowpwr[NUM_FCLR_FREQ-1]) {
       for(i = 0; i < NUM_FCLR_FREQ; i++) 
         if (avepower < lowpwr[i]) break;
       j = NUM_FCLR_FREQ - 1;

       /* move everything down one notch */
       while(j > i ) {
         lowpwr[j] = lowpwr[j-1];
         lowfrq[j] = lowfrq[j-1];
         j--;
       }   
       lowpwr[i] = avepower; 
       lowfrq[i] = frqtable[f];
      }
      ptr = ptr + (samples * numchn);
   } 

   if (statistic !=NULL) {
     /* calculate the power statistics */

     statistic[0]=pwrsum/frqnum; /* find the average power */
     
   }

   free(pwrtable);
   free(frqtable);
   return 0;
} 

