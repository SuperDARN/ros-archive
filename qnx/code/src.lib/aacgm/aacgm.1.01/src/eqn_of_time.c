/* eqn_of_time.c
   ============= */

/*
 $Log: eqn_of_time.c,v $
 Revision 1.2  1999/12/15 18:57:37  barnes
 Modifications for year 2000.

 Revision 1.1  1998/05/12 14:04:48  barnes
 Initial revision

*/

#include <math.h>
#include "math.h"

double eqn_of_time(double mean_lon,int yr) {

  int index;
  double coef[12][7]={
     {-105.8,596.2,4.4,-12.7,-429.0,-2.1,19.3},
     {-105.9,596.2,4.4,-12.7,-429.0,-2.1,19.3},
     {-106.1,596.2,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.2,596.2,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.4,596.1,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.5,596.1,4.4,-12.7,-428.8,-2.1,19.3},
     {-106.6,596.1,4.4,-12.7,-428.8,-2.1,19.3},
     {-106.7,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-106.8,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-107.0,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-107.2,596.1,4.4,-12.7,-428.6,-2.1,19.3},
     {-107.3,596.1,4.4,-12.7,-428.6,-2.1,19.3},
  };

  if (yr<88) index = yr + 2000 - 1988;
  if ((yr>=88) && (yr<100)) index = yr - 88;
  else if ((yr>=100) && (yr<1900)) index=yr-88;
  else index=yr-1988;

   
   if (index<0) index = 1;
   if (index>12) index = 12;

    return coef[index-1][0]*sind(mean_lon)+
           coef[index-1][1]*sind(2.0*mean_lon)+
           coef[index-1][2]*sind(3.0*mean_lon)+
           coef[index-1][3]*sind(4.0*mean_lon)+
           coef[index-1][4]*cosd(mean_lon)+
           coef[index-1][5]*cosd(2.0*mean_lon)+
           coef[index-1][6]*cosd(3.0*mean_lon);
}






