/* radarscan.c
   ============ 
   Author R.J.Barnes
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "scandata.h"

/*
 $Log: radarscan.c,v $
 Revision 1.5  2004/06/16 21:48:13  barnes
 Added missing header.

 Revision 1.4  2004/05/05 15:50:38  barnes
 Added missing header.

 Revision 1.3  2004/05/04 18:36:29  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2003/10/21 14:31:42  barnes
 Radar scan function.

*/


int RadarScanReset(struct RadarScan *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->bm !=NULL) free(ptr->bm);
  ptr->bm=0;
  ptr->num=0;
  return 0;
}



int RadarScanResetBeam(struct RadarScan *ptr,int bmnum,int *bmptr) {
  int c,n,num=0;
  struct RadarBeam *tmp;
  if (bmnum==0) return 0;
  if (bmptr==NULL) return -1;
  if (ptr==NULL) return -1;
  if (ptr->num==0) return -1;
  tmp=malloc(sizeof(struct RadarBeam)*ptr->num);
  if (tmp==NULL) return -1;

  for (n=0;n<ptr->num;n++) {
    for (c=0;c<bmnum;c++) 
      if (ptr->bm[n].bm==bmptr[c]) break;
    if (c !=bmnum) continue;
    memcpy(&tmp[num],&ptr->bm[n],sizeof(struct RadarBeam));
    num++;
  }
  free(ptr->bm);
  if (num>0) {
    ptr->bm=realloc(tmp,sizeof(struct RadarBeam)*num);
    if (ptr->bm==NULL) {
      free(tmp);
      ptr->num=0;
      return -1;
    }
  } else {
    free(tmp);
    ptr->bm=NULL;
  }
  ptr->num=num;
  return 0;
}


