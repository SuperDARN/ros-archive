/* csdwrite.h
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _CSDWRITE_H
#define _CSDWRITE_H

int CSDHeaderWrite(int fp,struct CSDdata *ptr);
int CSDWrite(int fp,struct CSDdata *ptr);
unsigned char *CSDMakeBuffer(struct CSDdata *ptr,int *sze);
int CSDHeaderFwrite(FILE *fp,struct CSDdata *ptr);
int CSDFwrite(FILE *fp,struct CSDdata *ptr);



#endif






