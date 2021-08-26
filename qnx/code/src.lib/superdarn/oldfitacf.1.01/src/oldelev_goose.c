/* oldelev_goose.c
   ===============
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
 $Log: oldelev_goose.c,v $
 Revision 1.1  2004/07/23 23:47:18  barnes
 Initial revision

*/

/*
 Old RCS Log:

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>

#include "fitblk.h"
#include "rmath.h"

double oldelev_goose(struct FitPrm *prm, double range, double phi0) {
  int n;
  double k, psi, dchi_cable, temp, sn2_alpha_min, cs_alpha_min;
  double sin_psi2, sin_psi_xi, sin_psi2_xi2, sn2_alpha_old;
  double dchi_sep_max, dchi_max, dchi, dchi_old, cs_alpha, sn2_eps=0, sn_eps;
  double elev;

  static double xi=0.0, sep= 0.0, elev_corr=0.0, cos_xi=0.0, 
                 sin_xi=0.0, cos_xi2=0.0, sin_xi2;

  /* calculate the values that don't change if this hasn't already been done.

  The coordinate system is: +x is in front of the main array,
  +y is to the left of the center, and +z is upward.   

  The angle xi is in the x-y plane.  The angle is measured counter-clockwise
  from the x axis.  If the y offset and x offset are both positive, the
  angle is positive.  

  Sep is the distance between the centers of the two arrays

  If the z offset is not zero, then the elevation angle has to be corrected.
  An apparent elevation angle of 0 would actually being pointing slightly
  upward (downward) if the z offset is positive (negative).

  */

  if (sep == 0.0) {
    xi = atan2(prm->interfer[0],(prm->interfer[1]));
    sep = sqrt(prm->interfer[0]*prm->interfer[0] + 
			prm->interfer[1]*prm->interfer[1] +
	       prm->interfer[2]*prm->interfer[2]);

    elev_corr = atan2(prm->interfer[2], 
			sqrt(prm->interfer[0]*prm->interfer[0] +
			prm->interfer[1]*prm->interfer[1]));
    cos_xi = cos(xi);
    sin_xi = sin(xi);
  
    cos_xi2 = cos(xi)*cos(xi);
    sin_xi2 = sin(xi)*sin(xi);
  }

  /* compute phasing matrix cone angle */
  psi = prm->bmsep*(prm->bmnum-7.5)*PI/180.;	

  /* compute wavenumber */
  k = 2 * PI * prm->tfreq * 1000.0/C;	

  dchi_cable = - 2* PI * prm->tfreq * 1000.0 * prm->tdiff * 1.0e-6;

  /*	compute the minimum cone angle (alpha) */


  temp = sin(psi) + sin_xi;
  sin_psi_xi = sin(psi)*sin_xi;

  sn2_alpha_min = (temp*temp)/(1.0 + sin_psi_xi);
  cs_alpha_min = sqrt(1.0 - sn2_alpha_min);

  /* now iterate sn2_alpha_min, cs_alpha_min to improve value of alpha_min */

  sin_psi2 = sin(psi)*sin(psi);
  sin_psi2_xi2 = sin_psi2 + sin_xi2;

  sn2_alpha_old = sn2_alpha_min;

  while( fabs((sn2_alpha_min = sin_psi2_xi2 + 2.0*cs_alpha_min*sin_psi_xi)
				- sn2_alpha_old) > 0.005*sn2_alpha_old) {
	cs_alpha_min = sqrt(1.0 - sn2_alpha_min);
	sn2_alpha_old = sn2_alpha_min;
  }
  cs_alpha_min = sqrt(1.0 - sn2_alpha_min);

  /* we've now got the sin & cos of alpha_min */
  /*	compute the total phase difference */

  dchi_sep_max = k * sep / cos_xi * cs_alpha_min;
  dchi_max = dchi_cable + dchi_sep_max;
  n = 0.5 - dchi_max/(2*PI);

  dchi = phi0 - n*(2*PI);	/* this should be the true phase difference */

  if (dchi > dchi_max) dchi = dchi - (2*PI);
  if (dchi < (dchi_max - (2*PI))) dchi = dchi + (2*PI);

  /* compute the cone angle (alpha) */

  dchi_old = 0.0;
  while (fabs(dchi_old - dchi) > PI) {
	cs_alpha = (dchi - dchi_cable)/(k*sep)*cos_xi;
	sn2_eps = 1.0 - (cs_alpha*cs_alpha)/(cos_xi2) - (sin_psi2/cos_xi2)
		 - 2.0*cs_alpha*sin_psi_xi/cos_xi2;
	dchi_old = dchi;

	if ((fabs(sn2_eps) > 1.0) || (sn2_eps < 0.0)) {
      dchi = dchi - (2*PI);
	  fprintf(stderr,"changing dchi by -2pi. %f -> %f\n",dchi_old,dchi);
	}
  }

  sn_eps = sqrt(sn2_eps);
  elev = asin(sn_eps);

  /* The calculated elevation angle is actually with respect to the plane
   that includes the two antenna arrays.  This has to be corrected for the
   difference in elevation between the front array and the back array.
  */

  elev = elev + elev_corr;
  return 180.0*elev/PI;

}

