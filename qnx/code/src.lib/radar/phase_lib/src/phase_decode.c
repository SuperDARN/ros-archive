/* phase_decode.c 
   ============== */

/*
$Log: phase_decode.c,v $
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

int phase_decode(struct tsg_parms *prm,int nchan,int16 *buf,
	            int *c_n,int *d_data) {
  int nsamp,max_samp;
  int nch,r,b,pulse;
  int range_inc;
  int s_index,r_index,temp,lag;
  int d_data_indx;
  nsamp = prm->nrang*prm->rsep*20/3/prm->smsep;
  max_samp = MAX_RANGE*prm->rsep*20/3/prm->smsep;

  range_inc = nchan;

  for (pulse = 0; pulse < prm->mppul; ++pulse) {
    lag = prm->pat[pulse]; /* lag where data from pulse starts*/
	s_index = (lag*prm->mpinc*nchan)/prm->smsep; /* index in input array */
		
    /* for each channel (real & imaginary, ACF and XCF) decode nranges */
	for (r = 0; r < nsamp; ++r) {
	  for( nch=0; nch<nchan; nch++ ) {
	    r_index = s_index + r*range_inc + nch;
		temp = 0;
				
		/* each range gate uses nbaud samples */

		for (b=0; b<prm->nbaud; ++b) 
		  temp = temp + buf[r_index + b*nchan] * 
                  prm->code[pulse*prm->nbaud + b];

		/* put decode value into appropriate output position */
		d_data_indx = nch*prm->mppul*max_samp + pulse*max_samp + r; 
		d_data[d_data_indx] = temp/prm->nbaud;
	  } 
	} 
  } 
  return 0;
}
  
 