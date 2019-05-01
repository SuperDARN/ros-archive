/* elevation.c
   ===========
   Author: R.J.Barnes & K.Baker
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
 $Log: elevation.c,v $
 Revision 1.6  2004/05/04 22:54:21  barnes
 Changed math header name.

 Revision 1.5  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.4  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>
#include "fitblk.h"
#include "rmath.h"

double elevation(struct FitPrm *prm,double range,double phi0) {
 
  double k;          /* wave number; 1/m */
  double phi;        /* beam direction off boresight; rad */
  double c_phi;      /* cosine of phi                     */
  double dchi_cable; /* phase shift caused by cables; rad */
  double chi_max;    /* maximum phase shift possible; rad */
  double phi_temp;   /* actual phase angle + cable;   rad */
  double psi;        /* actual phase angle - cable;   rad */
  double theta;      /* angle of arrival for horizontal antennas; rad */

  static double antenna_separation= 0.0; /* m */
  static double elev_corr= 0.0;
  /* elevation angle correction, if antennas are at different heights; rad */
  static double phi_sign= 0;
  /* +1 if interferometer antenna is in front of main antenna, -1 otherwise*/

  /* calculate the values that don't change if this hasn't already been done. */

  if (antenna_separation == 0.0) {
    antenna_separation= sqrt(prm->interfer[1]*prm->interfer[1] + 
			                 prm->interfer[0]*prm->interfer[0] +
	                         prm->interfer[2]*prm->interfer[2]);
    elev_corr= prm->phidiff* asin( prm->interfer[2]/ antenna_separation);
    if (prm->interfer[1] > 0.0) /* interferometer in front of main antenna */
      phi_sign= 1.0;
    else {                           /* interferometer behind main antenna */
      phi_sign= -1.0;
      elev_corr= -elev_corr;
    }
  }
  
  phi= prm->bmsep*(prm->bmnum - 7.5)* PI/ 180.0;
  c_phi= cos( phi);
  k= 2 * PI * prm->tfreq * 1000.0/C;

  /* the phase difference phi0 is between -pi and +pi and gets positive,  */
  /* if the signal from the interferometer antenna arrives earlier at the */
  /* receiver than the signal from the main antenna. */
  /* If the cable to the interferometer is shorter than the one to */
  /* the main antenna, than the signal from the interferometer     */
  /* antenna arrives earlier. tdiff < 0  --> dchi_cable > 0        */

  dchi_cable= - 2* PI * prm->tfreq * 1000.0 * prm->tdiff * 1.0e-6;

  /* If the interferometer antenna is in front of the main antenna */
  /* then lower elevation angles correspond to earlier arrival     */
  /* and greater phase difference. */    
  /* If the interferometer antenna is behind of the main antenna   */
  /* then lower elevation angles correspond to later arrival       */
  /* and smaller phase difference */    

  chi_max= phi_sign* k* antenna_separation* c_phi + dchi_cable;

  /* change phi0 by multiples of twopi, until it is in the range   */
  /* (chi_max - twopi) to chi_max (interferometer in front)        */
  /* or chi_max to (chi_max + twopi) (interferometer in the back)  */

  phi_temp= phi0 + 2*PI* floor( (chi_max - phi0)/ (2*PI));
  if (phi_sign < 0.0) phi_temp= phi_temp + (2*PI);

  /* subtract the cable effect */
  psi= phi_temp - dchi_cable;
  theta= psi/ (k* antenna_separation);
  theta= (c_phi* c_phi - theta* theta);
  /* set elevation angle to 0 for out of range values */

  if ( (theta < 0.0) || (fabs( theta) > 1.0) ) theta= - elev_corr;
  else theta= asin( sqrt( theta));

  return 180.0* (theta + elev_corr)/ PI; /* in degree */
}

