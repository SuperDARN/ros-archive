/* calc_phi_res.c
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
 $Log: calc_phi_res.c,v $
 Revision 1.6  2004/05/04 22:54:21  barnes
 Changed math header name.

 Revision 1.5  2004/01/08 22:19:45  barnes
 Kile Baker's Modifications for improved velocity error and ground scatter
 determination.

 Revision 1.4  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

/*
  2003/09/04 Kile Baker
  changed the return type of this function from void to int
  the function now checks to see if all the phases turned out
  to be 0.  If so, it returns -1, otherwise it returns 0.

  The return code of -1 indicates that you can't really do a fit
  because all the phases were exactly 0.  This can happen when
  the power is low and we are reprocessing DAT files that have lost
  some precision.
*/

#include <math.h>
#include "rmath.h"



int calc_phi_res(struct complex *acf,int *badlag,
				  double *phi_res,int mplgs) {
  int i, nzc;
  for (i=0, nzc=0; i< mplgs; ++i) { 
	phi_res[i] = ((badlag[i]) ? 0.0 : atan2(acf[i].y, acf[i].x));
	if (phi_res[i] != 0.0) nzc++;
  }
  if (nzc > 0) return 0;
  else return -1;
}
