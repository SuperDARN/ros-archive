/* do_fit.c
   ======== */

/*
 $Log: do_fit.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>
#include "math_constant.h"

#include "badsmp.h"
#include "fit_blk.h"
#include "dbl_cmp.h"
#include "badlags.h"
#include "fit_acf.h"
#include "noise_stat.h"
#include "noise_acf.h"
#include "fit_noise.h"
#include "remove_noise.h"
#include "elevation.h"
#include "elev_goose.h"
#include "ground_scatter.h"

int do_fit(struct fit_block *iptr,int lag_lim,int goose,
		   struct range_data *ptr,struct range_data *xptr,struct elv *elv,
		   struct noise *nptr) {

  struct badsmp badsmp;
  struct range_data nfit;
  struct complex ncfd[LAG_TAB_LEN];
  struct complex nxcf[LAG_TAB_LEN];
  int badlag[MAX_RANGE][LAG_TAB_LEN];

  int i=0,j,k, istat;
  char gflag;
 
  long tempdat[LAG_TAB_LEN][2];
  double pwrd[MAX_RANGE],pwrt[MAX_RANGE];
  double mnpwr, skylog, freq_to_vel, range;
  double xomega=0.0;

  double noise_pwr, ave_noise_pwr, Noise_signal, 
	     first_mnpwr, first_noise_pwr;
  int nfirst, nflag, ni;

  if (iptr->prm.nave <= 1) return 0;

  freq_to_vel = C/(4*PI)/(iptr->prm.tfreq * 1000.0);

  badlags(&iptr->prm,&badsmp);	/* determine what samples are bad */
  /* Determine the lag_0 noise level (0 dB reference) and the noise level at 
   which fit_acf is to quit (average power in the 
   fluctuations of the acfs which are pure noise) */

  /* If there is a coherent noise transmitter, the noise determination can
   actually be done twice.  The first time to identify and remove the
   coherent signal and the second time to determine the residual noise */

  nfirst = 1;
  nflag = 1;  

  while (nflag) {
	for (i=0; i < iptr->prm.nrang; i++) {
	  pwrd[i] = (double) iptr->prm.pwr0[i];	/* transfer powers into local array */
	  pwrt[i] = pwrd[i];
	}
	qsort(pwrt, iptr->prm.nrang, sizeof(double), dbl_cmp);
	/* determine the average lag0 power of the 10 lowest power acfs */
	
	ni = 0;
	i = 0;
	mnpwr = 0.0;
	
	/*  look for the lowest 10 values of lag0 power and average to 
		get the noise level.  Ignore values that are exactly 0.  If
		you can't find 10 useable values within the first 1/3 of the
		sorted power list, then just use whatever you got in that 
		first 1/3.  If you didn't get any useable values, then use
		the NOISE parameter */
		
	while ((ni < 10) && (i < iptr->prm.nrang/3)) {
      mnpwr += pwrt[i++];
	  if (mnpwr) ++ni;
	}

	ni = (ni > 0) ? ni :  1;
	mnpwr = mnpwr/ni;
	if (mnpwr < 1.0) mnpwr = iptr->prm.noise;
	nptr->skynoise = mnpwr;
  
/* Now determine the level which will be used as the cut-off power 
	for fit_acf.  This is the average power at all non-zero lags of all
	acfs which have lag0 power < 1.6*mnpwr + 1 stnd. deviation from that
	average power level */

	noise_pwr = noise_stat(mnpwr,&iptr->prm,&badsmp,
                           iptr->acfd, &Noise_signal);

	/* if this is the second time through */
    /*   take geometric mean of result this time with last time */
	if (!nfirst)	noise_pwr = sqrt(noise_pwr * first_noise_pwr);
		

    /* If there appears to be a noise signal, determine a "noise acf" */

	if (nfirst && (Noise_signal > 0.0)) {
      nfirst = 0;
	  ave_noise_pwr = noise_acf(mnpwr,&iptr->prm,pwrd,&badsmp, 
						        iptr->acfd, ncfd);
      if (iptr->prm.xcf) noise_acf(mnpwr,&iptr->prm,pwrd,&badsmp,
							  iptr->xcfd, nxcf);
      nptr->lag0=ncfd[0].x;

  /* compare the average noise power in the noise acf with the average
	 calculated from all the noise ranges independently.  If the average
	 power in the noise acf is great than about 1/2 of the independent
	 average, then there is a significant coherent signal that can
         be removed.
	 Otherwise, there is no coherent signal, so skip the noise removal */

  	  if (ave_noise_pwr > noise_pwr/2.0) {
	     /* run fit_acf on the noise acf and xcf */
         if (iptr->prm.xcf) 
           fit_noise(nxcf,badlag[0],&badsmp,nptr->skynoise,&iptr->prm,&nfit);
		 fit_noise(ncfd,badlag[0],&badsmp,nptr->skynoise,
			   &iptr->prm,&nfit);
		 nptr->vel = freq_to_vel * nfit.v;

		if (nfit.p_l <= 0.0) nflag = 0; /* if a fit for the noise */
		else nflag = 1.0; /* set the flag to 1 to re-do noise calc */
	  } else nflag = 0;	/* no coherent noise signal */

	  if (nflag) { 
	    for (i = 0; i < iptr->prm.nrang; i++) {
		  remove_noise(iptr->prm.mplgs,iptr->acfd[i], ncfd); 
		  iptr->prm.pwr0[i] = iptr->acfd[i][0].x;
		  if (iptr->prm.xcf) 
                    remove_noise(iptr->prm.mplgs,iptr->xcfd[i],nxcf);
		}

		first_mnpwr = mnpwr;
		first_noise_pwr = noise_pwr;
	  } else i=0; /* dummy statement to end the if(nflag) */
    } else {
      /* noise acf is not sufficently coherent */
	  Noise_signal = 0.0;
	  nflag = 0;
	  nfirst = 0;
	}
  }	/* end the while loop */

  /*	convert the lag0 powers to dB */

  if (nptr->skynoise > 0.0) skylog = 10.0 * log10(nptr->skynoise);
  else skylog = 0.0;

  for (i=0; i<iptr->prm.nrang; i++) {	
    pwrd[i] = pwrd[i] - nptr->skynoise;
	if (pwrd[i] <= 0.0) ptr[i].p_0 = -50.0;
	else ptr[i].p_0 = 10.0*log10(pwrd[i]) - skylog;
  }

  /*	reset the output arrays */

  for (i=0; i<MAX_RANGE; i++) {
	ptr[i].p_l = -50.0;
	ptr[i].p_s = -50.0;
	ptr[i].p_l_err= 0.0;
	ptr[i].p_s_err= 0.0;
	ptr[i].w_l = 0.0;
	ptr[i].w_s = 0.0;
	ptr[i].w_l_err = 0.0;
	ptr[i].w_l_err = 0.0;
	ptr[i].v = 0.0;
	ptr[i].v_err = 0.0;
	ptr[i].phi0 = 0.0;
	ptr[i].phi0_err=0.0;
	ptr[i].sdev_l = 0.0;
	ptr[i].sdev_s = 0.0;
	ptr[i].sdev_phi = 0.0;
	ptr[i].gsct = 0.0;
        ptr[i].qflg = 0;
	ptr[i].nump=0;

	xptr[i].p_l = -50.0;
	xptr[i].p_s = -50.0;
	xptr[i].p_l_err= 0.0;
	xptr[i].p_s_err= 0.0;
	xptr[i].w_l = 0.0;
	xptr[i].w_s = 0.0;
	xptr[i].w_l_err = 0.0;
	xptr[i].w_l_err = 0.0;
	xptr[i].v = 0.0;
	xptr[i].v_err = 0.0;
	xptr[i].phi0 = 0.0;
	xptr[i].phi0_err=0.0;
	xptr[i].sdev_l = 0.0;
	xptr[i].sdev_s = 0.0;
	xptr[i].sdev_phi = 0.0;
	xptr[i].gsct = 0.0;
	xptr[i].qflg = 0;
    xptr[i].nump=0;

	elv[i].normal= 0.0;
	elv[i].low = 0.0;
	elv[i].high = 0.0;


  }

 /* ----------------------------------------------------------------------*/
 /* 	Now do the fits for each acf */

  for (k=0, i=0; k<iptr->prm.nrang;k++) {

	ptr[k].qflg = fit_acf(iptr->acfd[k], k+1,badlag[k],&badsmp,
						   lag_lim,&iptr->prm,noise_pwr,0,0.0,&ptr[k]);
    xomega=ptr[k].v;
	if (ptr[k].qflg == 1)	{
      /* convert power from natural log to dB */

	  ptr[k].p_l = ptr[k].p_l*LN_TO_LOG - skylog;
	  ptr[k].p_s = ptr[k].p_s*LN_TO_LOG - skylog;
	  ptr[k].p_l_err = ptr[k].p_l_err*LN_TO_LOG;
	  ptr[k].p_s_err = ptr[k].p_s_err*LN_TO_LOG;

	  /* convert Doppler frequency to velocity */

	  ptr[k].v = iptr->prm.vdir*freq_to_vel*ptr[k].v;
	  ptr[k].v_err = freq_to_vel*ptr[k].v_err;

	  /* convert decay parameters to spectral widths */

	  ptr[k].w_l = freq_to_vel*2*ptr[k].w_l;
	  ptr[k].w_l_err = freq_to_vel*2*ptr[k].w_l_err;

	  /* sigma is returned as sigma**2 so check the sign for validity		
	  if sigma**2 is negative take sqrt of the abs and transfer the sign */

	  ptr[k].w_s = (ptr[k].w_s >= 0) ? 
                      sqrt(ptr[k].w_s) : sqrt(-ptr[k].w_s);
	  if (ptr[k].w_s != 0) ptr[k].w_s_err = 0.5*ptr[k].w_s_err/
                                                fabs(ptr[k].w_s_err);
	   ptr[k].w_s = 3.33*freq_to_vel*ptr[k].w_s;
	   ptr[k].w_s_err = 3.33*freq_to_vel*ptr[k].w_s_err;
		

  	  /*  Now check the values of power, velocity and width
	      to see if this should be flagged as ground-scatter */
	        
	  if (ptr[k].gsct == 0) ptr[k].gsct=ground_scatter(&ptr[k]);                             
	}
	
	if ((iptr->prm.xcf != 0) && (ptr[k].qflg == 1)) {
	   xptr[k].qflg = fit_acf(iptr->xcfd[k], k+1,badlag[k],&badsmp,
						       lag_lim,&iptr->prm,noise_pwr,1,xomega,
								&xptr[k]);
	}

	if (xptr[k].qflg == 1) {		
      xptr[k].p_l = xptr[k].p_l*LN_TO_LOG - skylog;
	  xptr[k].p_s = xptr[k].p_s*LN_TO_LOG - skylog;
	  xptr[k].p_l_err = xptr[k].p_l_err*LN_TO_LOG;
	  xptr[k].p_s_err = xptr[k].p_s_err*LN_TO_LOG;

	  /* convert Doppler frequency to velocity */

	  xptr[k].v = iptr->prm.vdir*freq_to_vel*xptr[k].v;
	  xptr[k].v_err = freq_to_vel*xptr[k].v_err;

	  /* convert decay parameters to spectral widths */

	  xptr[k].w_l = freq_to_vel*2*xptr[k].w_l;
	  xptr[k].w_l_err = freq_to_vel*2*xptr[k].w_l_err;

	  /* sigma is returned as sigma**2 so check the sign for validity		
	  if sigma**2 is negative take sqrt of the abs and transfer the sign */

	  xptr[k].w_s = (xptr[k].w_s >= 0) ? 
                      sqrt(xptr[k].w_s) : sqrt(-xptr[k].w_s);
	  if (xptr[k].w_s != 0) xptr[k].w_s_err = 0.5*xptr[k].w_s_err/
                                                fabs(xptr[k].w_s_err);
	  xptr[k].w_s = 3.33*freq_to_vel*xptr[k].w_s;
	  xptr[k].w_s_err = 3.33*freq_to_vel*xptr[k].w_s_err;


	  /* calculate the elevation angle */
	
	  if (xptr[k].phi0 > PI)  xptr[k].phi0 = xptr[k].phi0 - 2*PI;
	  if (xptr[k].phi0 < -PI) xptr[k].phi0 = xptr[k].phi0 + 2*PI;
	  xptr[k].phi0 = xptr[k].phi0*iptr->prm.phidiff; 
        /* changes which array is first */
		
	  range = 0.15*(iptr->prm.lagfr + iptr->prm.smsep*(k-1));
      if (goose==0) {
	    elv[k].normal = elevation(&iptr->prm,range, xptr[k].phi0);
	    elv[k].low = 
          elevation(&iptr->prm,range, xptr[k].phi0+xptr[k].phi0_err);
	    elv[k].high = 
          elevation(&iptr->prm,range,xptr[k].phi0-xptr[k].phi0_err);
      } else {
  	    elv[k].normal = elev_goose(&iptr->prm,range, xptr[k].phi0);
	    elv[k].low = 
          elev_goose(&iptr->prm,range, xptr[k].phi0+xptr[k].phi0_err);
	    elv[k].high = 
          elev_goose(&iptr->prm,range, xptr[k].phi0-xptr[k].phi0_err);
      }
    }
    if( (ptr[k].qflg == 1) || (xptr[k].qflg== 1)) i++;   
  }
  return i;
}

