/* cnv_gsm_gse.c
   ============= */

/*
 $Log: cnv_gsm_gse.c,v $
 Revision 1.1  1999/08/05 20:30:55  barnes
 Initial revision

 */

#include <stdio.h>
#include "crdnt.h"

void cnv_gsm_gse(int year,int month,int day,int hour,int min,double sec,
                double *bgsm,double *bgse) {
   
   dipole(year,month,day,hour,min,sec);
   sun(year,month,day,hour,min,sec);
   
   gsmgse();
   cnvrt(bgsm,bgse);
} 













