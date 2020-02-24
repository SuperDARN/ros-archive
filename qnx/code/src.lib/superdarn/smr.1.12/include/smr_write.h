/* smr_write.h
   ===========
   Author: R.J.Barnes
*/

#ifndef _SMR_WRITE_H
#define _SMR_WRITE_H

#ifndef _FITDATA_H
#include "fitdata.h"
#endif

int smr_header_fwrite(FILE * fp,char *version,char *text,char *rname);
int smr_fwrite(FILE *fp,struct fitdata *ptr,int pwr);
 
#endif
