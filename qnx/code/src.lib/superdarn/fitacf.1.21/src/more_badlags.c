/* more_badlags.c
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
 $Log: more_badlags.c,v $
 Revision 1.5  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.4  2002/03/28 15:48:44  barnes
 Fixed bug in determining fluctuation level.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>
 
double more_badlags(double *w,int *badlag,
                   double noise_lev,int mplgs,int nave) {

  double fluct0, fluct, fluct_old;
  short int badflag_1, badflag_2, k_old, k;
  short int sum_np;

  badflag_1 = 0;
  badflag_2 = 0;

  fluct0 = w[0]/sqrt(2.0*(double) nave);
  fluct = w[0] + 2.0*noise_lev+fluct0;
  fluct_old = fluct;
  sum_np = 0;
  k_old = 0;

  for (k=0; k<mplgs; k++) {
   if (badlag[k]) continue;
   if (badflag_2) badlag[k]=7;
   else if (w[k] <= 1.0) { /* noise level already subtracted */
   	badlag[k] = 3;
   	badflag_2 = badflag_1;
   	badflag_1 = 1;
  } else {
    badflag_1 = 0;
   	if (w[k] > fluct) {
	  badlag[k] = 5;
	  if (k < (mplgs - 1)) {
		if ((w[k] < fluct_old) && (w[k+1] > fluct) &&
			(w[k+1] < w[k])) {
		     badlag[k_old] = 9;
		     --sum_np;
		     badlag[k] = 0;
		  }
        }
	  }
	  fluct_old = fluct;
   	  fluct = 2.0*noise_lev + w[k] + fluct0;
    }

    if (!badlag[k]) {
	  ++sum_np;
	  k_old = k; 
    }
  }
  return (double) sum_np;
}
