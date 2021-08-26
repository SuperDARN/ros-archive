/* calc_skip.c
   =========== */

/*
 $Log: calc_skip.c,v $
 Revision 1.4  2000/10/24 16:35:46  barnes
 Fixed header problem.

 Revision 1.3  2000/10/13 16:01:20  barnes
 Fixed return state of function.

 Revision 1.2  2000/10/13 15:56:03  barnes
 Fixed bug in calc skip.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 */
#include <sys/types.h>
#include "limit.h"
#include "global_s.h"

int calc_skip(float bnd) {
  int skip;

  if (sec !=0) skip=((minut*60+sec) % (int) bnd + (intt+1)/2)/intt+2;
  if (skip>15) skip=0;
  return skip;
}

 
