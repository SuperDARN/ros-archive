/* omega_guess.c
   ============= */

/*
 $Log: omega_guess.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include "math_constant.h"

double omega_guess(struct complex *acf,double *tau,
	               int *badlag,double *phi_res,
                   double *omega_err,int mpinc,int mplgs) {

  int i,j,nave=0;
  double delta_tau, phi1, phi0, delta_phi, omega=0.0, 
         omega2=0.0, average=0.0, sigma, tau_lim=1.0;
  double sum_W=0.0, W;
  register double temp;
  double two_sigma;

  two_sigma = sigma = 2*PI;
  *omega_err = 9999.;

  while (tau_lim < 3 && nave < 3) {
    for (j=1; j<=tau_lim; ++j)
      for (i=0; i< mplgs - j; ++i) {
	    if (badlag[i+j] || badlag[i]) continue;
	    delta_tau = tau[i+j] - tau[i];
	    if (delta_tau != tau_lim) continue;
	    delta_phi = phi_res[i+j] - phi_res[i];
	    W = (cabs(acf[i]) + cabs(acf[i+j]))/2.0;  

	    if (delta_phi > PI) delta_phi = delta_phi - 2*PI;
	    if (delta_phi < -PI) delta_phi = delta_phi + 2*PI;

	    if ((average != 0.0) && (fabs(delta_phi - average) > two_sigma)) 
          continue;
	    temp = delta_phi/tau_lim;
	    omega = omega + temp*W;
	    omega2 = omega2 + W*(temp*temp);
	    sum_W = sum_W + W;
	    nave++;
	  }

    if (nave >= 3 && (sigma == 2*PI)) {
	  average = omega/sum_W;
	  sigma = (omega2/sum_W) - average*average;
	  sigma = (sigma > 0.0) ? sqrt(sigma) : 0.0;
	  two_sigma = 2.0*sigma;
	  omega = 0.0;
	  omega2 = 0.0; 
	  sum_W = 0;
	  nave = 0;
	  tau_lim = 1;
	} else if (nave >=3) {
	  omega = omega/sum_W;
	  omega = omega/(mpinc*1.0e-6);
	  *omega_err = sigma/(mpinc*1.0e-6);
	  return omega;
	}
    else ++tau_lim;
  }
  return 0.0;
}