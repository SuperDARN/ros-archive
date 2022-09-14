/* fitscan.h
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _FITSCAN_H
#define _FITSCAN_H

int FitReadRadarScan(int fid,int *state,
                 struct RadarScan *ptr,struct RadarParm *prm,
		 struct FitData *fit,int tlen,
                 int lock,int chn);

int FitFreadRadarScan(FILE *fp,int *state,
                 struct RadarScan *ptr,struct RadarParm *prm,
		 struct FitData *fit,int tlen,
                 int lock,int chn);



int FitToRadarScan(struct RadarScan *ptr,struct RadarParm *prm,
		   struct FitData *fit);


#endif


