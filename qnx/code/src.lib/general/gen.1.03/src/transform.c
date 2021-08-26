/* transform.c
   =========== */

/*
 $Log: transform.c,v $
 Revision 1.1  1998/06/03 12:23:19  root
 Initial revision


 */

#include <stdlib.h>
#include <math.h>
#include "math_constant.h"

void ptoc(double lat,double lon,double rho,
		 double *x,double *y,double *z) {
    lon=lon+270.0;
    *x=(float) (rho*cos(lon*PI/180.0)
                     *cos(lat*PI/180.0));
    *y=(float) (rho*sin(lat*PI/180.0));
    *z=(float) (rho*sin(lon*PI/180.0)
                     *cos(lat*PI/180.0)); 
}

void ctop(double x,double y,double z,double *lat,double *lon,double *rho) {
  *rho=sqrt(x*x+y*y+z*z);
  *lat=asin(y/(*rho))*180.0/PI;
  *lon=atan2(x,z)*180.0/PI;
}
  



