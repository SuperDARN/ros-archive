/* scandata.h
   ========== 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
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
