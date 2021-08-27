/* read_data_s.c
   ============= */

/*
 $Log: read_data_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "limit.h"
#include "raw_read.h"

#include "global_s.h"
 
int read_data_s(int chn,struct rawfp *fp) {
  struct rawdata *rdt;
  int i,j;
  int status=0;
  rdt=malloc(sizeof(struct rawdata));
  if (rdt==NULL) return -1;
 
  status=raw_read(fp,rdt); 
  if (status==0) {
    if (chn==0) {
      tfreq_A=rdt->PARMS.TFREQ;
      noise_A=rdt->PARMS.NOISE;
      atten_A=rdt->PARMS.ATTEN;
  
      for (i=0;i<nrang_A;i++) pwr0_A[i]=rdt->pwr0[i];
      for (i=0;i<nrang_A;i++) {
        for (j=0;j<mplgs_A;j++) {
          acfd_A[(i*mplgs_A*2)+(2*j)]=rdt->acfd[i][j][0];
          acfd_A[(i*mplgs_A*2)+(2*j)+1]=rdt->acfd[i][j][1];
          xcfd_A[(i*mplgs_A*2)+(2*j)]=rdt->xcfd[i][j][0];
          xcfd_A[(i*mplgs_A*2)+(2*j)+1]=rdt->xcfd[i][j][1];
       }
      }
    } else {
      tfreq_B=rdt->PARMS.TFREQ;
      noise_B=rdt->PARMS.NOISE;
      atten_B=rdt->PARMS.ATTEN;
  
      for (i=0;i<nrang_B;i++) pwr0_B[i]=rdt->pwr0[i];
      for (i=0;i<nrang_B;i++) {
        for (j=0;j<mplgs_B;j++) {
          acfd_B[(i*mplgs_B*2)+(2*j)]=rdt->acfd[i][j][0];
          acfd_B[(i*mplgs_B*2)+(2*j)+1]=rdt->acfd[i][j][1];
          xcfd_B[(i*mplgs_B*2)+(2*j)]=rdt->xcfd[i][j][0];
          xcfd_B[(i*mplgs_B*2)+(2*j)+1]=rdt->xcfd[i][j][1];
       }
      }
    }

  }

  free(rdt);
  return status;
}

 
