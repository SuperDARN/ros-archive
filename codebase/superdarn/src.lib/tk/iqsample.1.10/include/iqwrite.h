/* iqwrite.h
   ======== 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _IQWRITE_H
#define _IQWRITE_H


int IQWrite(int fid,struct RadarParm *prm,
	    struct IQData *iqdata,unsigned int *badtr,int16 *samples);

int IQFwrite(FILE *fp,struct RadarParm *prm,
	     struct IQData *iqdata,unsigned int *badtr,int16 *samples);

#endif

