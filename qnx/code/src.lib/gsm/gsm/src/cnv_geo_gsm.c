/* cnv_geo_gsm.c
   ============= */

/*
 $Log: cnv_geo_gsm.c,v $
 Revision 1.1  1999/08/05 20:30:55  barnes
 Initial revision

 */

#include <stdio.h>
#include "crdnt.h"


void cnv_geo_gsm(int year,int month,int day,int hour,int min,double sec,
                double *bgeo,double *bgsm) {
   
   dipole(year,month,day,hour,min,sec);
   sun(year,month,day,hour,min,sec);
   
   geogsm();
   cnvrt(bgeo,bgsm);
} 








