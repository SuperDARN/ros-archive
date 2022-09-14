/* rprm.h
   ====== 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/



#ifndef _RPRM_H
#define _RPRM_H

struct RadarParm {
  struct {
    char major;
    char minor;
  } revision;

  struct {
    char code;
    char time[ORIGIN_TIME_SIZE];
    char command[ORIGIN_COMMAND_SIZE];
  } origin;

  int16 cp;
  int16 stid;

  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    int16 sc;
    int32 us;
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
  float bmazm;
  int16 scan;
 
  int16 rxrise;
  struct {
    int16 sc;
    int32 us;
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
