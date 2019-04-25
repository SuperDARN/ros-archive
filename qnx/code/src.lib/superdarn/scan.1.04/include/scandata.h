/* scandata.h
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
  $Log: scandata.h,v $
  Revision 1.2  2006/08/16 20:58:59  code
  Added beam azimuth.

  Revision 1.1  2004/07/12 22:11:30  barnes
  Initial revision

*/


#ifndef _SCANDATA_H
#define _SCANDATA_H

struct RadarCell {
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

struct RadarBeam {
  int scan;
  int bm;
  float bmazm;
  double time;
  int cpid;
  struct {
    int sc;
    int us;
  } intt;
  int nave;
  int frang;
  int rsep;
  int rxrise;
  int freq;
  int noise;
  int atten;
  int channel;  
  int nrang;
  unsigned char sct[MAX_RANGE];
  struct RadarCell rng[MAX_RANGE];
};

struct RadarScan {
  int stid;
  struct {
    int major;
    int minor;
  } version;

  double st_time;
  double ed_time;
  int num;
  struct RadarBeam *bm;
};

int RadarScanReset(struct RadarScan *ptr);
int RadarScanResetBeam(struct RadarScan *ptr,int bmnum,int *bmptr);

#endif
