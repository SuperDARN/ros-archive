/* smr_read.h
   ========== */

#ifndef _SMR_READ_H
#define _SMR_READ_H

#ifndef _FIT_DATA_H
#include "fitdata.h"
#endif

#ifndef _RADAR_SCAN_H
#include "radar_scan.h"
#endif


int smr_fread(FILE *fp,struct fitdata *fit,int bmnum);
int smr_seek(FILE *fp,int yr,int mo,int dy,int hr,int mt,int sc,double *atme);
int smr_fread_scan(FILE *fp,struct radardata *ptr,struct fitdata *fit,
                  int fbeam);
int smr_fread_scan_period(FILE *fp,struct radardata *ptr,
                  struct fitdata *fit,int fbeam,int tlen);




#endif






