/* acf.c 
   ===== */

/*
$Log: acf.c,v $
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


int phase_acf(struct tsg_parms *prm,int *real2,int *imag2,
	          int *real1,int *imag1,
		      int mplgs,int *lag_table,
		      int *acf_buf,
		      int change_atten) {

   int range;
   int nave,n;
   int real;
   int imag;
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


