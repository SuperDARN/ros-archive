/* csdscan.h
  ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _CSDSCAN_H
#define _CSDSCAN_H

int CSDReadRadarScan(struct CSDfp *fp,int *state,
                 struct RadarScan *ptr,struct CSDdata *csd,int tlen,
                 int lock,int chn);




#endif
