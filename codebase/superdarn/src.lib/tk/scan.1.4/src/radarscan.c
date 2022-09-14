/* radarscan.c
   ============ 
   Author R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
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


