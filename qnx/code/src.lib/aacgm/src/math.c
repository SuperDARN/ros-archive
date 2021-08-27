/* math.c
   ====== */

/*
 $Log: math.c,v $
 Revision 1.3  1998/06/17 18:59:41  barnes
 Changed the code so that the order of the fit can be specified.

 Revision 1.2  1998/06/17 12:51:32  barnes
 Fixed problem with sign of latitude sometimes being reversed.

 Revision 1.1  1998/05/12 14:04:48  barnes
 Initial revision

 */

#include <stdlib.h>
#include <math.h>

double sgn(double a,double b) {
  double x=0;
  x=(double) (a>=0) ? a : -a;
  return (double) (b>=0) ? x: -x;
}

double mod(double x,double y) {
  double quotient;
  quotient = x / y;
  if (quotient >= 0) quotient = floor(quotient);
  else quotient = -floor(-quotient);
  return(x - y *quotient);
}





