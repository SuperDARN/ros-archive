/* mlt1.c
   ====== */

/*
 $Log: mlt1.c,v $
 Revision 1.4  1998/06/17 18:59:41  barnes
 There appeared to be a problem at certain MLT times because the order
 of the spherical harmonic expansion for the coordinate conversion was too
 high.

 Revision 1.3  1998/06/16 23:48:17  barnes
 Fixed bug that caused the interpolated mlt to be wrong.

 Revision 1.2  1998/06/05 20:46:36  barnes
 Modifications to fix problems with return codes.

 Revision 1.1  1998/05/12 14:04:48  barnes
 Initial revision

 */

#include <stdio.h>
#include <math.h>
#include "math.h"
#include "convert_geo_coord.h"


static double sol_dec_old=0;
static double told=1e12;
static double mslon1=0;
static double mslon2=0;


double mlt1(int t0,double solar_dec,double mlon,double *mslon) {
 
  double ret_val;
  double mslat1,mslat2,slon1,slon2,height;
  int err;
 
  if ((fabs(solar_dec-sol_dec_old)>0.1) || (sol_dec_old==0)) told=1e12;
  if (fabs(mslon2-mslon1)>10) told=1e12;
    
   if ((t0>=told) && (t0<(told+600))) {
    *mslon=mslon1+(t0-told)*(mslon2-mslon1)/600.0;
   } else {
    told=t0;
    sol_dec_old=solar_dec;

    slon1 = (43200.0-t0)*15.0/3600.0;
    slon2 = (43200.0-t0-600)*15.0/3600.0;

    height = 450;

    err=convert_geo_coord(solar_dec,slon1,height,&mslat1,&mslon1,0,4);
    err=convert_geo_coord(solar_dec,slon2,height,&mslat2,&mslon2,0,4);
    *mslon=mslon1;
  }

  
  ret_val = (mlon - *mslon) /15.0 + 12.0;
  if (ret_val >=24) ret_val -=24;
  if (ret_val <0) ret_val+=24;  
  return ret_val;
} 


