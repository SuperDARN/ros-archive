/* fit_read.h
   ==========
   Author: R.J.Barnes
*/

#ifndef _FIT_READ_H
#define _FIT_READ_H

#ifndef _FITDATA_H
#include "fitdata.h"
#endif

#ifndef _RADAR_SCAN_H
#include "radar_scan.h"
#endif

#include <sys/stat.h>
#include <sys/types.h>

struct fitfp {
  int fitfp;
  int inxfp;
  int fit_recl;
  int inx_recl;
  struct stat fstat;
  struct stat istat;
  int blen;
  int fptr;
  int iptr;
  unsigned char *fbuf;
  unsigned char *ibuf;
  int inx_srec;
  int inx_erec;
  double ctime;
  double stime;
  double etime;
  int time;
  char header[80];
  char date[32];
  char extra[256];
  char major_rev; 
  char minor_rev;
  int (*fitread)(struct fitfp *ptr,struct fitdata *fit_data);
};

int fit_read(struct fitfp *ptr,struct fitdata *fit_data);


int fit_seek(struct fitfp *ptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);
void fit_close(struct fitfp *ptr);
struct fitfp *fit_open(char *fitfile,char *inxfile);




int fit_read_scan(struct fitfp *fp,struct radardata *ptr,struct fitdata *fit);
int fit_read_scan_period(struct fitfp *fp,struct radardata *ptr,
			 struct fitdata *fit,int tlen);

int fit_read_scan_channel(struct fitfp *fp,struct radardata *ptr,
                          struct fitdata *fit,int channel);
int fit_read_scan_period_channel(struct fitfp *fp,struct radardata *ptr,
			 struct fitdata *fit,int tlen,int channel);

struct fitfp *fit_fileno(int fp);
struct fitfp *fit_stream(FILE *fp);

#endif


