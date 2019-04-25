/* oldfit_noise.c
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
#include "oldbadsmp.h"
#include "fitblk.h"
#include "oldfit_acf.h"

void oldfit_noise(struct complex *ncf,int *badlag,struct oldbadsmp *badsmp,
			   double skynoise,struct FitPrm *prm,
			   struct FitRange *ptr) {
 
  int lag_lim;
  int istat, j;
  double A, tau, t;
  

  lag_lim = prm->mplgs/2;	/* require at least 1/2 of lags be OK*/

  istat = oldfit_acf(ncf, 1, badlag, badsmp, lag_lim, prm, 0.0, 0,0.0,ptr);

  tau = prm->mpinc * 1.0e-6;

  if (istat ==1 && (ptr->v == 0.0)) ptr->v = 1.0;
  if (istat == 1 && (fabs(ptr->v_err/ptr->v) < 0.05)) {
    if (ptr->w_l_err <= ptr->w_s_err) {
      if (ptr->p_s > skynoise) ptr->p_s = skynoise;
      A = exp(ptr->p_s);
      for (j=0; j < prm->mplgs; ++j) {
        t = (prm->lag[1][j] - prm->lag[0][j])*tau;
        ncf[j].x = A*exp(-ptr->w_l*t)*cos(ptr->v*t);
        ncf[j].y = A*exp(-ptr->w_l*t)*sin(ptr->v*t);
      }
    } else {
      if (ptr->p_s > skynoise) ptr->p_s = skynoise;
      A = exp(ptr->p_s);
      for (j=0; j < prm->mplgs; ++j) {
        t = (prm->lag[1][j] - prm->lag[0][j])*tau;
        ncf[j].x = A*exp(-(ptr->w_s*t)*(ptr->w_s*t))*cos(ptr->v*t);
        ncf[j].y = A*exp(-(ptr->w_s*t)*(ptr->w_s*t))*sin(ptr->v*t);
      }
    }
  } else
    for (j=0; j < prm->mplgs; ++j) {
      ncf[j].x = 0;
      ncf[j].y = 0;
    }
  return;
}
