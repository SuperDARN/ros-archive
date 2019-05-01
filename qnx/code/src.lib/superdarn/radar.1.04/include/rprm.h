/* rprm.h
   ====== 
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
 $Log: rprm.h,v $
 Revision 1.1  2004/07/12 22:24:49  barnes
 Initial revision

*/

#ifndef _RPRM_H
#define _RPRM_H

struct RadarParm {
  struct {
    char major;
    char minor;
  } revision;

  int16 cp;
  int16 stid;

  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    int16 sc;
    int16 us;
  } time;

  int16 txpow;
  int16 nave;
  int16 atten;
  int16 lagfr;
  int16 smsep;
  int16  ercod;
  
  struct {
    int16 agc;
    int16 lopwr;
  } stat;

  struct {
    float search;
    float mean;
  } noise;

  int16 channel;
  int16 bmnum;
  int16 scan;
 
  int16 rxrise;
  struct {
    int16 sc;
    int16 us;
  } intt;
  int16 txpl;
  int16 mpinc;
  int16 mppul;
  int16 mplgs;
  int16 nrang;
  int16 frang;
  int16 rsep;
  int16 xcf;
  int16 tfreq;
  int16 offset; 

  int32 mxpwr;
  int32 lvmax;

  int16 pulse[PULSE_SIZE];
  int16 lag[LAG_SIZE][2];  
  char combf[COMBF_SIZE];

};

#endif
