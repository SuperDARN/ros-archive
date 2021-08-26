/* integrate_grid.c
   ================ */


/* 
 $Log: integrate_grid.c,v $
 Revision 1.1  2000/06/15 18:18:09  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cnv_time.h"
#include "file_lib.h"
#include "griddata.h"

void integrate_grid(struct griddata *a,struct griddata *b,double *err) {


  int i=0,j=0,k=0,l=0,m=0;
  int st_id,index;
  int pnt=0;
  double v_e,p_e,w_e;

  if (b==NULL) return;
  sort_grid(b);

  a->st_time=b->st_time;
  a->ed_time=b->ed_time;

  a->nprm[0]=b->nprm[0];
  a->nprm[1]=b->nprm[1];
  a->npnt[0]=b->npnt[0];
  a->npnt[1]=0;

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

  /* allocate enough space in the output grid to store
     every possible vector in the input */

  if (b->npnt[1]>0) { 
  if (a->data==NULL) a->data=malloc(sizeof(struct gvec)*b->npnt[1]);
  else a->data=realloc(a->data,sizeof(struct gvec)*b->npnt[1]);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->npnt[0] !=0) memcpy(a->sdata,b->sdata,sizeof(struct svec)*b->npnt[0]);
  if (b->npnt[1] ==0) return;

  /* clear the array */
  memset(a->data,0,sizeof(struct gvec)*b->npnt[1]);
    
  /* average together vectors from the same radar that lie
     in the same cell */

 
  i=0;
  while (i<b->npnt[1]) {
    st_id=b->data[i].st_id;
    j=i+1;
    while (j<b->npnt[1]) {
      if (b->data[j].st_id !=st_id) break;
      j++;
    }
    
    /* okay i and j mark the start and end of a station */    
    k=i;
    while (k<j) {
      index=b->data[k].index;
      l=k+1;
      while (l<j) {
        if (b->data[l].index !=index) break;      
        l++;
      }
      for (m=k;m<l;m++) { /* add vectors to the output */
        v_e=b->data[m].vel.sd;
        p_e=b->data[m].pwr.sd;
        w_e=b->data[m].wdt.sd;

        if (v_e<err[0]) v_e=err[0];
        if (p_e<err[1]) p_e=err[1];
        if (w_e<err[2]) w_e=err[2];

        a->data[pnt].azm+=b->data[m].azm;
        a->data[pnt].vel.median+=b->data[m].vel.median*1/(v_e*v_e);
        a->data[pnt].pwr.median+=b->data[m].pwr.median*1/(p_e*p_e);
        a->data[pnt].wdt.median+=b->data[m].wdt.median*1/(w_e*w_e);
        
        a->data[pnt].vel.sd+=1/(v_e*v_e);
        a->data[pnt].pwr.sd+=1/(p_e*p_e);
        a->data[pnt].wdt.sd+=1/(w_e*w_e);
      }

      a->data[pnt].azm=a->data[pnt].azm/(l-k);
      
      a->data[pnt].vel.median=a->data[pnt].vel.median/a->data[pnt].vel.sd;
      a->data[pnt].wdt.median=a->data[pnt].wdt.median/a->data[pnt].wdt.sd;
      a->data[pnt].pwr.median=a->data[pnt].pwr.median/a->data[pnt].pwr.sd;
      

      a->data[pnt].vel.sd=1/sqrt(a->data[pnt].vel.sd);
      a->data[pnt].wdt.sd=1/sqrt(a->data[pnt].wdt.sd);
      a->data[pnt].pwr.sd=1/sqrt(a->data[pnt].pwr.sd);
      a->data[pnt].mlat=b->data[k].mlat;
      a->data[pnt].mlon=b->data[k].mlon;

      a->data[pnt].st_id=st_id;
      a->data[pnt].index=index;
      pnt++; 
      
      k=l;
    }
    i=j;
  }
  a->npnt[1]=pnt;
  a->data=realloc(a->data,sizeof(struct gvec)*b->npnt[1]);


}

 
