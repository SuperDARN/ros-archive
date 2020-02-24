/* aacgm_lib.h
   ===========
   Author: R.J.Barnes
*/

#ifndef _AACGM_LIB_H
#define _AACGM_LIB_H

int load_coef_aacgm(char *fname);
int init_aacgm(int year);
int cnv_aacgm(double in_lat,double in_lon,double height,
              double *out_lat,double *out_lon,double *r,
              int flag);
double mlt_aacgm(int yr,int yr_sec,double mlon);

#endif


