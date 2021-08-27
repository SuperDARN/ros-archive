/*   cnv_geo_gsm  converts geo to gsm
*/

#include <stdio.h>

extern void cnvrt();
extern void sun();
extern void dipole();
extern void gsmgeo();

void cnv_gsm_geo(syear,smonth,sday,shour,smin,ssec,bgsm,bgeo)
short int syear,smonth,sday,shour,smin,ssec;
double bgeo[];
double bgsm[];
{
   double posvdh[3];
   double dsec;
   int year;
   int month;
   int day;
   int hour;
   int min;

   year = (int) syear;
   month = (int) smonth;
   day = (int) sday;
   hour = (int) shour;
   min = (int) smin;
   dsec = (double)ssec;

   dipole(&year,&month,&day,&hour,&min,&dsec);
   sun(&year,&month,&day,&hour,&min,&dsec);

   gsmgeo();
   cnvrt(bgsm,bgeo);

}   /* cnv_vdh_gse() */








