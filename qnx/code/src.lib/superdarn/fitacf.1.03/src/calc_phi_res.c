/* calc_phi_res.c
   ============== */

/*
 $Log: calc_phi_res.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include "math_constant.h"

void calc_phi_res(struct complex *acf,int *badlag,
				  double *phi_res,int mplgs) {
  int i;
  for (i=0; i< mplgs; ++i) 
	phi_res[i] = ((badlag[i]) ? 0.0 : atan2(acf[i].y, acf[i].x));
}
