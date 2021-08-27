/* remove_noise.c
   ============== */

/*
 $Log: remove_noise.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>

#include "math_constant.h"
#include "badsmp.h"
#include "fit_blk.h"
#include "noise_stat.h"

void remove_noise(int mplgs,struct complex *acf,
				  struct complex *ncf) {
  int i;
  double pa, plim;

  plim = lag_power(&ncf[0]);
  pa = lag_power(&acf[0]);

  if( pa > plim ) for (i=0; i < mplgs; i++) {
      acf[i].x -= ncf[i].x;	
	  acf[i].y -= ncf[i].y;
    }
  else for (i=0; i < mplgs; i++) {
    acf[i].x = 0;
	acf[i].y = 0;
  }
  return;
}
