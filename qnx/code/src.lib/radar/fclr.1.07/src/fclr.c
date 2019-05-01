/* fclr.c
   ======
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


/*
 $Log: fclr.c,v $
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

 Revision 1.1  2004/05/03 22:29:12  barnes
 Initial revision

*/

int FCLR(ipcid_t dioid,ipcid_t adid,ipcid_t resetid,
         struct TSGtable *tab,int *tsgid,struct FreqTable *fptr,
         int start,
	 int end,int step,void **bufadr,int bufsize,
	 float mxpwr,int maxatten,
	 int *atten,float *noise,
         float *statistic,float *dco,int *flg) {

   struct TSGprm *prm;
   int id;   
   int numchn;
   int lowfrq[NUM_FCLR_FREQ]; 

   int freq;
   *flg=0;   


   numchn = 2;      /* use only 2 channels of the DT2828 */
   id=tsgid[0];
   prm=&tab->buf[id];
   if  (FCLRDoBase(id,prm->samples,
		   start,end,step,
		   bufadr,bufsize,fptr,lowfrq,statistic,flg,dco,
                   numchn,dioid,adid,resetid) !=0) return -1;

   id=tsgid[1];
   prm=&tab->buf[id];
   freq=FCLRDoFine(id,prm->samples,lowfrq,start,
		  numchn,mxpwr,maxatten,bufadr,
		  dioid,adid,resetid,atten,noise,flg,dco);
  

   /* compute DC Offset if requested */

   if (dco !=NULL) {    
     FCLRGetDCOffset(id,10,freq,4,maxatten,
		     bufadr,dioid,adid,resetid,dco,flg);     
   }
  DIOSetGain(dioid,*atten);
  return freq;
} 
