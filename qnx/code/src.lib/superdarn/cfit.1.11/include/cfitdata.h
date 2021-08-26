/* cfitdata.h
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
 $Log: cfitdata.h,v $
 Revision 1.4  2006/08/16 20:53:51  code
 Added beam azimuth as a parameter.

 Revision 1.3  2006/01/09 16:40:10  barnes
 Bumped minor version number.

 Revision 1.2  2006/01/06 16:39:17  barnes
 Changed microseconds to long integer.

 Revision 1.1  2004/08/01 15:48:57  barnes
 Initial revision

*/

#ifndef _CFITDATA_H
#define _CFITDATA_H


#define CFIT_MAJOR_REVISION 1
#define CFIT_MINOR_REVISION 1

struct CFitCell {
  int gsct;
  double p_0;
  double p_0_e;
  double v;
  double v_e;
  double w_l;
  double w_l_e;
  double p_l;
  double p_l_e;
};

struct CFitdata {
  struct {
    int major;
    int minor;
  } version;
  int16 stid; 
  double time;
  int16 scan;
  int16 cp;
  int16 bmnum;
  float bmazm;
  int16 channel;
  struct {
    int16 sc;
    int32 us;
  } intt;
  int16 frang;
  int16 rsep;
  int16 rxrise;
  int16 tfreq;
  int32 noise;
  int16 atten;
  int16 nave;
  int16 nrang;

  unsigned char num;
  unsigned char rng[MAX_RANGE];
  struct CFitCell data[MAX_RANGE];
};



#endif

 





