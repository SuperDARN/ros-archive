/* cfitdata.h
   ==========
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
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

 





