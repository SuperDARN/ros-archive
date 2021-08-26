/* oldfit_acf.c
   ============
   Author: R.J.Barnes
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
 $Log: oldfit_acf.c,v $
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
#include <stdio.h>
#include "limit.h"
#include "oldbadsmp.h"
#include "fitblk.h"


#include "oldacf_preproc.h" 
#include "oldcalc_phi_res.h"
#include "oldomega_guess.h"
#include "oldbadlags.h"
#include "oldmore_badlags.h"
  

#define olddeterm(aa,bb,cc,dd) (aa*dd - bb*cc)

int oldfit_acf(struct complex *acf,int range,
             int *badlag,struct oldbadsmp *badsmp,int lag_lim,
			 struct FitPrm *prm,
		     double noise_lev_in,char xflag,double xomega,
		     struct FitRange *ptr) {
      
  double sum_np,sum_w,sum_wk,sum_wk2,sum_wk2_arr[LAG_SIZE],sum_wk4;
  double sum_p,sum_pk,sum_pk2,sum_phi,sum_kphi;
  double t0,t2,t4,phi_res[LAG_SIZE],phi_tot,phi_pred;
  int j,n_twopi,icnt;
  long k;
  int nphi, npp;
  double tau[LAG_SIZE], tau2[LAG_SIZE];
  double phi_k[LAG_SIZE]; 
  double omega_old, omega_old_2;
  double w[LAG_SIZE], pwr[LAG_SIZE];
  double wt[LAG_SIZE], wt2[LAG_SIZE], wp[LAG_SIZE];
  double e2;

  double c_log,c_log_err=0.0,d;
  double omega_loc, omega_err_loc, omega_init, phi_loc, noise_lev;
  double temp;

/*  The following variables have been added for version 2.0 of cfitacf */

  double P0, P0n;   /* this is the power level where the acf levels off */
  int last_good;
  int bad_pwr[LAG_SIZE];



/* the following array has been added to support preprocessing of the
   acf */

  int acf_stat=ACF_UNMODIFIED;

/* ----------------End of declarations ----------------------------------*/
  
  if (cabs(acf[0]) < noise_lev_in) {
    for (j=0; j<prm->mplgs; j++) badlag[j]=3;
    ptr->p_l = 0.0;
    ptr->p_s = 0.0;
    ptr->p_l_err = 0.0;
    ptr->p_s_err = 0.0;
    ptr->v = 0.0;
    ptr->v_err = 0.0;
    ptr->w_l = 0.0;
    ptr->w_l_err = 0.0;
    ptr->w_s = 0.0;
    ptr->w_s_err = 0.0;
    ptr->phi0 = 0.0;
    ptr->phi0_err = 0.0;
    ptr->sdev_l = 0.0;
    ptr->sdev_s = 0.0;
    return 2;
  }

  /* initialize the table of abs(acf[k]) and log(abs(acf[k])) */

  oldckrng(range, badlag, badsmp, prm);

  /* Save the original ACF in a new variable so we can try some
     preprocessing on it.
  */

/*  for (k=0; k < prm->mplgs; k++) {
    orig_acf[k].x = acf[k].x;
    orig_acf[k].y = acf[k].y;
  }
*/

  /* This next statement provides a hook for a routine to pre-process
    the ACF and return a modified ACF that will actually be fitted.

    The function acf_preproc should return a 0 if no change was made
    and a non-zero value otherwise - the actual non-zero value may be
    used to provide information about the change that was made.  Specifically
    the following values should be defined:
	   ACF_UNMODIFIED	  the ACF was unchanged
       ACF_GROUND_SCAT    the preprocessing indicates the ACF is ground
	   						scatter.
	   ACF_ION_SCAT		  the preprocessing indicates the ACF is Ionospheric
	   ACF_MIXED_SCAT	  the preprocessing indicates a mixture of
	   						ionospheric and ground scatter.

    The original acf will be in the array "orig_acf" and the modified
    acf will be in "acf".

    To write an acf_preprocessor you must use the calling convention below.
    The revised acf will be returned in the array acf.  You must also provide
    the value of the noise_level (noise_lev_in), the range number, the badlag
    table, and the number of lags
  */

  noise_lev = noise_lev_in;
