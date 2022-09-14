/* fitseek.h
   ========= 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/



#ifndef _FITSEEK_H
#define _FITSEEK_H

int FitSeek(int fid,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	      struct FitIndex *inx);
int FitFseek(FILE *fp,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	       struct FitIndex *inx);

#endif