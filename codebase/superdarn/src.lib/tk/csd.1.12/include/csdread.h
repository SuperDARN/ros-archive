/* csdread.h
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _CSDREAD_H
#define _CSDREAD_H

struct CSDfp {
  int fp;
  unsigned char *fbuf;
  int fptr;
  int fsze;
  int blen;
  double ctime;
};

int CSDRead(struct CSDfp *fptr,struct CSDdata *ptr);
struct CSDfp *CSDOpen(char *name);
void CSDClose(struct CSDfp *fptr);
int CSDSeek(struct CSDfp *fptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);


#endif






