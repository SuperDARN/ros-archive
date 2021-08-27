/* read_data.c
   =========== */

/*
 $Log: read_data.c,v $
 Revision 1.1  1999/03/31 19:58:35  barnes
 Initial revision

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "limit.h"
#include "raw_read.h"

#include "global.h"
 
int read_data(struct rawfp *fp) {
  struct rawdata *rdt;
  int i,j;
  int status=0;
  rdt=malloc(sizeof(struct rawdata));
  if (rdt==NULL) return -1;
 
  status=raw_read(fp,rdt); 
  if (status==0) {
    tfreq=rdt->PARMS.TFREQ;
    noise=rdt->PARMS.NOISE;
    atten=rdt->PARMS.ATTEN;
  
    for (i=0;i<nrang;i++) pwr0[i]=rdt->pwr0[i];
    for (i=0;i<nrang;i++) {
      for (j=0;j<mplgs;j++) {
        acfd[(i*mplgs*2)+(2*j)]=rdt->acfd[i][j][0];
        acfd[(i*mplgs*2)+(2*j)+1]=rdt->acfd[i][j][1];
        xcfd[(i*mplgs*2)+(2*j)]=rdt->xcfd[i][j][0];
        xcfd[(i*mplgs*2)+(2*j)+1]=rdt->xcfd[i][j][1];
      }
    }
  }

  free(rdt);
  return status;
}

 