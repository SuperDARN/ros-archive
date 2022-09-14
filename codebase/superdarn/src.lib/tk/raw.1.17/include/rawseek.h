/* rawseek.h
   ========= 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/



#ifndef _RAWSEEK_H
#define _RAWSEEK_H

int RawSeek(int fid,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	      struct RawIndex *inx);
int RawFseek(FILE *fp,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	       struct RawIndex *inx);

#endif
