/* aff_grid.c
   ========== */


/* 
 $Log: add_grid.c,v $
 Revision 1.1  2000/06/15 18:18:09  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cnv_time.h"
#include "file_lib.h"
#include "griddata.h"

void add_grid(struct griddata *a,struct griddata *b,int recnum) {
  int i;
  int j;

  if (b==NULL)  { /* tidy up */
    if (recnum>1) {
      for (i=0;i<a->npnt[0];i++) {
        a->sdata[i].freq0=a->sdata[i].freq0/recnum;
        a->sdata[i].noise.mean=a->sdata[i].noise.mean/recnum;
        a->sdata[i].noise.sd=a->sdata[i].noise.sd/recnum;
      }
    }
    return;
  }
 
  if (b->npnt[0] !=0) {
    if (a->npnt[0]==0) {
      a->npnt[0]=b->npnt[0];
      if (a->sdata==NULL) a->sdata=malloc(sizeof(struct svec)*b->npnt[0]);
      else a->sdata=realloc(a->sdata,sizeof(struct svec)*b->npnt[0]);
      memcpy(a->sdata,b->sdata,sizeof(struct svec)*b->npnt[0]);
    } else {
      int st_id;
      /* merge the header information */
      for (i=0;i<b->npnt[0];i++) {
         st_id=b->sdata[i].st_id;
         for (j=0;(j<a->npnt[0]) && (a->sdata[j].st_id !=st_id);j++);
         if (j<a->npnt[0]) { /* merge record */
            a->sdata[j].npnt+=b->sdata[i].npnt;
            a->sdata[j].freq0+=b->sdata[i].freq0;
            if (b->sdata[i].gsct !=0) a->sdata[i].gsct=b->sdata[i].gsct !=0;
            a->sdata[j].noise.mean+=b->sdata[i].noise.mean;
            a->sdata[j].noise.sd+=b->sdata[i].noise.sd;
            if (b->sdata[i].vel.min<a->sdata[j].vel.min)
                a->sdata[j].vel.min=b->sdata[i].vel.min;
            if (b->sdata[i].pwr.min<a->sdata[j].pwr.min)
                a->sdata[j].pwr.min=b->sdata[i].pwr.min;
            if (b->sdata[i].wdt.min<a->sdata[j].wdt.min)
                a->sdata[j].wdt.min=b->sdata[i].wdt.min;
            if (b->sdata[i].vel.max>a->sdata[j].vel.max)
                a->sdata[j].vel.max=b->sdata[i].vel.max;
            if (b->sdata[i].pwr.max>a->sdata[j].pwr.max)
                a->sdata[j].pwr.max=b->sdata[i].pwr.max;
            if (b->sdata[i].wdt.max>a->sdata[j].wdt.max)
                a->sdata[j].wdt.max=b->sdata[i].wdt.max;
	 } else {
           a->sdata=realloc(a->sdata,sizeof(struct svec)*(j+1));
           memcpy(&a->sdata[j],&b->sdata[i],sizeof(struct svec));
           a->npnt[0]++;
	 }
      }
    }
  }

  i=a->npnt[1]; 
  j=a->npnt[1]+b->npnt[1];

  if (b->npnt[1] !=0) {  
    if (a->data==NULL) a->data=malloc(sizeof(struct gvec)*j);
    else a->data=realloc(a->data,sizeof(struct gvec)*j);
    memcpy(&a->data[i],b->data,
          sizeof(struct gvec)*b->npnt[1]);
    a->npnt[1]=j;
  }

}
