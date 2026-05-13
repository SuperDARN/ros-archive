/* oldnoise_acf.c
   ==============
   Author: R.J.Barnes & K.Baker
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
 $Log: oldnoise_acf.c,v $
 Revision 1.1  2004/07/23 23:47:18  barnes
 Initial revision

*/

/*
 Old RCS Log:

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include "rmath.h"
#include "limit.h"
#include "oldbadsmp.h"
#include "fitblk.h"
#include "oldbadlags.h"
#include "oldnoise_stat.h"

#define PLIM 1.6

double oldnoise_acf(double mnpwr,struct FitPrm *ptr,
	             double *pwr, struct oldbadsmp *badsmp,
				 struct complex *raw[MAX_RANGE],
				 struct complex *n_acf) {
  int i, j;
  int np[LAG_SIZE];
  int bad[LAG_SIZE];
  double plim, P;

  for (i=0; i< ptr->mplgs; i++) {
	n_acf[i].x = 0;
	n_acf[i].y= 0;
	np[i] = 0;
  }
  plim = PLIM * mnpwr;

  for (i=0; i< ptr->nrang; i++) {
    if ((pwr[i] < plim) && ((fabs(raw[i][0].x) + 
			fabs(raw[i][0].y)) > 0) &&
			(fabs(raw[i][0].x) < plim) &&
			(fabs(raw[i][0].y) < plim)) {
	  oldckrng((i+1), bad,badsmp, ptr);

	  for (j=0; j< ptr->mplgs; j++) {
	    if ((fabs(raw[i][j].x) < plim) &&
			(fabs(raw[i][j].y) < plim) &&
			(bad[j] == 0)) {
		  n_acf[j].x = n_acf[j].x + raw[i][j].x;
		  n_acf[j].y = n_acf[j].y + raw[i][j].y;
		  ++(np[j]);
		}
	  }
	}
  }

  if (np[0] <= 2) {
	for (i=0; i < ptr->mplgs; ++i) {
	  n_acf[i].x = 0;
	  n_acf[i].y = 0;
	}
    return 0.0;
  }

  for (i=0; i< ptr->mplgs; i++) {
	if (np[i] > 2) {
	  n_acf[i].x = n_acf[i].x/np[i];
	  n_acf[i].y = n_acf[i].y/np[i];
	} else {
	  n_acf[i].x = 0;
	  n_acf[i].y= 0;
	}
  }

  /*  Now determine the average power in the non-zero lags of the noise acf */

  for (i=1, P=0; i < ptr->mplgs; ++i) {
	P = P + oldlag_power(&n_acf[i]);
  }
  P = P/(ptr->mplgs - 1);
  return P;
}
