/*   badlags_s.c
     ===========
     Author: R.J.Barnes & Nigel Wade, A. Sessai Yukimatu,
     Original code Kile Baker
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

/*
 $Log: badlags_s.c,v $
 Revision 1.4  2004/06/16 22:43:14  barnes
 Incorporated Sessai's modifications.

 Revision 1.3  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.2  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.1  2002/02/05 22:44:52  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>

#include "badsmp.h"
#include "fitblk.h"

#include "rang_badlags.h"

int *badtmp=NULL;
int maxbad=MAXBAD;


void FitACFBadlagsStereo(struct FitPrm *ptr, struct FitACFBadSample *bptr) {
    int i, k, l, n, sample;
    long ts, t1=0, t2=0;
    int nbad;
    
    int offset;
    
    if (badtmp==NULL) {
	badtmp = malloc(maxbad*sizeof(int));
	if ( badtmp==NULL) {
	    fprintf( stderr, "badlags: memory allocation failure\n" );
	    return;
	}
    }
    
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
	
	/*  we now have a pulse that occurs after the current sample.  Start
	    incrementing the sample number until we find a sample that lies
	    within the pulse */
	
	while (ts < t1) {
	    sample++;
	    ts = ts + ptr->smsep;
	}
	
	/* ok, we now have a sample which occurs after the pulse starts.
	   check to see if it occurs before the pulse ends, and if so, mark
		it as a bad sample */
	
	while ((ts >= t1) && (ts <= t2)) {
	    if ( k > maxbad ) {	/* run out of storage, double the allocation */
		maxbad += MAXBAD;
		badtmp = realloc( badtmp,maxbad*sizeof(int));
		if (badtmp==NULL) {
		    fprintf( stderr, "badlags: memory allocation failure\n" );
		    return;
		}
	    }

	    badtmp[k] = sample;
	    k++;
	    sample++;
	    ts = ts + ptr->smsep;
	}
    }



    /* do it all again for the other half */

    i = -1;
    ts = (long) ptr->lagfr;
    sample = 0;

    t2 = 0L;

    /* offset is the offset in time between the transmission of 
       the two halves of stereo */

    offset=ptr->offset;
    if (ptr->channel==1) offset=-offset;

    if (offset==0) return;


    while ( offset != 0 && i < (ptr->mppul - 1) && k < maxbad ) {
	/* first, skip over any pulses that occur before the first sample */

	while ((ts > t2) && (i < (ptr->mppul - 1))) {
	    i++;
	    t1 = (long) (ptr->pulse[i]) * (long) (ptr->mpinc)
			    - ptr->txpl/2 + offset;
	    t2 = t1 + 3*ptr->txpl/2 + 100; /* adjust for rx-on delay */
	}	

	/*   we now have a pulse that occurs after the current sample.  Start
	     incrementing the sample number until we find a sample that lies
	     within the pulse */

	while (ts < t1)	{
	    sample++;
	    ts = ts + ptr->smsep;
	}
	
	/*  ok, we now have a sample which occurs after the pulse starts.
	    check to see if it occurs before the pulse ends, and if so, mark
	    it as a bad sample */

	while ((ts >= t1) && (ts <= t2) && k < maxbad ) {
	    if ( k > maxbad ) {	/* run out of storage, double the allocation */
		maxbad +=MAXBAD;
		badtmp = realloc(badtmp,maxbad*sizeof(int));
		if (badtmp==NULL) {
		    fprintf( stderr, "badlags: memory allocation failure\n" );
		    return;
		}
	    }
	    badtmp[k] = sample;
	    k++;
	    sample++;
	    ts = ts + ptr->smsep;
	}
    }

   
    nbad = k;	/* total number of bad samples */
    


    /* merge the two sets of badlags, sorting and removing duplicate entries */

    bptr->badsmp[0] = badtmp[0];
    n = 1;
    for ( i = 1; i < nbad; i++ ) {

      if ( n >= MAXBAD ) {
	fprintf( stderr, "badlags: internal storage for bad lags exceeded\n" );
	break;
      }

	for ( k = n-1; k >= 0; k-- ) {
	    if ( badtmp[i] == bptr->badsmp[k] )    /* duplicate, reject it */
		break;

	    if ( badtmp[i] > bptr->badsmp[k] ) {  
                /* put it in the list after the current entry */
		for ( l = n; l > k+1; l-- )
		    bptr->badsmp[l] = bptr->badsmp[l-1];
		bptr->badsmp[k+1] = badtmp[i];
		n++;
		break;
	    }

 	    if ( k == 0 ) {  	
            /* must be less than all entries, 
               put it in the list at the start */
		for ( l = n; l > 0; l-- )
		    bptr->badsmp[l] = bptr->badsmp[l-1];
		bptr->badsmp[0] = badtmp[i];
		n++;
	    }
	}
	
    }

    bptr->nbad = n;

    /* Now set up a table for checking range interference */
    r_overlap(ptr);
    return;
}

