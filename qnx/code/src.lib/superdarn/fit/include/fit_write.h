/* fit_write.h
   =========== */

#ifndef _FIT_WRITE_H
#define _FIT_WRITE_H
#ifndef _FITDATA_H
#include "fitdata.h"
#endif

int fit_header_write(int fitfp, char *text, char *name,char *version);
int fit_inx_close(int inxfp,struct fitdata *ptr,int irec);
int fit_inx_header_write(int inxfp,struct fitdata *ptr);
int fit_inx_write(int inxfp,int drec,int dnum,struct fitdata *ptr);
int fit_write(int fitfp,struct fitdata *ptr);

int fit_header_fwrite(FILE *fitfp, char *text, char *name,char *version);
int fit_inx_fclose(FILE *inxfp,struct fitdata *ptr,int irec);
int fit_inx_header_fwrite(FILE *inxfp,struct fitdata *ptr);
int fit_inx_fwrite(FILE *inxfp,int drec,int dnum,struct fitdata *ptr);
int fit_fwrite(FILE *fitfp,struct fitdata *ptr);


#endif
