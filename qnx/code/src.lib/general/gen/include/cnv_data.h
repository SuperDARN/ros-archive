/* cnv_data.h
   ========== */

#ifndef _CNV_DATA_H
#define _CNV_DATA_H

#ifndef _INT_TYPES
#include "types.h"
#endif

int bit_order();
int cnv_long(unsigned char *,int32 *val);
int cnv_short(unsigned char *,int16 *val);
int cnv_double(unsigned char *,double *val);
int cnv_float(unsigned char *,float *val);

int long_cnv(int32 val,unsigned char *);
int short_cnv(int16 val,unsigned char *);
int double_cnv(double val,unsigned char *);
int float_cnv(float val,unsigned char *);

void cnv_block(unsigned char *buf,int *pattern);

int fread_long(FILE *fp,int32 *val);
int fread_short(FILE *fp,int16 *val);
int fread_double(FILE *fp,double *val);
int fread_float(FILE *fp,float *val);
 
int fwrite_long(FILE *fp,int32 val);
int fwrite_short(FILE *fp,int16 val);
int fwrite_double(FILE *fp,double val);
int fwrite_float(FILE *fp,float val);

int read_long(int fp,int32 *val);
int read_short(int fp,int16 *val);
int read_double(int fp,double *val);
int read_float(int fp,float *val);
 
int write_long(int fp,int32 val);
int write_short(int fp,int16 val);
int write_double(int fp,double val);
int write_float(int fp,float val);

#endif






