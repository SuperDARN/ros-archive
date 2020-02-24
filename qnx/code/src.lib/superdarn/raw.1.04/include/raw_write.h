/* raw_write.h
   ===========
   Author: R.J.Barnes
*/

#ifndef _RAW_WRITE_H
#define _RAW_WRITE_H

#ifndef _RAWDATA_H
#include "rawdata.h"
#endif

int raw_write(int fp,char *name,struct rawdata *data,
		       int thresh,int recnum);
int raw_header_write(char *name,char *version,int thresh,char *text,int fp);

int raw_fwrite(FILE *fp,char *name,struct rawdata *data,
		       int thresh,int recnum);
int raw_header_fwrite(char *name,char *version,int thresh,char *text,FILE *fp);
 


#endif













