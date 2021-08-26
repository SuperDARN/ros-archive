/* rconvert.h
   ==========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

/*
 $Log: rconvert.h,v $
 Revision 1.2  2004/06/22 17:11:14  barnes
 Changed function name of "Long" to "Int" for future 64-bit compatibility.

 Revision 1.1  2003/03/01 18:09:21  barnes
 Initial revision

*/

#ifndef _RCONVERT_H
#define _RCONVERT_H

int ConvertBitOrder();
void ConvertToInt(unsigned char *cnv,int32 *val);
void ConvertToShort(unsigned char *cnv,int16 *val);
void ConvertToDouble(unsigned char *cnv,double *val);
void ConvertToFloat(unsigned char *cnv,float *val);
void ConvertFromFloat(float val,unsigned char *cnv);
void ConvertFromDouble(double val,unsigned char *cnv);
void ConvertFromInt(int32 val,unsigned char *cnv);
void ConvertFromShort(int16 val,unsigned char *cnv);
void ConvertBlock(unsigned char *cnv,int *pattern);
int ConvertFreadInt(FILE *fp,int32 *val);
int ConvertFreadShort(FILE *fp,int16 *val);
int ConvertFreadDouble(FILE *fp,double *val);
int ConvertFreadFloat(FILE *fp,float *val);
int ConvertFwriteFloat(FILE *fp,float val);
int ConvertFwriteDouble(FILE *fp,double val);
int ConvertFwriteInt(FILE *fp,int32 val);
int ConvertFwriteShort(FILE *fp,int16 val);
int ConvertReadInt(int fp,int32 *val);
int ConvertReadShort(int fp,int16 *val);
int ConvertReadDouble(int fp,double *val);
int ConvertReadFloat(int fp,float *val);
int ConvertWriteFloat(int fp,float val);
int ConvertWriteDouble(int fp,double val);
int ConvertWriteInt(int fp,int32 val);
int ConvertWriteShort(int fp,int16 val);

#endif



