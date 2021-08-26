/* grid.c 
   ====== */

/*
 $Log: gtable.c,v $
 Revision 1.12  2000/06/15 15:32:29  barnes
 Added missing RCS log message.

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hardware.h" 
#include "inv_mag.h"

#include "math_constant.h"
#include "fitdata.h"
#include "radar_scan.h"
#include "gtable.h"

double v_e_min=100;
double p_l_e_min=1;
double w_l_e_min=1;


int zero_gtable(int pnum,struct gpnt *ptr) {

  int i;
  
  for (i=0;i<pnum;i++) {
      ptr[i].azm=0;
      ptr[i].vel.median=0;
      ptr[i].vel.sd=0;
      ptr[i].pwr.median=0;
      ptr[i].pwr.sd=0;
      ptr[i].wdt.median=0;
      ptr[i].wdt.sd=0;
      ptr[i].cnt=0;
  }
  return 0;
}

int gtable_sort(const void *a,const void *b) {
  struct gpnt *ap;
  struct gpnt *bp;
  ap=(struct gpnt *) a;
  bp=(struct gpnt *) b;
 
  if (ap->ref<bp->ref) return -1;
  if (ap->ref>bp->ref) return 1;
  return 0;
}

struct gpnt *make_gtable(struct radar_site *pos,
                       int *frang,int *rsep,int *rxrise,
                       int  *ltab,int *pnum,
                       double *azmt) {

  struct gpnt *ptr=NULL;
  int nlon;
  double lat,lon,azm,lspc;
  int bm,rn,i=0,j,k=0;

  ptr=malloc(sizeof(struct gpnt)*(16*MAX_RANGE)); /* space for everything */

  for (bm=0;bm<16;bm++) {
  
    if ((frang[bm] !=-1) && (rsep[bm] !=0)) {
    
      for (rn=0;rn<MAX_RANGE;rn++) {
         inv_mag(bm,rn,1995,pos,
                 100*frang[bm]/15.0,100*rsep[bm]/15.0,rxrise[bm],
                 400.0,&lat,&lon,&azm);
         if (lon<0) lon+=360;
         if (lat<50) lat=50;

         /* fix lat and lon to grid spacing */
         
         ptr[i].mlat=(int) (lat)+0.5;
        
         lspc=((int) (360*cos(ptr[i].mlat*PI/180)+0.5))/(360.0);
  
         ptr[i].mlon=((int) (lon*lspc)+0.5)/lspc;

         
         ptr[i].ref=1000*( (int) lat-50)+( (int) (lon*lspc) ); 
                    /* grid reference */   
         ptr[i].cnt=bm*MAX_RANGE+rn;
         azmt[bm*MAX_RANGE+rn]=azm;
         i++;
      }
    }
  }

  /* sort the grid table into ascending order */

  qsort(ptr,i,sizeof(struct gpnt),gtable_sort);


  /* now sift through and remove redundancy */


  ltab[ptr[0].cnt]=0; 
  ptr[0].npix=1;
  for (j=1;j<i;j++) {
    if (ptr[k].ref==ptr[j].ref) { /* repeat */
      ltab[ptr[j].cnt]=k;
      ptr[k].npix++;
    } else {
      k++;
      memcpy(&ptr[k],&ptr[j],sizeof(struct gpnt));
      ltab[ptr[j].cnt]=k;
      ptr[k].npix=1;
    }    
  }

  *pnum=k+1;
  ptr=realloc(ptr,sizeof(struct gpnt)*(k+1));
  return ptr;  
}

int test_gtable_write(struct gtable *ptr,struct radardata *scan,int tlen) {

  double tm;
  int i;
  tm=(scan->stime+scan->etime)/2.0;
  if (ptr->st_time==-1) return 0;
  ptr->ed_time=tlen*( (int) (tm/tlen)); 

  if ((ptr->ed_time-ptr->st_time) >=tlen) {
    /* average the grid */
   
    ptr->npnt=0;
    ptr->freq=ptr->freq/ptr->nscan;
    for (i=0;i<ptr->pnum;i++) {
      if (ptr->pnt[i].cnt==0) continue;
      if (ptr->pnt[i].cnt<=(0.25*ptr->nscan*ptr->pnt[i].npix)) {
        ptr->pnt[i].cnt=0;
        continue;
      }
      ptr->npnt++;
      ptr->pnt[i].azm=ptr->pnt[i].azm/ptr->pnt[i].cnt;
      ptr->pnt[i].vel.median=ptr->pnt[i].vel.median/ptr->pnt[i].vel.sd;
      ptr->pnt[i].wdt.median=ptr->pnt[i].wdt.median/ptr->pnt[i].wdt.sd;
      ptr->pnt[i].pwr.median=ptr->pnt[i].pwr.median/ptr->pnt[i].pwr.sd;
      
     
      ptr->pnt[i].vel.sd=1/sqrt(ptr->pnt[i].vel.sd);
      ptr->pnt[i].wdt.sd=1/sqrt(ptr->pnt[i].wdt.sd);
      ptr->pnt[i].pwr.sd=1/sqrt(ptr->pnt[i].pwr.sd);

    }
    ptr->status=1;
    return 1;
  }
  return 0;
}



