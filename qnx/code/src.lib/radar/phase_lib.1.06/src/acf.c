/* acf.c
   =====
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

/*
$Log: acf.c,v $
Revision 1.4  2001/10/15 15:39:41  barnes
Moved to floating point internals.

Revision 1.3  2001/06/27 20:53:59  barnes
Added license tag

Revision 1.2  2001/01/29 18:11:50  barnes
Added Author Name

Revision 1.1  1999/03/30 23:55:58  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "limit.h"
#include "tsg.h"
#include "do_phase.h"

int lag_lookup(int *ptab,int mppul,int lag_val) {
  int i;
  for (i=0;(ptab[i] !=lag_val) && (i<mppul);i++);
  return i;
}


int phase_acf(struct tsg_parms *prm,float *real2,float *imag2,
	      float *real1,float *imag1,
		      int mplgs,int *lag_table,
		      float *acf_buf,
		      int change_atten) {

   int range;
   int nave,n;
   float real;
   float imag;
   int lag;
   int sample1;
   int sample2;
   int nrang;
   
   int normal[18]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

   nrang = prm->nrang;
   nave=prm->rsep*20/prm->smsep/3;
  	
   for(range=0; range<nrang*nave ; range+=nave) {
	 for(lag=0; lag<mplgs; lag++) {
       sample1 =lag_table[lag];        
       sample2 =lag_table[mplgs+lag];

       sample1=lag_lookup(prm->pat,prm->mppul,sample1);
       sample2=lag_lookup(prm->pat,prm->mppul,sample2);

   	   real=0;
	   imag=0;
	
   	   for(n=0; n<nave; n++) {
         real+=real2[sample2*MAX_RANGE*3+range+n] * 
               real1[sample1*MAX_RANGE*3+range+n] +
			   imag2[sample2*MAX_RANGE*3+range+n] * 
               imag1[sample1*MAX_RANGE*3+range+n];
         imag+=real1[sample1*MAX_RANGE*3+range+n] * 
               imag2[sample2*MAX_RANGE*3+range+n] -
			   real2[sample2*MAX_RANGE*3+range+n] * 
               imag1[sample1*MAX_RANGE*3+range+n];
	   }

       if(change_atten) {
          real = real / ATTEN_STEP;
          imag = imag / ATTEN_STEP;
       }
 		 
       acf_buf[range*(2*mplgs)+2*lag]+=real/nave/normal[lag];
       acf_buf[range*(2*mplgs)+2*lag+1]+=imag/nave/normal[lag];
     } 
   }
   return 0;
}  