/*
  if (noise_lev != 0.0) acf_stat = oldacf_preproc (acf, orig_acf, &noise_lev,
			range, badlag, prm->mplgs);
*/
  for (k=0; k<prm->mplgs; k++) {
    tau[k] = prm->lag[1][k] - prm->lag[0][k];
    tau2[k] = tau[k] * tau[k];
    w[k] = cabs(acf[k]) - noise_lev;
    if (w[k] <= 0.0) w[k] = 0.1;
  }

  /* we now have to compute the amount of power to subtract off the 
   power level at each range.  The amouont to be subtracted is P(0)/sqrt(nave)
   which is the approximate expectation value of the power level after the
   ACF has decorrelated. 

   [ To derive this, determine the expectation value of
   P**2 = R(tau)*conj(R(tau))]
  */

  P0n = w[0]/sqrt((double) prm->nave);
  if ((w[0] - P0n) < noise_lev) return 2;  
    /* give up if left over pwr is too low */

  /*  identify any additional bad lags */

  sum_np = oldmore_badlags(w, badlag, noise_lev, prm->mplgs,prm->nave);

  ptr->nump = (char) sum_np;

  /*  We must have at least lag_lim good lags */

  if (sum_np < lag_lim) return 4;

  if (noise_lev <= 0.0) noise_lev = 0.1; 
    /* this is required to make logs ok */


  /* OK, now we have determined the good lags for the phase fit.  
     Now subtract of P0n from the power profile */

  /* calculate the power values for each lag.  'w' is the linear power.
     wt is the power times the lag.  wt2 is power times lag**2.  
     pwr is the log of the power. wp is the linear power times the log of
     the power.  The items that involve the linear power are all parts of 
     the least squares fits with the weighting done by the linear power. */

  for (k=0; k<prm->mplgs; k++) {
    w[k] = w[k] - P0n;
    if (w[k] <= 0.0) w[k] = 0.1;
    wt[k] = w[k]*tau[k];
    wt2[k] = wt[k]*tau[k];
    pwr[k] = log(w[k]);
    wp[k] = w[k]*pwr[k];
  }
  /* we now have to check to see how many additional bad lags have been
     introduced by subtracting off P0n. */

  for (k=0, npp=0; k < prm->mplgs; k++) {
    if (w[k] < noise_lev && !badlag[k]) bad_pwr[k] = 1;
    else bad_pwr[k] = 0;
    if (! (badlag[k] || bad_pwr[k])) ++npp;
  }

 /* initialize the sums */

  sum_np = 1;
  sum_w = w[0];
  sum_wk = 0;
  sum_wk2 = 0;
  sum_wk2_arr[0] = 0;
  sum_wk4 = 0;
  sum_p = w[0]*pwr[0];
  sum_pk = 0;
  sum_pk2 = 0;
  phi_loc = atan2(acf[0].y, acf[0].x);
  sum_kphi = 0;
  t0 =  prm->mpinc * 1.0e-6;
  t2 = t0 * t0;
  t4 = t2 * t2;

  /* calculate all the residual phases */

  oldcalc_phi_res(acf, badlag, phi_res, prm->mplgs);

  if (!xflag) {
    if (acf_stat == ACF_GROUND_SCAT) omega_loc = 0.0;
    else omega_loc = oldomega_guess(acf, tau, badlag, phi_res, 
						&omega_err_loc,prm->mpinc,prm->mplgs);
    phi_k[0] = 0;
    sum_phi = 0;
  } else {
    phi_k[0] = phi_loc;
    sum_phi = phi_loc * w[0];
    omega_loc = xomega;
  }


  /*  The preliminaries are now over.  
  Now start the fitting process */

  /* first, calculate the sums needed for the phase fit */

  for (k=1; k<prm->mplgs; k++) {
    if (badlag[k]) {
      sum_wk2_arr[k] = sum_wk2_arr[k-1];
      continue;
    }
    sum_w = sum_w + w[k];
    sum_np = sum_np + 1;
    sum_wk = sum_wk + w[k]*tau[k];
    sum_wk2 = sum_wk2 + wt2[k];
    sum_wk2_arr[k] = sum_wk2;
  }

  /* do the phase fit using the current value of omega */

  omega_old = 9999.;
  omega_old_2 = omega_old;
  icnt = 0;

  while (fabs(omega_old - omega_loc) > fabs(omega_loc * PI/64.)) {
    if (omega_loc == omega_old_2) return 16;
    if (++icnt >= 5) return 32;
    omega_old_2 = omega_old;
    omega_old = omega_loc;
    omega_init = omega_loc;
    if (!xflag) phi_loc = 0.;
    sum_phi = atan2(acf[0].y,acf[0].x);
    sum_phi = sum_phi*w[0];
    sum_kphi = 0.0;
    n_twopi = 0;
    nphi = 0;
    for (k=1; k<prm->mplgs; k++) {
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
      sum_phi = sum_phi + phi_tot*w[k];
      sum_kphi = sum_kphi + tau[k]*phi_tot*w[k];
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
      d = olddeterm(sum_w,sum_wk*t0,sum_wk*t0,sum_wk2*t2);
      if (d == 0) return 8;
      phi_loc = olddeterm(sum_phi,sum_wk*t0,sum_kphi*t0,sum_wk2*t2)/d;
      omega_loc = olddeterm(sum_w,sum_phi,sum_wk*t0,sum_kphi*t0)/d;
    } else {
      phi_loc = 0;
      if (sum_wk2 <= 0.0) return 8;
      omega_loc = sum_kphi/(t0*sum_wk2);
    }
  }
  /*  End of While loop */

  if (phi_loc > PI) phi_loc = phi_loc - 2*PI;
  ptr->phi0 = phi_loc;
  ptr->v = omega_loc;

  /*  now calculate the error estimates on the phase fit */

  e2 = 0.;
  for (k=0; k<prm->mplgs; k++)
    if (!badlag[k])
	    e2 = e2 + w[k]*(phi_k[k] - phi_loc - omega_loc*tau[k]*t0)*
             (phi_k[k] - phi_loc - omega_loc*tau[k]*t0);
    ptr->sdev_phi = sqrt(e2/(sum_w));

  /* estimate the standard deviation of the ACF phase fit to be the
     geometric mean of the deviation of our best fit line, and 
     the variation in the 1-tau lags.
	
	 NOTE: the error estimates on the XCF velocity will probably be
	 smaller than the ACF estimates, but the XCF error is 
	 artificially low.  This is necessary in order to get
	 a proper error estimate on the lag-0 phase.
  */

  if (!xflag) ptr->sdev_phi = 
              sqrt(ptr->sdev_phi*omega_err_loc*t0);


  if (xflag) {
     ptr->phi0_err = ptr->sdev_phi*sqrt(sum_wk2/
                                (sum_w*sum_wk2 - sum_wk*sum_wk));
     ptr->v_err = ptr->sdev_phi*sqrt(sum_w/(t2*(sum_w*sum_wk2 -
                   sum_wk*sum_wk)));
  } else {
    ptr->phi0_err = 0.0;
    ptr->v_err = ptr->sdev_phi*sqrt(sum_w/(t2*sum_wk2));   
  }

  /* POWER FITS:  We now turn to the power fits.  The sums have to be
  partially redone, since we have subtracted P0n. */

  /* We are now faced with the question of what to do if we don't have enough 
  lags left to do a fit.  we can't abaondon the data because the phase fit is
  actually ok.  we have to have at least 3 points to do the fit and estimate 
  an error on the fit.

  If we don't have at least 3 good points, then simply set the lamda and
  sigma powers both to the power_lag0 level.  If there are only 2 good points
  then calculate the value of sigma and lamda, but set the error estimate
  to HUGE_VAL.

  If we end up with only lag-0 being good, then flag the width estimate
  by setting it to a large negative value.

*/

  if (npp < 3) {
    c_log = pwr[0];

    /* if c_log < 0 it means that after subtracting the noise and P0n,
     the result is less than 1.0.  This must really be pretty meaningless
     It shouldn't even be possible since we have supposedly already checked
     this at the beginning. */

    if (c_log < 0 ) return 2;    

    ptr->p_l = c_log;
    ptr->p_s = c_log;

    /* find the last good lag */
    last_good = -1;
    for (k= 0; k < prm->mplgs; k++) if (!badlag[k]) last_good = k;

    /* if there are no good lags, or only lag-0 is good, set the width
       to a high negative value, by setting the last_good lag to 1
       */

    if (last_good <=0 ) {
  	  ptr->w_l = -9999.0;
	  ptr->w_s = -9999.0;
	  ptr->p_l_err = HUGE_VAL;
	  ptr->p_s_err = HUGE_VAL;
	  ptr->w_l_err = HUGE_VAL;
	  ptr->w_s_err = HUGE_VAL;
	  ptr->sdev_l = HUGE_VAL;
	  ptr->sdev_s = HUGE_VAL;
    } else {
      /* now calculate the width as the lag-0 power divided by the
       time to the last good lag. */

      ptr->w_l = c_log/(tau[last_good]*t0);
      ptr->w_s = c_log/(tau2[last_good]*t2);

      /* set the errors to the maximum value */

      ptr->p_l_err = HUGE_VAL;
      ptr->p_s_err = HUGE_VAL;
      ptr->w_l_err = HUGE_VAL;
      ptr->w_s_err = HUGE_VAL;
      ptr->sdev_l = HUGE_VAL;
      ptr->sdev_s = HUGE_VAL;
    }
  } else {
    /*  Calculate the sums that were not used in the phase fit */
    for (k=1; k < prm->mplgs; k++) {
	  if (badlag[k] || bad_pwr[k]) continue;
	  sum_p = sum_p + wp[k];
	  sum_pk = sum_pk + pwr[k]*wt[k];
	  sum_pk2 = sum_pk2 + pwr[k]*wt2[k];
	  sum_wk4 = sum_wk4 + wt2[k]*tau2[k];
    }

    /* Now adjust the sums that were used in the phase fit, but that
       have changed because of additional bad lags */

    for (k=1; k< prm->mplgs; k++) {
	  if (bad_pwr[k]) {
	    sum_w = sum_w - w[k];
	    sum_np = sum_np - 1;
	    sum_wk = sum_wk - w[k]*tau[k];
	    sum_wk2 = sum_wk2 - wt2[k];
  	  }
    }
    
/*  start with the lamda fit */

    d = olddeterm(sum_w,-t0*sum_wk,t0*sum_wk,-t2*sum_wk2);
    c_log = olddeterm(sum_p,-t0*sum_wk,t0*sum_pk,-t2*sum_wk2)/d;
    ptr->w_l = olddeterm(sum_w,sum_p,t0*sum_wk,t0*sum_pk)/d;

    if (sum_np > 3) {
	  e2 = 0.;
	  for (k=0; k<prm->mplgs; k++)
	    if (badlag[k] == 0) {
		  temp = pwr[k] - (c_log - tau[k]*t0* (ptr->w_l));
		  e2 = e2 + w[k]*(temp*temp);
	    }
	  ptr->sdev_l = sqrt(e2/(sum_np - 2));
  	  P0 = exp(c_log) + P0n;
	  c_log = log(P0);
	  ptr->p_l = c_log;

	  if ((sum_w*sum_wk2 - sum_wk*sum_wk) <=0) {
	    ptr->p_l_err = HUGE_VAL;
	    ptr->w_l_err = HUGE_VAL;
	    ptr->sdev_l = HUGE_VAL;
	  } else {
	    c_log_err = ptr->sdev_l * 
		sqrt(sum_wk2/(sum_w*sum_wk2 - sum_wk*sum_wk));
	    ptr->p_l_err = c_log_err;

	    ptr->w_l_err = ptr->sdev_l * 
						sqrt(sum_w/(t2*(sum_w*sum_wk2 -
                         sum_wk*sum_wk)));
	  }
    } else {
	  ptr->p_l_err = HUGE_VAL;
	  ptr->w_l_err = HUGE_VAL;
	  ptr->sdev_l = HUGE_VAL;
    }
    

/* ----------------now do the sigma fit ------------------------ */

    d = olddeterm(sum_w,-t2*sum_wk2,t2*sum_wk2,-t4*sum_wk4);
    c_log = olddeterm(sum_p,-t2*sum_wk2,t2*sum_pk2,-t4*sum_wk4)/d;
    ptr->w_s = olddeterm(sum_w,sum_p,t2*sum_wk2,t2*sum_pk2)/d;

    if (sum_np > 3) {	
	  e2 = 0.;
	  for (k=0; k<prm->mplgs; k++)
	    if (badlag[k] == 0) {
		  temp = pwr[k] - (c_log - tau2[k]*t2* (ptr->w_s)); 
		  e2 = e2 + w[k]*(temp*temp);
	    }
	  ptr->sdev_s = sqrt(e2/(sum_np - 2));

	  if ((sum_w*sum_wk4 - sum_wk2*sum_wk2) <= 0.0 ) {
	    ptr->p_s_err = HUGE_VAL;
	    ptr->w_s_err = HUGE_VAL;
	    ptr->sdev_s = HUGE_VAL;
	  } else {
	    ptr->p_s_err = c_log_err;
	    c_log_err = ptr->sdev_s * 
	    sqrt(sum_wk4/(sum_w*sum_wk4 - sum_wk2*sum_wk2));
	    ptr->p_s_err = c_log_err;	    
	    ptr->w_s_err = ptr->sdev_s * 
				  sqrt(sum_w/(t4*(sum_w*sum_wk4 -
                  sum_wk2*sum_wk2)));
	  }
	  P0 = exp(c_log) + P0n;
	  c_log = log(P0);
	  ptr->p_s = c_log;
    } else {
	  ptr->p_s_err = HUGE_VAL;
	  ptr->w_s_err = HUGE_VAL;
	  ptr->sdev_s = HUGE_VAL;
    }	

    /* finally check for ground scatter fit */

    /*  first, see if an ACF preprocessor has already identified the
  	  scatter as being ground scatter.  */
	
    if (acf_stat == ACF_GROUND_SCAT) ptr->gsct = 1; 
    else {
      ptr->gsct = 0;
    }

    /* finally, adjust the standard deviations for the power values */
	
    ptr->sdev_l = ptr->sdev_l/sqrt(sum_w);
    ptr->sdev_s = ptr->sdev_s/sqrt(sum_w);
  }
    
  /* all done - return code = 1 */
  if (npp < 1) return 4;
  else return 1;
}


