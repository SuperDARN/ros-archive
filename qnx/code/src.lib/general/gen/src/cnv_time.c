/* cnv_time.c 
   ========== */

/*
 $Log: cnv_time.c,v $
 Revision 1.5  1998/06/24 18:22:39  barnes
 Fixed problem that occurs at midnight on the new year.

 Revision 1.4  1998/06/08 16:37:35  barnes
 Fixed problem of leap years giving the wrong time.

 Revision 1.3  1998/02/21 18:09:09  barnes
 No longer use POSIX routines for calculating epoch as they are local time
 dependent.
 Also changed the names of the functions to be consistent.

 Revision 1.2  1998/02/13 13:32:59  barnes
 Added routines for converting time to seconds from epoch.

 Revision 1.1  1998/02/10 19:58:40  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DAY_SEC (24*3600)
#define YEAR_SEC (365*24*3600)
#define LYEAR_SEC (366*24*3600)

void yrsec_time(int time,int yr, 
              int *mo,int *dy,
              int *hr,int *mn,int *sc) {
  int yd;
  int dt; 
  int n;
  int *jday;

  int nday[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  int lday[12] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

  if ((yr % 4)==0) jday=lday; 
  else jday=nday;  

  yd=time/(24L*60L*60L); /* get the day of the year */

  for (n=0;(n<12) && (yd>=jday[n]);n++);
 
  *mo=n; /* got the month */
  if (n>0) *dy=1+yd-jday[n-1]; /* got the date */
   else *dy=yd+1; 

  dt=time % (24L*60L*60L);
  *hr=dt/(60L*60L);
  *mn=(dt % (60L*60L))/60L;
  *sc=dt % 60L;     
}    

int time_yrsec(int yr, int mo, int day, int hr, int mn,int sec) {
  int t;
  int jday[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  int mday[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if ( (mo < 1) || (mo > 12) || (hr < 0) || (hr > 23) || 
       (day <1) || (mn < 0) || (mn > 59) || (sec < 0) || (sec >= 60) )
	   return -1;

  if (day > mday[mo-1])
	if (mo != 2) return -1;
	else if (day != (mday[1] +1) || ((yr % 4) != 0)) return -1;

  t = jday[mo-1] + day - 1;

  if ( (mo > 2) && ((yr % 4) == 0)) t = t+1;

  return (((t*24L + hr)*60L + mn)*60L)+sec;
}

double time_epoch(int yr,int mo,int dy,int hr,int mn,double sec) {
   double tme;
   int lpyear,ryear;
   int yrsec;
   if (yr<1970) return -1;
   yrsec=time_yrsec(yr,mo,dy,hr,mn,(int) sec);
   if (yrsec==-1) return -1;
   tme=yrsec+(sec-(int) sec);
    
   /* work out number of seconds from 1970 to start of year */

   lpyear=(yr-1969)/4;
   ryear=(yr-1970)-lpyear;

   tme+=(lpyear*LYEAR_SEC)+(ryear*YEAR_SEC); 
   return tme;
}

void epoch_time(double tme,int *yr,int *mo,int *dy,int *hr,int *mn,
	       double *sc) {
  int yrsec=0;
  int sec;
  int i=0;

  /* find the year */

  while ((yrsec<=tme) && (i<1e6)) {
    if ((i % 4)==2) yrsec+=LYEAR_SEC;
    else yrsec+=YEAR_SEC;
    i++;
  }
  if (((i-1) % 4)==2) tme-=yrsec-LYEAR_SEC;
  else tme-=yrsec-YEAR_SEC;
  *yr=i+1969;
  yrsec_time( (int) tme,*yr,mo,dy,hr,mn,&sec);

  *sc=sec+(tme-(int) tme);

}





