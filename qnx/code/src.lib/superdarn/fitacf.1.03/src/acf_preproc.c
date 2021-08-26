/* acf_preproc.c
   ============= */

/*
 $Log: acf_preproc.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <stdio.h>
#include <math.h>

#include "math_constant.h"
#include "acf_preproc.h"

int acf_preproc(struct complex *acf,
                struct complex *orig_acf,
                double *noise_lev,
                int range,int *badlag,
                int mplgs) {
  int k;
  
  for (k=0; k < mplgs; k++) {
    acf[k].x = orig_acf[k].x;
    acf[k].y = orig_acf[k].y;
  }
  return ACF_UNMODIFIED; 
}


