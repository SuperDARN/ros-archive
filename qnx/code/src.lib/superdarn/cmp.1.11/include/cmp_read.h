/* cmp_read.h
   ==========
   Author: R.J.Barnes
*/

#ifndef _CMP_READ_H
#define _CMP_READ_H

#ifndef _CMPDATA_H
#include "cmpdata.h"
#endif

#ifndef _RADAR_SCAN_H
#include "radar_scan.h"
#endif

struct cmpfp {
  int fp;
  struct cmphdr hdr;
  unsigned char *fbuf;
  int fptr;
  int fsze;
  int blen;
  double ctime;
};

int cmp_read(struct cmpfp *fptr,struct cmpdata *ptr);
struct cmpfp *cmp_open(char *name);
void cmp_close(struct cmpfp *fptr);
int cmp_seek(struct cmpfp *fptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);

int cmp_read_scan(struct cmpfp *fp,struct radardata *ptr,struct cmpdata *cmp);

int cmp_read_scan_period(struct cmpfp *fp,struct radardata *ptr,
                         struct cmpdata *fit,int tlen);

int cmp_read_scan_channel(struct cmpfp *fp,struct radardata *ptr,
                          struct cmpdata *cmp,int channel);

int cmp_read_scan_period_channel(struct cmpfp *fp,struct radardata *ptr,
                         struct cmpdata *fit,int tlen,int channel);



#endif






