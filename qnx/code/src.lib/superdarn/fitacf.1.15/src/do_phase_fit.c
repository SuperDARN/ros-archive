/* do_phase_fit.c
   ==============
   Author: K.Baker
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
 $Log: do_phase_fit.c,v $
 Revision 1.3  2004/05/04 22:54:21  barnes
 Changed math header name.

 Revision 1.2  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/08 22:20:22  barnes
 Initial revision

 */


/*
  This routine does the fitting to the phase of the ACF.  It is called
  from 'fit_acf'.

  status values returned by the function are:
    0 = OK
    16 = PHASE_OSCILLATION
    32 = NO_CONVERGENCE

  returned values:
    omega  -  (double) the least square error fit for the slope
    phi0 - (double) phase at lag 0 (may be non-zero for XCFs)
    sdev - (double) average standard deviation of the points in the fit
    omega_err - (double) 1-sigma error estimate of the slope
    phi0_err - (double) 1-sima error estimate on lag-0 phase
  input values:
    omega_guess - (double) initial guess for the value of omega
    xflag - (char) flag to indicate this is an XCF fit
    mplgs - (int) number of lags in the acf
    acf - (struct complex) the acf (or xcf) to be fitted
    tau - (double) array of lag values
    w - (double) array of weights (powers) for each lag
    sum_wk2_arr - (double) array of sum of w*k^2 
    phi_res - (double) array of measured phases
    badlag - (int) array of bad lag flags
    t0 - (double) basic time lag
    sum_w - (double) sum of the weights
    sum_wk - (double) sum of w*k
    sum_wk2 - (double) sum of w*k^2

*/

#include <math.h>
#include <stdio.h>
#include "rmath.h"

#define determ(aa,bb,cc,dd) (((aa)*(dd)) - ((bb)*(cc)))


int do_phase_fit (double omega_guess, 
		  char xflag, 
		  int mplgs,
		  struct complex acf[],
		  double tau[],
		  double w[],
		  double sum_wk2_arr[],
		  double phi_res[],
		  int badlag[],
		  double t0,
		  double sum_w,
		  double sum_wk,
		  double sum_wk2,
		  
		  double *omega,
		  double *phi0,
		  double *sdev,
		  double *phi0_err,
		  double *omega_err
		  )
{
  /*  local declarations */

  double omega_loc, omega_init;
  double omega_old_2=0.0, omega_old = 9999.0;
  int icnt = 0;

  double phi_loc = 0.0;
  double sum_phi = 0.0;
  double sum_kphi = 0.0;
  double phi_pred;
  double phi_tot;
  double phi_k[48];
  double t2;
  double wbar;

  int n_twopi;
  int nphi;
  int k;

  double d=0.0, e2;

  omega_loc = omega_guess;
  t2 = t0*t0;

  while (fabs(omega_old - omega_loc) > fabs(omega_loc * PI/64.)) {

    /* if omega_loc == omega_old_2 it means we are oscillating between
       two different values of omega */

    if (omega_loc == omega_old_2) {
      *omega = (omega_old + omega_loc)/2.;
      /* return the average value of the two omega values and return
	 with error code 16 */
      *phi0 = phi_loc;
      return 16;
    }

    /* if icnt >= 5 it means we aren't converging on a stable value for
       omega */

    if (++icnt >= 5) {
      /* return whatever we have at this moment
	 and set error code 32 */
      *omega = omega_loc;
      *phi0 = phi_loc;
      return 32;
    }

    omega_old_2 = omega_old;
    omega_old = omega_loc;
    omega_init = omega_loc;

    if (!xflag) phi_loc = 0.;

    sum_phi = atan2(acf[0].y,acf[0].x);
    sum_phi = sum_phi*w[0]*w[0];
    sum_kphi = 0.0;
    n_twopi = 0;
    nphi = 0;

    /* now go through the point, one at a time, predicting the new
       value for phi_tot from the current best value for omega */

    for (k=1; k<mplgs; k++) {
      if (badlag[k]) continue;
      phi_pred = phi_loc + omega_loc*tau[k]*t0;
      n_twopi = (PI + phi_pred - phi_res[k])/(2*PI) - 
					((omega_loc > 0) ? 0.0 : 1.0);
      phi_tot = phi_res[k] + n_twopi*(2*PI);
      if (fabs(phi_pred - phi_tot) > PI) {
	    if (phi_pred > phi_tot) phi_tot = phi_tot + 2*PI;
	    else phi_tot = phi_tot - 2*PI;
      }

      phi_k[k] = phi_tot;
      sum_phi = sum_phi + phi_tot*w[k]*w[k];
      sum_kphi = sum_kphi + tau[k]*phi_tot*w[k]*w[k];
      ++nphi;

      /* if this is the first time through the omega fit loop AND
  	  we are doing ACFs, NOT xcfs, and we've got enough points to
	  draw a line, THEN compute a new value of omega_loc as we add each
	  new point */

      if (!xflag && sum_wk2_arr[k] && (omega_old_2 == 9999.)) {
	    omega_loc = sum_kphi/(t0*sum_wk2_arr[k]);
	    omega_loc = (nphi*omega_loc + omega_init)/(nphi + 1);
      }
    }

    if (xflag) {
      d = determ(sum_w,sum_wk*t0,sum_wk*t0,sum_wk2*t2);
      if (d == 0) return 8;
      phi_loc = determ(sum_phi,sum_wk*t0,sum_kphi*t0,sum_wk2*t2)/d;
      omega_loc = determ(sum_w,sum_phi,sum_wk*t0,sum_kphi*t0)/d;
    } else {
      phi_loc = 0;
      if (sum_wk2 <= 0.0) return 8;
      omega_loc = sum_kphi/(t0*sum_wk2);
    }
  }
  /*  End of While loop */

  if (phi_loc > PI) phi_loc = phi_loc - 2*PI;

  *phi0 = phi_loc;
  *omega = omega_loc;

  /* Now we calculate the estimated error of the fit */

  wbar = 0;
  e2 = 0.;
  nphi = 0;
  for (k=0; k<mplgs; k++) {
    if (!badlag[k]) {
	    e2 += w[k]*w[k]*(phi_k[k] - phi_loc - omega_loc*tau[k]*t0)*
             (phi_k[k] - phi_loc - omega_loc*tau[k]*t0);
	    wbar += w[k];
	    nphi++;
    }
  }
  wbar = wbar/nphi;
  if (xflag) *sdev = sqrt(e2/(sum_w)/(nphi-2));
  else *sdev = sqrt(e2/sum_w/(nphi-1));

  if (xflag) {
    *phi0_err =  *sdev * wbar * sqrt(sum_wk2*t2/d);
    *omega_err = *sdev * wbar * sqrt(sum_w/d);
  }
  else {
    *phi0_err = 0;
    *omega_err = *sdev*wbar/sqrt(sum_wk2)/t0;
  }
  return 0;
}




