/* fclrfine.c
   ==========
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
#include "tsg.h"
#include "freq.h"
#include "fclr.h"
#include "acf.h"

/*
 $Log: fclrfine.c,v $
 Revision 1.6  2004/06/17 13:42:41  barnes
 Removed reset of the error flag.

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

int FCLRDoFine(int tsgid,int samples,int *lowfrq,
	     int start,int numchn,float mxpwr,int maxatten,
	     void **bufadr,
	     ipcid_t dioid,ipcid_t adid,ipcid_t resetid,
	     int *atten,float *noise,int *flg,float *dco) {
  int chosenfreq=0;
  int freq;
  int i;
  float maxpower;
  float avepower;
  int loop;
  int numbadatten;
  int status;
  unsigned char nflg=0;
  unsigned char seq;
  float *dptr=NULL;

  seq=tsgid;

  loop = 1;
  chosenfreq = lowfrq[0];
  *atten = ATTEN_OFF;

  /* now do the second search */

  while (loop ) {

    loop = 0;
    numbadatten = 0;
    *noise = MAX_LONG;

    /* try all NUM_FCLR_FREQ freqs selected in the first search */
    for(i = 0; i < NUM_FCLR_FREQ ;i++) {
      freq = lowfrq[i];
      if (freq == 0) freq = start;

      /* set the freq */
      if ( (status=DIOSetFreq(dioid,freq)) != 0) {
        /* report frequency error */
        *flg|=FCLR_FREQ_FAIL;
        return -1;
      }
		 
      /* start sampling */

      if (ADDoScan(adid,0,samples*numchn,0,numchn) !=0) {
        /* report ad error */
        *flg|=FCLR_SCAN_FAIL;
        return -1;
      }

      if (DIOSendTSG(dioid,0,&seq,&nflg,&nflg,NULL,NULL) != 0) { 
        ADScanReset(adid,resetid);
        *flg|=FCLR_TSG_FAIL;
        return -1;
      }
		 
      status=(ADGetScanStatus(adid)==SCAN_OK);
      if (status==1) {

        if (dco !=NULL) dptr=&dco[4*(*atten)];
        else dptr=NULL;
        ACFSumProduct((int16 *) bufadr[0],&avepower,samples,
			     &maxpower,dptr);

        /*  count the number of freqs that exceed mxpwr  */
        if (maxpower >= mxpwr)  numbadatten++;
         
        if (*noise > avepower) {
          *noise = avepower;
          chosenfreq = freq;
        }
      }


    } 

    /* check to see if it is necessary to increase attenuator */
    if ( (numbadatten >= NUM_FCLR_FREQ) && (*atten < maxatten) ) {
      (*atten)++;
      if (DIOSetGain(dioid,*atten) != 0) {
        /* report set gain error */
        *flg|=FCLR_GAIN_FAIL;
        return -1;
      }
      loop = 1;
    } 
  }
  return chosenfreq;
}

