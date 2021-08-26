/* badlags.c
   ========= */

/*
 $Log: badlags.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include "badsmp.h"
#include "fit_blk.h"

#include "rang_badlags.h"


void badlags(struct fit_prm *ptr,struct badsmp *bptr) {
  int i, k, sample;
  long ts, t1, t2;

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

void ckrng(int range,int *badlag,struct badsmp *bptr,
	       struct fit_prm *ptr) {
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
