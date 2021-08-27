/* phase_sum_power.c 
   ================= */

/*
$Log: sum_power.c,v $
Revision 1.1  1999/03/30 23:55:58  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "limit.h"
#include "do_phase.h"
#include "tsg.h"
#include "phase_acf.h"

#define DDATA(p,r,c) (d_data[ (c) * max_samp * prm->mppul + (p) * max_samp + (r)] )
#define SQUARE(x) ( (x) * (x) )


int phase_sum_power(struct tsg_parms *prm,int mplgs,
              int *lag_table,int *acf_pwr0,int mxpwr,
			  int *d_data,int range_offset,
			  int change_atten,int bad_range) {

  int range,j;
  int selec_range,temp_min_pwr;
  int max_range,max_samp;
  int nave;
  int pwr0[MAX_RANGE];
  int real_power;
  int imag_power;
  int max_pwr;
  int lag0,pulse;
  int min_pwr;
  int new_lag0_m_sample;
 
  max_samp = MAX_RANGE*prm->rsep*20/3/prm->smsep;
  nave =prm->rsep*20/3/prm->smsep;

  /* determine the pulse number to use for calculating lag0 power */
  lag0 = lag_table[0];
  pulse = lag_lookup(prm->pat,prm->mppul,lag0);

  max_range = prm->nrang;

  min_pwr = MAX_LONG;
  new_lag0_m_sample = 0;

  for(range=0; range < max_range; range++) {

    if((range >= bad_range)  && (new_lag0_m_sample == 0)) {
      new_lag0_m_sample =1;
      lag0=lag_table[mplgs];
      pulse = lag_lookup(prm->pat,prm->mppul,lag0);
    }

	real_power=0;
	imag_power=0;
	for(j=0; j<nave; j++){	
		real_power+=SQUARE( DDATA( pulse, nave*range+j, 0 ));
		imag_power+=SQUARE( DDATA( pulse, nave*range+j, 1 ));
	}   
    pwr0[range] = (real_power + imag_power)/nave; 
     
	if(change_atten) pwr0[range] = pwr0[range] / ATTEN_STEP;
    if (min_pwr > pwr0[range]) min_pwr = pwr0[range];
	
  } 
 
  /* accumulate the power in raw_data which is the output buffer */
 
  max_pwr = 0L;
  selec_range = 0;
  for(range=0; range < max_range;range++)  {
    acf_pwr0[range] = acf_pwr0[range] + pwr0[range];

    if(acf_pwr0[range] > max_pwr) {
      max_pwr = acf_pwr0[range];
      selec_range = range;
    }
  }

  /* set the attenuation flag to invalid range if 
     max_power is smaller than mxpwr */

  /* temp_min_pwr = 4 *   min_pwr;    */
 
  temp_min_pwr = 0;

  if((pwr0[selec_range] < mxpwr)  ||
     (pwr0[selec_range] < temp_min_pwr)) return -1;
  
  return selec_range;
}


