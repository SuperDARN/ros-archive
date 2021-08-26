/* ground_scatter.c
   ================ */

/*
 $Log: ground_scatter.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>
#include "fit_blk.h"

/* the next two lines define the maximum values of
	velocity and spectral width that ground scatter can have */
	
#define GS_VMAX 30.
#define GS_WMAX 35.

int ground_scatter (struct range_data *ptr) {

  double vabs,wabs;
  vabs = ((fabs(ptr->v) - ptr->v_err) > 0.0) ? 
	      (fabs(ptr->v) - ptr->v_err) : 0.0;

  wabs = ((fabs(ptr->w_l) - ptr->w_l_err) > 0.0) ? 
	      (fabs(ptr->w_l) - ptr->w_l_err) : 0.0;

  if ((vabs < GS_VMAX) && (wabs < GS_WMAX)) return 1;
  else return 0;
}
