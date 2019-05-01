/* ground_scatter.c
   ================
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
 $Log: ground_scatter.c,v $
 Revision 1.5  2004/01/08 22:20:39  barnes
 Kile Baker's Modifications for improved velocity error and ground scatter
 determination.

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


/* 
   This version of the ground-scatter detection algorithm is based
   on a statistical analysis of ground-scatter vs ionospheric scatter
   done by Stefan Sundeen, Gerard Blanchard and Kile Baker.

   The analysis inluded all data from 12 days (with large amounts of both
   ionospheric scatter and ground scatter) distributed over all seasons of
   the year.  Two radars were used in the analysis, Saskatoon and Kapuskasing.

   The result of the initial statistical analysis showed that scatter was 
   most likely ground scatter if

           abs(v) < GS_VMAX - (GS_VMAX/GS_WMAX)*abs(width)

   Let g(vabs,wabs) = (abs(v) - GS_VMAX - (GS_VMAX/GS_WMAX)*abs(width))

   Then, if was assume the errors in v and width are independent,
   we can estimate the error in the function g to be:

   g_err/g = sqrt((v_err/vabs)^2 + (width_err/width)^2)

   Then the final condition for flagging data as ground scatter is:

   if g - g_err <= 0 then flag as ground scatter.

*/

/* the next two lines define the maximum values of
	velocity and spectral width that ground scatter can have */
	
#define GS_VMAX 30.
#define GS_WMAX 90.

int ground_scatter (struct FitRange *ptr) {

  double vabs,wabs,g,g_err;
  double dv, dw;

  /*
    This is the old version.  No one seems to remember why
    it was done this way.

  vabs = ((fabs(ptr->v) - ptr->v_err) > 0.0) ? 
	      (fabs(ptr->v) - ptr->v_err) : 0.0;

  wabs = ((fabs(ptr->w_l) - ptr->w_l_err) > 0.0) ? 
	      (fabs(ptr->w_l) - ptr->w_l_err) : 0.0;

  if ((vabs < GS_VMAX) && (wabs < GS_WMAX)) return 1;

  */

  /* Here is the Sundeen-Blanchard-Baker version */

  vabs = fabs(ptr->v);
  wabs = fabs(ptr->w_l);
  g = (vabs - (GS_VMAX - (GS_VMAX/GS_WMAX)*wabs));

  /* don't allow the error to get too extreme.  If v_err/v or
     w_err/w is greater than 1 (100% error) just use 1.0 */

  dv = ((ptr->v_err/vabs) > 1.0) ? 1.0 :
    (ptr->v_err/vabs);

  dw = ((ptr->w_l_err/wabs) > 1.0) ? 1.0 :
    (ptr->w_l_err/wabs);

  /*  g_err = fabs(g)*sqrt(dv*dv + dw*dw);*/
  g_err = 0; /* turn off any correction for the error */

  if ( (g - g_err) < 0.0) return 1;
  else return 0;
}