void map_to_gtable(struct gtable *ptr,struct radardata *scan,
                struct radar_site *pos,int tlen) {
  int gflg,i,k,j,l,fcnt=0;
  double freq=0,noise=0;
  double variance=0;
  double tm;

  tm=(scan->stime+scan->etime)/2.0;
   
  /* test to see if we need to remake the grid */
 
  for (gflg=0;gflg<16;gflg++) {
    if (scan->bmcnt[gflg] !=0) {  
      if ((ptr->frang[gflg] !=scan->frang[gflg][0]) ||
         (ptr->rsep[gflg] !=scan->rsep[gflg][0])) break;
    }
  }
 
  
  if ((ptr->st_time==-1) || (gflg<16)) {

    /* generate grid */

    ptr->st_id=scan->st_id;
    for (i=0;i<16;i++) {
      if ((scan->bmcnt[i] !=0) && (scan->frang[i][0] !=-1)) 
         ptr->frang[i]=scan->frang[i][0];
      else ptr->frang[i]=-1;
      if ((scan->bmcnt[i] !=0) && (scan->rsep[i][0] !=0)) 
         ptr->rsep[i]=scan->rsep[i][0];
      else ptr->rsep[i]=0;
    }
    ptr->nscan=0;
    if (ptr->pnt !=NULL) free (ptr->pnt);
    ptr->pnt=make_gtable(pos,ptr->frang,ptr->rsep,ptr->rxrise,
                         ptr->ltable,&ptr->pnum,
                         ptr->azm);
    ptr->st_time=tlen*( (int) (tm/tlen)); 
    zero_gtable(ptr->pnum,ptr->pnt);
  }
 

  /* if last grid was completed then reset the arrays */

  if (ptr->status==1) {
     ptr->status=0;
     ptr->freq=0;
     ptr->nscan=0;
     zero_gtable(ptr->pnum,ptr->pnt);
     ptr->st_time=ptr->ed_time;    
  }

  /* okay map the pixels */

  for (i=0;i<16;i++) {
    if (scan->bmcnt[i]==0) continue;
    for (k=0;k<scan->bmcnt[i];k++) {
      ptr->prog_id=scan->cpid[i][k];
      freq+=scan->freq[i][k];
      noise+=scan->noise[i][k];
   
      fcnt++;
      for (j=0;j<MAX_RANGE;j++) {
        if (scan->sct[i][j][k] !=0) {
          double v_e; 
          double p_l_e;
          double w_l_e;
           
          v_e=scan->rng[i][j][k].v_e;
          p_l_e=scan->rng[i][j][k].p_l_e;
          w_l_e=scan->rng[i][j][k].w_l_e;
   
          /* apply floor on uncertainties */
      

          if (v_e<v_e_min) v_e=v_e_min;
          if (p_l_e<p_l_e_min) p_l_e=p_l_e_min;
          if (w_l_e<w_l_e_min) w_l_e=w_l_e_min;
 
          l=ptr->ltable[i*MAX_RANGE+j];
       
          ptr->pnt[l].azm+=ptr->azm[i*MAX_RANGE+j];
          ptr->pnt[l].vel.median+=-scan->rng[i][j][k].v*1/(v_e*v_e);;
          ptr->pnt[l].pwr.median+=scan->rng[i][j][k].p_l*1/(p_l_e*p_l_e);
          ptr->pnt[l].wdt.median+=scan->rng[i][j][k].w_l*1/(w_l_e*w_l_e);

          ptr->pnt[l].vel.sd+=1/(v_e*v_e);
          ptr->pnt[l].pwr.sd+=1/(p_l_e*p_l_e);
          ptr->pnt[l].wdt.sd+=1/(w_l_e*w_l_e);

          ptr->pnt[l].cnt++;
	}
      }
    }
  }

  freq=freq/fcnt;
  noise=noise/fcnt;
  

  /* find variance of the noise */
  
  for (i=0;i<16;i++) {
    if (scan->bmcnt[i]==0) continue;
    for (k=0;k<scan->bmcnt[i];k++) 
      variance+=(scan->noise[i][k]-noise)*(scan->noise[i][k]-noise);
  }

  ptr->noise.mean=noise;
  ptr->noise.sd=sqrt(variance/fcnt);
  ptr->freq=freq;
  ptr->nscan++;
}

