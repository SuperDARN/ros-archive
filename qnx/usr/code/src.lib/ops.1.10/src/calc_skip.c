/* calc_skip.c
   =========== */

/*
 $Log: calc_skip.c,v $
 Revision 1.4  2000/10/30 16:41:09  barnes
 Initialized skip to zero.

 Revision 1.3  2000/10/13 16:00:13  barnes
 Fixed return state of function.

 Revision 1.2  2000/10/13 15:56:35  barnes
 Fixed bug in calc_skip.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 */
#include <sys/types.h>
#include "limit.h"
#include "global.h"

int calc_skip(float bnd) {
  int skip=0;

  if (sec !=0) skip=((minut*60+sec) % (int) bnd + (intt+1)/2)/intt+2;
  if (skip>15) skip=0;
  return skip;
}

 
