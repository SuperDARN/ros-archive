/* copy_grid.c
   =========== */


/* 
 $Log: copy_grid.c,v $
 Revision 1.1  2000/06/15 18:18:09  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cnv_time.h"
#include "file_lib.h"
#include "griddata.h"

void copy_grid(struct griddata *a,struct griddata *b) {

  int i;
  a->st_time=b->st_time;
  a->ed_time=b->ed_time;

  a->nprm[0]=b->nprm[0];
  a->nprm[1]=b->nprm[1];
  a->npnt[0]=b->npnt[0];
  a->npnt[1]=b->npnt[1];

  for (i=0;i<2;i++) {
     if (a->name[i]==NULL) a->name[i]=malloc(strlen(b->name[i])+1);
     else a->name[i]=realloc(a->name[i],strlen(b->name[i])+1);
     if (a->type[i]==NULL) a->type[i]=malloc(strlen(b->type[i])+1);
     else a->type[i]=realloc(a->type[i],strlen(b->type[i])+1);
     if (a->unit[i]==NULL) a->unit[i]=malloc(strlen(b->unit[i])+1);
     else a->unit[i]=realloc(a->unit[i],strlen(b->unit[i])+1);
     strcpy(a->name[i],b->name[i]);
     strcpy(a->type[i],b->type[i]);
     strcpy(a->unit[i],b->unit[i]);
  }
   

  if (b->npnt[0]>0) {
    if (a->sdata==NULL) a->sdata=malloc(sizeof(struct svec)*b->npnt[0]);
    else a->sdata=realloc(a->sdata,sizeof(struct svec)*b->npnt[0]);
  } else if (a->sdata !=NULL) {
    free(a->sdata);
    a->sdata=NULL;
  }

  if (b->npnt[1]>0) {
  if (a->data==NULL) a->data=malloc(sizeof(struct gvec)*b->npnt[1]);
  else a->data=realloc(a->data,sizeof(struct gvec)*b->npnt[1]);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->npnt[0] !=0) memcpy(a->sdata,b->sdata,sizeof(struct svec)*b->npnt[0]);
  if (b->npnt[1] !=0) memcpy(a->data,b->data,sizeof(struct gvec)*b->npnt[1]);
}
