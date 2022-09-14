/* fitcsd.h
   ========
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/



#ifndef _FITCSD_H
#define _FITCSD_H

int FitToCSD(int store,double min_pwr,
             double pmax,double vmax,double wmax,struct RadarParm *prm,
             struct FitData *fit,
             struct CSDdata *ptr);

#endif






