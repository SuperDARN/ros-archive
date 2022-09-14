/* cfitscan.h
  ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _CFITSCAN_H
#define _CFITSCAN_H

int CFitReadRadarScan(struct CFitfp *fp,int *state,
                 struct RadarScan *ptr,struct CFitdata *fit,int tlen,
                 int lock,int chn);

int CFitToRadarScan(struct RadarScan *ptr,struct CFitdata *fit);


#endif

