/* csddata.h
   =========
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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
 $Log: csddata.h,v $
 Revision 1.1  2004/07/12 22:06:03  barnes
 Initial revision

*/

#ifndef _CSDDATA_H
#define _CSDDATA_H


#define CSD_MAJOR_REVISION 1
#define CSD_MINOR_REVISION 0

#define read_bit(t,n) ((t[n/8] & (1<<(n%8))) !=0) 
#define set_bit(t,n)  t[n/8]=t[n/8] | (1<<(n%8))
 
struct CSDdata {
  struct {
    int32 major;
    int32 minor;
  } version;
  double pmax;
  double vmax;
  double wmax;
  int16 stid; 
  double time;
  int16 scan;
  int16 cp;
  int16 bmnum;
  int16 channel; 
  struct {
    int16 sc;
    int16 us;
  } intt;
  int16 frang;
  int16 rsep;
  int16 rxrise;
  int16 tfreq;
  int32 noise;
  int16 atten;
  int16 nave;
  int16 nrang;
  unsigned char stored;
  unsigned char store[MAX_RANGE*5];
};



#endif

 





