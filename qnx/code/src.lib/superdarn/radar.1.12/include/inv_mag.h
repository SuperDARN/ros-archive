/* inv_mag.h
   =========
   Author: R.J.Barnes
*/


void rngbm_azmelv(int bm,int rn,int year,struct radar_site *hdw,double lagfr,
                  double smsep,double rx,double height,
		  double *azm,double *elv);

void inv_mag(int bm,int rn,int year,struct radar_site *hdw,double lagfr,
             double smsep,double rx,double height,
             double *mlat,double *mlon,double *azm);
