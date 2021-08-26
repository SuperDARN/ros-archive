/* raw_read.c 
   ========== */

/*
 $Log: raw_read.c,v $
 Revision 1.1  1999/03/16 16:32:48  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rawdata.h"
#include "raw_read.h"

int raw_read(struct rawfp *fp,struct rawdata *raw_data) { 
  if (fp==NULL) return -1;
  return (fp->rawread)(fp,raw_data);
}
 
int raw_read_data(struct rawfp *fp,struct rawdata *raw_data) {

  struct rawdata *raw_tmp;


 /* reads only the power and ACFs without altering the 
    radar parameters */

  int status,i;
  int nrang;
  if (fp==NULL) return -1;
  raw_tmp=malloc(sizeof(struct rawdata));
  if (raw_tmp==NULL) return -1;
  if ( (fp->rawread)(fp,raw_tmp) !=0) return -1;

  raw_data->PARMS.NOISE=raw_tmp->PARMS.NOISE;
  raw_data->PARMS.TFREQ=raw_tmp->PARMS.TFREQ;

  memcpy(raw_data->pwr0,raw_tmp->pwr0,
         sizeof(int32)*MAX_RANGE);

  memcpy(raw_data->acfd,raw_tmp->acfd,
		 sizeof(int32)*2*LAG_TAB_LEN*MAX_RANGE);
  memcpy(raw_data->xcfd,raw_tmp->xcfd,
		 sizeof(int32)*2*LAG_TAB_LEN*MAX_RANGE);
  free(raw_tmp); 
  return 0;
}




