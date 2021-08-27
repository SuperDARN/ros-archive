/* calc_skip.c
   =========== */

/*
 $Log: calc_skip.c,v $
 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 */
#include <sys/types.h>
#include "limit.h"
#include "global.h"

int calc_skip(float bnd) {
  if (sec !=0) return ((minut*60+sec) % (int) (bnd*60) + (intt+1)/2)/intt+2;
  return 0;
}

 