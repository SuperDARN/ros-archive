/* avg_grid.c
   ========== */

/*
 $Log: avg_grid.c,v $
 Revision 1.6  1999/10/12 21:11:13  barnes
 Changed name of the ground scatter flag.

 Revision 1.5  1999/10/08 20:24:17  barnes
 Modifications for the extended file format.

 Revision 1.4  1999/10/07 20:26:02  barnes
 Added missing headers.

 Revision 1.3  1999/10/04 20:07:43  barnes
 New File Format Changes.

 Revision 1.2  1999/09/08 15:54:42  barnes
 Fixed the averaging algorithm.

 Revision 1.1  1999/08/11 19:56:15  barnes
 Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "math_constant.h"
#include "file_lib.h"
#include "griddata.h"
     
int locate_cell(int npnt,struct gvec *ptr,int index) {
  int i;
  for (i=0;i<npnt && (ptr[i].index !=index);i++);
  return i;
}

void avg_grid(struct griddata *mptr,struct griddata *ptr,int flg) {

  int i,j;
  int k;
 

  ptr->st_time=mptr->st_time;
  ptr->ed_time=mptr->ed_time;
 
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
   
  ptr->sdata[0].st_id=0;
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


  if (ptr->data !=NULL) {
    free(ptr->data);
    ptr->data=NULL;
  }

  
  for (i=0;i<mptr->npnt[1];i++) {

      k=locate_cell(ptr->npnt[1],ptr->data,mptr->data[i].index);
      if (k==ptr->npnt[1]) {
        /* new cell */
        ptr->npnt[1]++;
        if (ptr->data==NULL) ptr->data=malloc(sizeof(struct gvec));
        else ptr->data=realloc(ptr->data,sizeof(struct gvec)*ptr->npnt[1]);

        ptr->data[k].azm=mptr->data[i].azm;
        ptr->data[k].vel.median=mptr->data[i].vel.median;
        ptr->data[k].vel.sd=mptr->data[i].vel.sd;
        ptr->data[k].pwr.median=mptr->data[i].pwr.median;
        ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
        ptr->data[k].wdt.median=mptr->data[i].wdt.median;
        ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
        ptr->data[k].mlon=mptr->data[i].mlon;
        ptr->data[k].mlat=mptr->data[i].mlat;
        ptr->data[k].index=mptr->data[i].index;      
     
        ptr->data[k].st_id=1;
        if (flg !=0) ptr->data[k].st_id=mptr->data[i].st_id;
      } else { 
        if (flg==0) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm+=mptr->data[i].azm;


                ptr->data[k].vel.median+=mptr->data[i].vel.median;
                ptr->data[k].vel.sd+=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median+=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd+=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median+=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd+=mptr->data[i].wdt.sd;

                ptr->data[k].st_id++;
              } else if ((flg==1) && 
                       (mptr->data[i].pwr.median>ptr->data[k].pwr.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
                ptr->data[k].st_id=mptr->data[i].st_id;
	      } else if ((flg==2) && 
                       (mptr->data[i].vel.median>ptr->data[k].vel.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==3) && 
                       (mptr->data[i].wdt.median>ptr->data[k].wdt.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==4) && 
                       (mptr->data[i].pwr.median<ptr->data[k].pwr.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
	      } else if ((flg==5) && 
			(mptr->data[i].vel.median<ptr->data[k].vel.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==6) && 
                       (mptr->data[i].wdt.median<ptr->data[k].wdt.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              }
	    }
        
  }
  if (flg==0) {
    for (i=0;i<ptr->npnt[1];i++) {
      ptr->data[i].azm=ptr->data[i].azm/ptr->data[i].st_id;
      ptr->data[i].vel.median=ptr->data[i].vel.median/ptr->data[i].st_id;
      ptr->data[i].vel.sd=ptr->data[i].vel.sd/ptr->data[i].st_id;

      ptr->data[i].pwr.median=ptr->data[i].pwr.median/ptr->data[i].st_id;
      ptr->data[i].pwr.sd=ptr->data[i].pwr.sd/ptr->data[i].st_id;
      ptr->data[i].wdt.median=ptr->data[i].wdt.median/ptr->data[i].st_id;
      ptr->data[i].wdt.sd=ptr->data[i].wdt.sd/ptr->data[i].st_id;
    }
  }
  ptr->sdata[0].npnt=ptr->npnt[1];
  return;
}

















