/* sum_power.c
   ===========
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
$Log: sum_power.c,v $
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
#include "do_phase.h"
#include "tsg.h"
#include "phase_acf.h"

#define DDATA(p,r,c) (d_data[ (c) * max_samp * prm->mppul + (p) * max_samp + (r)] )
#define SQUARE(x) ( (x) * (x) )


int phase_sum_power(struct tsg_parms *prm,int mplgs,
              int *lag_table,float *acf_pwr0,float mxpwr,
			  float *d_data,int range_offset,
			  int change_atten,int bad_range) {

  int range,j;
  int selec_range,temp_min_pwr;
  int max_range,max_samp;
  int nave;
  float pwr0[MAX_RANGE];
  float real_power;
  float imag_power;
  float max_pwr;
  int lag0,pulse;
  float min_pwr;
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


