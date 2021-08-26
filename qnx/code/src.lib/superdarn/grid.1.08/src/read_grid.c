/* read_grid.c
   =========== */


/*
 $Log: read_grid.c,v $
 Revision 1.13  2000/08/02 12:23:40  barnes
 Replaced the grid_seek function with a wrapper to the seek_file function.

 Revision 1.12  2000/03/17 19:19:51  barnes
 Fixed bug in releasing memory after a seek.

 Revision 1.11  1999/10/14 19:14:47  barnes
 Fixed missing log message.

 */

#include <stdio.h>
#include <stdlib.h>

#include "cnv_time.h"
#include "file_lib.h"
#include "griddata.h"


#define BLOCK1 "st_id nvec freq0 major_rev minor_rev prog_id noise_mean noise_sd gsct v_min v_max p_min p_max w_min w_max ve_min ve_max"

#define BLOCK2 "gmlong gmlat kvect st_id grid_index vlos vlos_sd"


#define BLOCK3 "gmlong gmlat kvect st_id grid_index vlos vlos_sd pwr pwr_sd wdt wdt_sd"

 
int decode_grid_one(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr) {

  struct griddata *gp;
  gp=(struct griddata *)ptr;

  if (strcmp(name,BLOCK1) !=0) return 0;

  if (pnt==0) {
    if (npnt>0) {
      if (gp->sdata !=NULL) gp->sdata=realloc(gp->sdata,
        sizeof(struct svec)*npnt);
      else gp->sdata=malloc( sizeof(struct svec)*npnt);
    } else {
      free(gp->sdata);
      gp->sdata=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->nprm[0]=nprm;
    gp->npnt[0]=npnt;
    
    if (gp->name[0] !=NULL) gp->name[0]=realloc(gp->name[0],strlen(name)+1);
    else  gp->name[0]=malloc(strlen(name)+1);
    strcpy(gp->name[0],name);
 
    if (gp->unit[0] !=NULL) gp->unit[0]=realloc(gp->unit[0],strlen(unit)+1);
    else  gp->unit[0]=malloc(strlen(unit)+1);
    strcpy(gp->unit[0],unit);

    if (gp->type[0] !=NULL) gp->type[0]=realloc(gp->type[0],strlen(type)+1);
    else  gp->type[0]=malloc(strlen(type)+1);
    strcpy(gp->type[0],type);
  }

  if (npnt>0) {
    gp->sdata[pnt].st_id=data[0].data.ival;
    gp->sdata[pnt].npnt=data[1].data.ival;
    gp->sdata[pnt].freq0=data[2].data.dval;
    gp->sdata[pnt].major_revision=data[3].data.ival;
    gp->sdata[pnt].minor_revision=data[4].data.ival;
    gp->sdata[pnt].prog_id=data[5].data.ival;
    gp->sdata[pnt].noise.mean=data[6].data.dval;
    gp->sdata[pnt].noise.sd=data[7].data.dval;
    gp->sdata[pnt].gsct=data[8].data.ival;
    gp->sdata[pnt].vel.min=data[9].data.dval;
    gp->sdata[pnt].vel.max=data[10].data.dval;
    gp->sdata[pnt].pwr.min=data[11].data.dval;
    gp->sdata[pnt].pwr.max=data[12].data.dval;
    gp->sdata[pnt].wdt.min=data[13].data.dval;
    gp->sdata[pnt].wdt.max=data[14].data.dval;
    gp->sdata[pnt].verr.min=data[15].data.dval;
    gp->sdata[pnt].verr.max=data[16].data.dval;
   
  }
  return 1;
}

int decode_grid_two(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr) {

  struct griddata *gp;
  gp=(struct griddata *)ptr;

  if (strcmp(name,BLOCK2) !=0) return 0;

  if (pnt==0) {
    
    if (npnt>0) {
      if (gp->data !=NULL) gp->data=realloc(gp->data,
        sizeof(struct gvec)*npnt);
      else gp->data=malloc( sizeof(struct gvec)*npnt);
    } else {
      free(gp->data);
      gp->data=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->nprm[1]=nprm;
    gp->npnt[1]=npnt;
    
    if (gp->name[1] !=NULL) gp->name[1]=realloc(gp->name[1],strlen(name)+1);
    else  gp->name[1]=malloc(strlen(name)+1);
    strcpy(gp->name[1],name);
 
    if (gp->unit[1] !=NULL) gp->unit[1]=realloc(gp->unit[1],strlen(unit)+1);
    else  gp->unit[1]=malloc(strlen(unit)+1);
    strcpy(gp->unit[1],unit);

    if (gp->type[1] !=NULL) gp->type[1]=realloc(gp->type[1],strlen(type)+1);
    else  gp->type[1]=malloc(strlen(type)+1);
    strcpy(gp->type[1],type);
  }
  if (npnt>0) {
    gp->data[pnt].mlat=data[1].data.dval;
    gp->data[pnt].mlon=data[0].data.dval;
    gp->data[pnt].azm=data[2].data.dval;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].index=data[4].data.ival;
    gp->data[pnt].vel.median=data[5].data.dval;
    gp->data[pnt].vel.sd=data[6].data.dval;
  }
  return 1;
}


int decode_grid_three(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr) {

  struct griddata *gp;
  gp=(struct griddata *)ptr;

  if (strcmp(name,BLOCK3) !=0) return 0;

  if (pnt==0) {
    
    if (npnt>0) {
      if (gp->data !=NULL) gp->data=realloc(gp->data,
        sizeof(struct gvec)*npnt);
      else gp->data=malloc( sizeof(struct gvec)*npnt);
    } else {
      free(gp->data);
      gp->data=NULL;
    }
    gp->st_time=st_time;
    gp->ed_time=ed_time;
    gp->nprm[1]=nprm;
    gp->npnt[1]=npnt;
    
    if (gp->name[1] !=NULL) gp->name[1]=realloc(gp->name[1],strlen(name)+1);
    else  gp->name[1]=malloc(strlen(name)+1);
    strcpy(gp->name[1],name);
 
    if (gp->unit[1] !=NULL) gp->unit[1]=realloc(gp->unit[1],strlen(unit)+1);
    else  gp->unit[1]=malloc(strlen(unit)+1);
    strcpy(gp->unit[1],unit);

    if (gp->type[1] !=NULL) gp->type[1]=realloc(gp->type[1],strlen(type)+1);
    else  gp->type[1]=malloc(strlen(type)+1);
    strcpy(gp->type[1],type);
  }
  if (npnt>0) {
    gp->data[pnt].mlat=data[1].data.dval;
    gp->data[pnt].mlon=data[0].data.dval;
    gp->data[pnt].azm=data[2].data.dval;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].index=data[4].data.ival;
    gp->data[pnt].vel.median=data[5].data.dval;
    gp->data[pnt].vel.sd=data[6].data.dval;
    gp->data[pnt].pwr.median=data[7].data.dval;
    gp->data[pnt].pwr.sd=data[8].data.dval;
    gp->data[pnt].wdt.median=data[9].data.dval;
    gp->data[pnt].wdt.sd=data[10].data.dval;
  }
  return 1;
}




int read_grid(FILE *fp,struct griddata *ptr) {

  decoder decode[3];
  void *dptr[3];

  decode[0]=decode_grid_one;
  decode[1]=decode_grid_two;
  decode[2]=decode_grid_three;

  dptr[0]=ptr;
  dptr[1]=ptr;
  dptr[2]=ptr;
  
  return read_file(fp,3,decode,dptr);
}

int grid_seek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,
              struct file_index *inx,double *aval) {

  return seek_file(fp,yr,mo,dy,hr,mt,sc,inx,aval);
}


















