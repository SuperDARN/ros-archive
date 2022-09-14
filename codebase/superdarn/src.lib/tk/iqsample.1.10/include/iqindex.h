/* iqindex.h
   ========= 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/



#ifndef _IQINDEX_H
#define _IQINDEX_H


struct IQIndex {
  int num;
  double *tme;
  int *inx;
};

void IQIndexFree(struct IQIndex *inx);
struct IQIndex *IQIndexLoad(int fid);
struct IQIndex *IQIndexFload(FILE *fp);
 


#endif
