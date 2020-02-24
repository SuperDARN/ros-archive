/* check_ops.c
   ===========
   Author: R.J.Barnes
*/

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
 $Log: check_ops.c,v $
 Revision 1.6  2001/06/27 20:47:43  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:52  barnes
 Added Author Name

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
