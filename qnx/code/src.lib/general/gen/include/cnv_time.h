/* cnv_time.h 
   ========== */

#ifndef _CNV_TIME_H
#define _CNV_TIME_H

void yrsec_time(int time,int yr, 
              int *mo,int *dy,
              int *hr,int *mn,int *sc);
int time_yrsec(int yr,int mo,int dy,int hr,int mn,int sc);

double time_epoch(int yr,int mo,int dy,int hr,int mn,double sec);
void   epoch_time(double tme,int *yr,int *mo,int *dy,
		  int *hr,int *mn,double *sec);

#endif
