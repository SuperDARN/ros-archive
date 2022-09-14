/* cfitread.h
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _CFITREAD_H
#define _CFITREAD_H

struct CFitfp {
  int fp;
  unsigned char *fbuf;
  int fptr;
  int fsze;
  int blen;
  double ctime;
};

int CFitRead(struct CFitfp *fptr,struct CFitdata *ptr);
struct CFitfp *CFitOpen(char *name);
void CFitClose(struct CFitfp *fptr);
int CFitSeek(struct CFitfp *fptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);


#endif






