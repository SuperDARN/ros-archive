/* radar_scan.h
   ============ */

#ifndef _RADAR_SCAN_H
#define _RADAR_SCAN_H

#ifndef _RADAR_LIMIT_H
#include "limit.h"
#endif

#ifndef _RAWDATA_H
#include "rawdata.h"
#endif


#ifndef _FITDATA_H
#include "fitdata.h"
#endif

#ifndef _CMPDATA_H
#include "cmpdata.h"
#endif



#define SMR_SCAN 1
#define CMP_SCAN 2
#define FIT_SCAN 3
#define RT_SCAN 4

struct datapoint {
  int gsct;
  double v;
  double v_e;
  double w_l;
  double w_l_e;
  double p_l; 
  double p_l_e;
};

struct radardata {
  int scnt;
  int st_id;
  unsigned char ftype;
  int  bmcnt[16];
  int stored;
  unsigned char major_rev;
  unsigned char minor_rev;
  
  double stime;
  double etime;
  
  int *cpid[16];
  int *intt[16];
  int *frang[16];
  int *rsep[16];
  int *rxrise[16];
  int *freq[16];
  int *noise[16];
  int *atten[16];
  double *time[16];
  unsigned char *sct[16][MAX_RANGE]; 
  struct datapoint *rng[16][MAX_RANGE];
};


void  reset_scan(struct radardata *ptr);
void add_cmp_scan(struct radardata *ptr,struct cmpdata *cmp,
                  struct cmphdr *hdr);
void add_fit_scan(struct radardata *ptr,struct fitdata *fit);
void copy_scan(struct radardata *dst,
	      struct radardata *src);

#endif













