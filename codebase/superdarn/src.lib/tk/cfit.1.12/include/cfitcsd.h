/* cfitcsd.h
   ========
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/



#ifndef _CFITCSD_H
#define _CFITCSD_H

int CFitToCSD(int store,double min_pwr,
             double pmax,double vmax,double wmax,struct CFitdata *cfit,
             struct CSDdata *ptr);

#endif






