/* read_data_s.c
   ============= */

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

/*
 $Log: read_data_s.c,v $
 Revision 1.2  2001/06/27 21:10:46  barnes
 Added license tag

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

 
