/* transform.h
   ===========
   Author: R.J.Barnes
*/

#ifndef _TRANSFORM_H
#define _TRANSFORM_H

int ptoc(double lat,double lon,double rho,
		 double *x,double *y,double *z);
int ctop(double x,double y,double z,double *lat,double *lon,double *rho);
 
#endif

