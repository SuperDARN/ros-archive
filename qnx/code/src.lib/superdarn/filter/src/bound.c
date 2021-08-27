/* bound.c
   ======= */

/*
 $Log: bound.c,v $
 Revision 1.5  1999/10/12 20:03:03  barnes
 Fixed bug in setting the order of the parameters.

 Revision 1.4  1999/10/12 19:41:24  barnes
 Reversed ordering of bit flags to be consistent with the filter.

 Revision 1.3  1999/10/12 18:43:03  barnes
 Added the ability to bound based on ground scatter flag.

 Revision 1.2  1999/10/08 22:13:00  barnes
 Modifications for the extended format.

 Revision 1.1  1999/08/12 13:23:07  barnes
 Initial revision

*/

#include <math.h>
#include <stdio.h>
#include "radar_scan.h"

void bound_type(struct radardata *input,int type) {
 
  int i,j,k;
  if (type !=2) type=!type;
  for (i=0;i<16;i++) {
    if (input->bmcnt[i]==0) continue;
    for (k=0;k<input->bmcnt[i];k++) {
      for (j=0;j<75;j++) {
        if (input->sct[i][j][k]==0) continue; 
        if (input->rng[i][j][k].gsct==type) input->sct[i][j][k]=0;
      }
    }
  }
}


void bound(int prm,struct radardata *input,double *min,double *max) {
  int i,j,k;
  
  for (i=0;i<16;i++) {
    if (input->bmcnt[i]==0) continue;
    for (k=0;k<input->bmcnt[i];k++) {
      for (j=0;j<75;j++) {
        if (input->sct[i][j][k]==0) continue;  

       if (prm & 0x01) {
          if (fabs(input->rng[i][j][k].v) < min[0]) input->sct[i][j][k]=0;
          if (fabs(input->rng[i][j][k].v) > max[0]) input->sct[i][j][k]=0;
        }

        if (prm & 0x02) {
          if (input->rng[i][j][k].p_l < min[1]) input->sct[i][j][k]=0;                   if (input->rng[i][j][k].p_l  > max[1]) input->sct[i][j][k]=0;
        }
        if (prm & 0x04) {
          if (input->rng[i][j][k].w_l < min[2]) input->sct[i][j][k]=0;
          if (input->rng[i][j][k].w_l > max[2]) input->sct[i][j][k]=0;
        }
        if (prm & 0x08) {
          if (input->rng[i][j][k].v_e < min[3]) input->sct[i][j][k]=0;
          if (input->rng[i][j][k].v_e > max[3]) input->sct[i][j][k]=0;
        }

      }
    }
  }
}











