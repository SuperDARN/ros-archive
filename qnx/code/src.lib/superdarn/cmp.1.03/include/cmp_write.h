/* cmp_write.h
   =========== */

#ifndef _CMP_WRITE_H
#define _CMP_WRITE_H

#ifndef _CMPDATA_H
#include "cmpdata.h"
#endif

int cmp_header_write(int fp,struct cmphdr *ptr);
int cmp_write(int fp,struct cmpdata *ptr,int flg);

int cmp_header_fwrite(FILE *fp,struct cmphdr *ptr);
int cmp_fwrite(FILE *fp,struct cmpdata *ptr,int flg);


#endif
