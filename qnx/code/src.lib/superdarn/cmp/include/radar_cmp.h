/* radar_cmp.h
   =========== */

#ifndef _RADAR_CMP_H
#define _RADAR_CMP_H

void radar_cmphdr(double pmax,double vmax,double wmax,
				struct radardata *scan,struct cmphdr *ptr);
 
int radar_cmp(int bmnum,int cnt,int store,double min_pwr,
	    double pmax,double vmax,double wmax,struct radardata *sptr,
	    struct cmpdata *ptr,int scan);
#endif





