/* phase_decode.c
   ==============
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
$Log: phase_decode.c,v $
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

int phase_decode(struct tsg_parms *prm,int nchan,int16 *buf,
	            int *c_n,float *d_data) {
  int nsamp,max_samp;
  int nch,r,b,pulse;
  int range_inc;
  int s_index,r_index,lag;
  float temp;
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
  
 
