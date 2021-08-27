/* cnv_gsm_sm.c
   ============ */

/*
 $Log: cnv_gsm_sm.c,v $
 Revision 1.1  1999/08/05 20:30:55  barnes
 Initial revision

 */

#include <stdio.h>
#include "crdnt.h"

void cnv_gsm_sm(int year,int month,int day,int hour,int min,double sec,
                double *bgsm,double *bsm) {
   
   dipole(year,month,day,hour,min,sec);
   sun(year,month,day,hour,min,sec);
   
   gsmsm();
   cnvrt(bgsm,bsm);
} 











