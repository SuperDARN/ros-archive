/* merge_grid.c
   ============ */

/*
 $Log: merge_grid.c,v $
 Revision 1.6  2000/06/07 16:04:52  barnes
 Changed result station id to 255.

 Revision 1.5  1999/10/12 21:11:13  barnes
 Changed name of the ground scatter flag.

 Revision 1.4  1999/10/08 20:24:17  barnes
 Modifications for the extended file format.

 Revision 1.3  1999/10/07 20:26:02  barnes
 Added missing headers.

 Revision 1.2  1999/10/04 20:07:43  barnes
 New File Format Changes.

 Revision 1.1  1999/08/11 19:56:15  barnes
 Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "math_constant.h"
#include "file_lib.h"
#include "griddata.h"

void linreg(int num,struct gvec **data,double *vpar,double *vper) {

  int k;
  double sx=0,cx=0,ysx=0,ycx=0,cxsx=0;
  double den;
  for (k=0;k<num;k++) {
    sx=sx+sin(data[k]->azm*PI/180.0)*sin(data[k]->azm*PI/180.0);
    cx=cx+cos(data[k]->azm*PI/180.0)*cos(data[k]->azm*PI/180.0);
    ysx=ysx+data[k]->vel.median*sin(data[k]->azm*PI/180.0);
    ycx=ycx+data[k]->vel.median*cos(data[k]->azm*PI/180.0);
    cxsx=cxsx+sin(data[k]->azm*PI/180.0)*cos(data[k]->azm*PI/180.0);
  }   
    
  
  den=sx*cx-cxsx*cxsx;
  if (den !=0) {
    *vpar=(sx*ycx-cxsx*ysx)/den;
    *vper=(cx*ysx-cxsx*ycx)/den;
  } else {
    *vpar=0;
    *vper=0;
  }

}


void merge_grid(struct griddata *mptr,struct griddata *ptr) {

  int i,j;
  int k;
  int *index=NULL;
  int *count=NULL;

  struct gvec **data=NULL;
  int data_count=0;
  int num=0;

  double vpar,vper;

  ptr->st_time=mptr->st_time;
  ptr->ed_time=mptr->ed_time;

  ptr->nprm[0]=mptr->nprm[0];
  ptr->nprm[1]=mptr->nprm[1];

  ptr->name[0]=mptr->name[0];
  ptr->name[1]=mptr->name[1];
  ptr->type[0]=mptr->type[0];
  ptr->type[1]=mptr->type[1];
  ptr->unit[0]=mptr->unit[0];
  ptr->unit[1]=mptr->unit[1];
  
  ptr->npnt[1]=0;
  ptr->npnt[0]=1;
   
  if (ptr->sdata !=NULL) ptr->sdata=realloc(ptr->sdata,sizeof(struct svec));
  else ptr->sdata=malloc(sizeof(struct svec));
   
  ptr->sdata[0].st_id=255;
  ptr->sdata[0].freq0=0;
  ptr->sdata[0].major_revision=mptr->sdata[0].major_revision;
  ptr->sdata[0].minor_revision=mptr->sdata[0].minor_revision;
 
 /* need to verify that these are correct */

  ptr->sdata[0].prog_id=mptr->sdata[0].prog_id;
  ptr->sdata[0].noise.mean=mptr->sdata[0].noise.mean;
  ptr->sdata[0].noise.sd=mptr->sdata[0].noise.sd;

  ptr->sdata[0].gsct=mptr->sdata[0].gsct;

  ptr->sdata[0].vel.min=mptr->sdata[0].vel.min;
  ptr->sdata[0].vel.max=mptr->sdata[0].vel.max;
  ptr->sdata[0].pwr.min=mptr->sdata[0].pwr.min;
  ptr->sdata[0].pwr.max=mptr->sdata[0].pwr.max;
  ptr->sdata[0].wdt.min=mptr->sdata[0].wdt.min;
  ptr->sdata[0].wdt.max=mptr->sdata[0].wdt.max;
  
  if (ptr->data !=NULL) free(ptr->data);
  ptr->data=NULL;
  


  /* scan the grid file for unique cell numbers */

  for (i=0;i<mptr->npnt[1];i++) {
   
    if (num==0) {
      index=malloc(sizeof(int));
      count=malloc(sizeof(int));
      index[0]=mptr->data[i].index;
      count[0]=1;
      num++;
    }  else {
      /* search to determine if this is a new cell */
      for (k=0;(index[k] !=mptr->data[i].index) && (k<num);k++);
      if (k==num) {
        /* new cell */
        index=realloc(index,sizeof(int)*(num+1));
        count=realloc(count,sizeof(int)*(num+1));
        index[num]=mptr->data[i].index;
        count[num]=1;
        num++;
      } else count[k]++; /* existing cell */
    }
  }
 
  
  for (k=0;k<num;k++) {
    if (count[k]<2) continue;
    data_count=0;         
    /* locate all the data points in the array */
   
    for (i=0;i<mptr->npnt[1];i++) {
      
      if (mptr->data[i].index==index[k]) {
        /* got a point from a grid cell so add to the list */
        if (data==NULL) data=malloc(sizeof(struct gvec *));
        else data=realloc(data,sizeof(struct gvec *)*(data_count+1));
        data[data_count]=&mptr->data[i];
        data_count++;
      }
    }
    /* done the search and data is an array pointing to all the vectors */
   
    if (data==NULL) continue;
  
    /* add the new point to the merge grid */

    if (ptr->data==NULL) ptr->data=malloc(sizeof(struct gvec));
    else ptr->data=realloc(ptr->data,sizeof(struct gvec)*(ptr->npnt[1]+1));


    for (i=0;i<data_count;i++) data[i]->azm=90-data[i]->azm;
    linreg(data_count,data,&vpar,&vper);

    if (vper !=0) {         
      ptr->data[ptr->npnt[1]].azm=atan(vpar/vper)*180.0/PI;
      if (vper<0) ptr->data[ptr->npnt[1]].azm+=180.0;
    } else ptr->data[ptr->npnt[1]].azm=0;

    ptr->data[ptr->npnt[1]].mlon=data[0]->mlon;
    ptr->data[ptr->npnt[1]].mlat=data[0]->mlat;
    ptr->data[ptr->npnt[1]].vel.median=sqrt(vpar*vpar+vper*vper);
    ptr->data[ptr->npnt[1]].vel.sd=0;
    ptr->data[ptr->npnt[1]].pwr.median=0;
    ptr->data[ptr->npnt[1]].pwr.sd=0;
    ptr->data[ptr->npnt[1]].wdt.median=0;
    ptr->data[ptr->npnt[1]].wdt.sd=0;
    ptr->data[ptr->npnt[1]].st_id=255;
   
    ptr->data[ptr->npnt[1]].index=index[k];
    ptr->npnt[1]++;
    ptr->sdata[0].npnt++;
    free(data);
    data=NULL;
    data_count=0;


  }
  if (index !=NULL) free(index);
  if (count !=NULL) free(count);
 
}
     

















