/* vari_calc_skip.c
   =========== */

/*
 $Log: vari_calc_skip.c,v $
 Initial revision; 20020423 DA

 */
#include <sys/types.h>
#include "limit.h"
#include "global.h"

int vari_calc_skip(float bnd, int intt_arr[], int nloop)

{
  int skip;
  int intt_sum;
  int iloop= 0;

  if (sec!= 0)
  {
    skip= (minut*60+sec) % (int) bnd;
    intt_sum= 0;
    do
    {
      intt_sum= intt_sum + intt_arr[ iloop];
      iloop= iloop + 1;
    } while ( (intt_sum < skip) && (iloop < nloop));
    iloop= iloop + 3; /* ??? */
    if (iloop > nloop) iloop= 0;
  }
  return iloop;
}

 
