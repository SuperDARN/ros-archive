/* check_ops.c
   =========== */


/*
 $Log: check_ops.c,v $
 Revision 1.4  1999/01/25 19:17:21  barnes
 Modifications to use the modified radardata structure.

 Revision 1.3  1998/06/07 22:33:04  barnes
 Modifications to use the radardata structure and to use epoch time.

 Revision 1.2  1998/05/12 14:02:04  barnes
 Removed frequency check.

 Revision 1.1  1998/02/03 23:11:58  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "radar_scan.h"

int check_ops(struct radardata *input) {
  int i,j,k;   

  for (i=0;i<3;i++) {
    for (j=0;j<16;j++) {
      if (input[1].bmcnt[j]==0) continue;
      for (k=0;k<input[i].bmcnt[j];k++) {
        if ( (input[i].frang[j][k] != input[1].frang[j][0]) ||
           (input[i].rsep[j][k] !=input[1].rsep[j][0])) {

	   /* (fabs(input[i].freq[j]-input[1].freq[j]) > 500) ) */
           return -1;
	}
      }
    }   
  }   
  return 0;
}
