/* iqread.h
   ======== 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _IQREAD_H
#define _IQREAD_H

int IQDecode(struct DataMap *ptr,struct RadarParm *prm,
	     struct IQData *iqdata,unsigned int **badtr,int16 **samples);

int IQRead(int fid,struct RadarParm *prm,
	   struct IQData *iqdata,unsigned int **badtr,int16 **samples);

int IQFread(FILE *fp,struct RadarParm *prm,
	    struct IQData *iqdata,unsigned int **badtr,int16 **samples);

#endif

