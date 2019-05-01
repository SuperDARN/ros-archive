/* badlags.c
   =========
   Author: R.J.Barnes & K. Baker
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
 $Log: badlags.c,v $
 Revision 1.5  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.4  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include "badsmp.h"
#include "fitblk.h"

#include "rang_badlags.h"


void FitACFBadlags(struct FitPrm *ptr,struct FitACFBadSample *bptr) {
  int i, k, sample;
  long ts, t1=0, t2=0;

  i = -1;
  ts = (long) ptr->lagfr;
  sample = 0;
  k = 0;

  t2 = 0L;

  while (i < (ptr->mppul - 1)) {
	/* first, skip over any pulses that occur before the first sample */

	while ((ts > t2) && (i < (ptr->mppul - 1))) {
      i++;
	  t1 = (long) (ptr->pulse[i]) * (long) (ptr->mpinc)
				- ptr->txpl/2;
	  t2 = t1 + 3*ptr->txpl/2 + 100; /* adjust for rx-on delay */
	}	

	/*	we now have a pulse that occurs after the current sample.  Start
		incrementing the sample number until we find a sample that lies
		within the pulse */

	while (ts < t1) {
      sample++;
	  ts = ts + ptr->smsep;
	}
	
	/*	ok, we now have a sample which occurs after the pulse starts.
		check to see if it occurs before the pulse ends, and if so, mark
		it as a bad sample */

	while ((ts >= t1) && (ts <= t2)) {
	  bptr->badsmp[k] = sample;
	  k++;
	  sample++;
	  ts = ts + ptr->smsep;
	}
  }
  bptr->nbad = k;	/* total number of bad samples */

  /* Now set up a table for checking range interference */
  r_overlap(ptr);
return;
}

/*	This routine uses the table set up by badlags to locate which lags
	are bad for a specified range */

void FitACFCkRng(int range,int *badlag,struct FitACFBadSample *bptr,
	       struct FitPrm *ptr) {
  int sam1, sam2, i, j;
  for (i=0; i<ptr->mplgs; i++) {
	badlag[i] = 0;
	sam1 = ptr->lag[0][i]*(ptr->mpinc/ptr->smsep)
			+ range - 1;
	sam2 = ptr->lag[1][i]*(ptr->mpinc/ptr->smsep)
			+ range - 1;

	for (j=0; j<bptr->nbad; j++) {
      if ((sam1 == bptr->badsmp[j]) || (sam2 == bptr->badsmp[j])) 
        badlag[i] = 1;
	  if (sam2 < bptr->badsmp[j]) break;
    }
  }
	
  /* This section of code is only of use to fitacf for reprocessing old
	data that used the 16 lag, 7 pulse code.  */
	
  if ((ptr->mplgs == 17) && (ptr->old !=0) )
	badlag[13] = 1;
	 
  /* finally, check for range interference */
  lag_overlap(range, badlag, ptr);
  return;
}
