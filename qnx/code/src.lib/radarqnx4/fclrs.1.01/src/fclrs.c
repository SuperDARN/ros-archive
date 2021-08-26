/* fclrs.c
   =======
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
#include "limit.h"
#include "dio.h"
#include "ad.h" 
#include "freq.h"

#include "tsg.h"
#include "tsgtable.h"
#include "fclr.h"
#include "fclrs.h"

/*
 $Log: fclrs.c,v $
 Revision 1.1  2006/03/29 13:25:16  barnes
 Initial revision

*/

int FCLRS(ipcid_t dioid,ipcid_t *adid,ipcid_t *resetid,
         struct TSGtable *tab,int *tsgid,
         struct FreqTable *fptr,int *delay,
         int startA,int endA,int stepA,
         int startB,int endB,int stepB,
         void **bufadr,int *bufsize,int bufoff,
	     float mxpwrA,int maxattenA,
	     int *attenA,float *noiseA,
         float *statisticA,float *dcoA,int *flgA,int *freqA,
         float mxpwrB,int maxattenB,
         int *attenB,float *noiseB,
         float *statisticB,float *dcoB,int *flgB,int *freqB) {

  struct TSGprm *prm;
  int id;   
  int numchn;
  int lowfrqA[NUM_FCLR_FREQ]; 
  int lowfrqB[NUM_FCLR_FREQ];

  int nf = NUM_FCLR_FREQ;
  int i,j;


  *flgA=0;   
  *flgB=0;

  numchn = 2;      /* use only 2 channels of the DT2828 */
  id=tsgid[0];
  prm=&tab->buf[id];
 
  if  (FCLRSDoBase(id,prm->samples,delay,
  	       startA,endA,stepA,
           startB,endB,stepB,
		   bufadr,bufsize,bufoff,fptr,
           lowfrqA,statisticA,flgA,dcoA,
           lowfrqB,statisticB,flgB,dcoB,
           numchn,dioid,adid,resetid) !=0) return -1;
  id=tsgid[1];
  prm=&tab->buf[id];
  DIOSetChannel(dioid,0);

  *freqA=FCLRDoFine(id,prm->samples,lowfrqA,startA,
		  numchn,mxpwrA,maxattenA,&bufadr[0],
		  dioid,adid[0],resetid[0],attenA,noiseA,flgA,dcoA);
  

  /* compute DC Offset if requested */

  if (dcoA !=NULL) {    
    FCLRGetDCOffset(id,10,*freqA,4,maxattenA,
		     bufadr[0],dioid,adid[0],resetid[0],dcoA,flgA);     
  }
  DIOSetGain(dioid,*attenA);

  /* Remove any channel B frequencies within AB_FREQ_DIFF (15kHz) 
     of channel A, this should leave at least one frequency
  */

  for (i = 0; i < NUM_FCLR_FREQ;i++) {
    if (abs(lowfrqB[i] - *freqA) < AB_FREQ_DIFF) {
	  lowfrqB[i] = -1;
	  nf--;
	}
  }
  for (i = 0; i < NUM_FCLR_FREQ; i++) if (lowfrqB[i] != -1) break;
		
  /* fill the invalid frequencies with the best valid frequency */

  for (j = 0; j < NUM_FCLR_FREQ; j++) 
    if (lowfrqB[j] == -1) lowfrqB[j] = lowfrqB[i];

  DIOSetChannel(dioid,1);

  *freqB=FCLRDoFine(id,prm->samples,lowfrqB,startB,
		  numchn,mxpwrB,maxattenB,&bufadr[bufoff],
		  dioid,adid[1],resetid[1],attenB,noiseB,flgB,dcoB);
  

  /* compute DC Offset if requested */

  if (dcoB !=NULL) {    
    FCLRGetDCOffset(id,10,*freqB,4,maxattenB,
	     bufadr[bufoff],dioid,adid[1],resetid[1],dcoB,flgB);     
  }
  DIOSetGain(dioid,*attenB);
  if ((*freqA==FREQ_LOCAL) || (*freqB==FREQ_LOCAL)) return FREQ_LOCAL;
  return 0;
} 
