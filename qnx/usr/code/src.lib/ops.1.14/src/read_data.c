/* read_data.c
   =========== */

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
 $Log: read_data.c,v $
 Revision 1.2  2001/06/27 21:10:39  barnes
 Added license tag

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

 