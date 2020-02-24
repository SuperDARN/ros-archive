/* cmpdata.h
   =========
   Author: R.J.Barnes
*/

#ifndef _CMPDATA_H
#define _CMPDATA_H

#ifndef _INT_TYPES
#include "types.h"
#endif

#define MAJOR_REVISION 0
#define MINOR_REVISION 91


#define read_bit(t,n) ((t[n/8] & (1<<(n%8))) !=0) 
#define set_bit(t,n)  t[n/8]=t[n/8] | (1<<(n%8))
#define set_top(t,n) t[9]=t[9] | (n & 0x0f)<<4
#define read_top(t)  ((t[9] >> 4) &0x0f)
 
struct cmpdata {
  int32 magic;
  int16 tfreq;
  int32 noise;
  int16 frang,rsep,cp;
  int16 atten;
  double time;
  unsigned char channel;
  unsigned char bmnum;
  unsigned char scan;
  unsigned char stored;
  unsigned char store[75*5];
};

struct cmphdr {

  unsigned char major_rev;
  unsigned char minor_rev;
  double pmax;
  double vmax;
  double wmax;
  int16 st_id;
  int16 frang;
  int16 rsep;
  int16 atten;
  int16 cp;
 
};

#endif

 





