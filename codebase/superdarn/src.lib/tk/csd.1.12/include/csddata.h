/* csddata.h
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _CSDDATA_H
#define _CSDDATA_H


#define CSD_MAJOR_REVISION 1
#define CSD_MINOR_REVISION 1

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
  unsigned char stored;
  unsigned char store[MAX_RANGE*5];
};



#endif

 





