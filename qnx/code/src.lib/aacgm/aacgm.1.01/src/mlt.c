/* mlt.c
   ===== */

/*
 $Log: mlt.c,v $
 Revision 1.1  1998/05/12 14:04:48  barnes
 Initial revision

 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "eqn_of_time.h"
#include "solar_loc.h"
#include "mlt1.h"

double mlt(int yr,int t0,double mlong, double *mslong) {

    double ret_val;
    double mean_lon;
    int mo,day,hr,minut,sc;
    
    double apparent_time;
    double et;
    double ut;
    double dec;

    if (yr > 1900) yr-=1900;
    solar_loc(yr, t0, &mean_lon,&dec);

    et = eqn_of_time(mean_lon, yr);

    ut=(float) (t0 % (24*3600));
    apparent_time = ut + et;
   
    ret_val = mlt1(apparent_time, dec, mlong, mslong);
    return ret_val;
}






