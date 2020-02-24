/* read_grid.c
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
 $Log: read_grid.c,v $
 Revision 1.19  2002/02/07 15:47:02  barnes
 Fixed misc bugs.

 Revision 1.18  2002/02/06 22:52:57  barnes
 Added channel to the header record.

 Revision 1.17  2002/02/06 21:30:14  barnes
 Modification to include the channel number in the file.

 Revision 1.16  2001/09/26 16:04:32  barnes
 Fixed bug caused by more rigorous typing of the data. This was
 a consequence of the digital version of the general read routine.

 Revision 1.15  2001/06/27 20:49:26  barnes
 Added license tag

 Revision 1.14  2001/01/29 18:11:53  barnes
 Added Author Name

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

#define BLOCK2 "st_id chn nvec freq0 major_rev minor_rev prog_id noise_mean noise_sd gsct v_min v_max p_min p_max w_min w_max ve_min ve_max"


#define BLOCK3 "gmlong gmlat kvect st_id grid_index vlos vlos_sd"


#define BLOCK4 "gmlong gmlat kvect st_id grid_index vlos vlos_sd pwr pwr_sd wdt wdt_sd"

 
#define BLOCK5 "gmlong gmlat kvect st_id chn grid_index vlos vlos_sd"


#define BLOCK6 "gmlong gmlat kvect st_id chn grid_index vlos vlos_sd pwr pwr_sd wdt wdt_sd"




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
    gp->sdata[pnt].freq0=data[2].data.fval;
    gp->sdata[pnt].major_revision=data[3].data.ival;
    gp->sdata[pnt].minor_revision=data[4].data.ival;
    gp->sdata[pnt].prog_id=data[5].data.ival;
    gp->sdata[pnt].noise.mean=data[6].data.fval;
    gp->sdata[pnt].noise.sd=data[7].data.fval;
    gp->sdata[pnt].gsct=data[8].data.ival;
    gp->sdata[pnt].vel.min=data[9].data.fval;
    gp->sdata[pnt].vel.max=data[10].data.fval;
    gp->sdata[pnt].pwr.min=data[11].data.fval;
    gp->sdata[pnt].pwr.max=data[12].data.fval;
    gp->sdata[pnt].wdt.min=data[13].data.fval;
    gp->sdata[pnt].wdt.max=data[14].data.fval;
    gp->sdata[pnt].verr.min=data[15].data.fval;
    gp->sdata[pnt].verr.max=data[16].data.fval;
   
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
    gp->sdata[pnt].chn=data[1].data.ival;
    gp->sdata[pnt].npnt=data[2].data.ival;
    gp->sdata[pnt].freq0=data[3].data.fval;
    gp->sdata[pnt].major_revision=data[4].data.ival;
    gp->sdata[pnt].minor_revision=data[5].data.ival;
    gp->sdata[pnt].prog_id=data[6].data.ival;
    gp->sdata[pnt].noise.mean=data[7].data.fval;
    gp->sdata[pnt].noise.sd=data[8].data.fval;
    gp->sdata[pnt].gsct=data[9].data.ival;
    gp->sdata[pnt].vel.min=data[10].data.fval;
    gp->sdata[pnt].vel.max=data[11].data.fval;
    gp->sdata[pnt].pwr.min=data[12].data.fval;
    gp->sdata[pnt].pwr.max=data[13].data.fval;
    gp->sdata[pnt].wdt.min=data[14].data.fval;
    gp->sdata[pnt].wdt.max=data[15].data.fval;
    gp->sdata[pnt].verr.min=data[16].data.fval;
    gp->sdata[pnt].verr.max=data[17].data.fval;
   
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
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].index=data[4].data.ival;
    gp->data[pnt].vel.median=data[5].data.fval;
    gp->data[pnt].vel.sd=data[6].data.fval;
  }
  return 1;
}


int decode_grid_four(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr) {

  struct griddata *gp;
  gp=(struct griddata *)ptr;


  if (strcmp(name,BLOCK4) !=0) return 0;
 
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
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].index=data[4].data.ival;
    gp->data[pnt].vel.median=data[5].data.fval;
    gp->data[pnt].vel.sd=data[6].data.fval;
    gp->data[pnt].pwr.median=data[7].data.fval;
    gp->data[pnt].pwr.sd=data[8].data.fval;
    gp->data[pnt].wdt.median=data[9].data.fval;
    gp->data[pnt].wdt.sd=data[10].data.fval;
  }
  return 1;
}




int decode_grid_five(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr) {

  struct griddata *gp;
  gp=(struct griddata *)ptr;

  if (strcmp(name,BLOCK5) !=0) return 0;
 
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
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].chn=data[4].data.ival;

    gp->data[pnt].index=data[5].data.ival;
    gp->data[pnt].vel.median=data[6].data.fval;
    gp->data[pnt].vel.sd=data[7].data.fval;
  }
  return 1;
}



int decode_grid_six(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr) {

  struct griddata *gp;
  gp=(struct griddata *)ptr;

  if (strcmp(name,BLOCK6) !=0) return 0;
 
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
    gp->data[pnt].mlat=data[1].data.fval;
    gp->data[pnt].mlon=data[0].data.fval;
    gp->data[pnt].azm=data[2].data.fval;
    gp->data[pnt].st_id=data[3].data.ival;
    gp->data[pnt].chn=data[4].data.ival;

    gp->data[pnt].index=data[5].data.ival;
    gp->data[pnt].vel.median=data[6].data.fval;
    gp->data[pnt].vel.sd=data[7].data.fval;
    gp->data[pnt].pwr.median=data[8].data.fval;
    gp->data[pnt].pwr.sd=data[9].data.fval;
    gp->data[pnt].wdt.median=data[10].data.fval;
    gp->data[pnt].wdt.sd=data[11].data.fval;
  }
  return 1;
}




int read_grid(FILE *fp,struct griddata *ptr) {

  decoder decode[6];
  void *dptr[6];

  decode[0]=decode_grid_one;
  decode[1]=decode_grid_two;
  decode[2]=decode_grid_three;

  decode[3]=decode_grid_four;
  decode[4]=decode_grid_five;
  decode[5]=decode_grid_six;


  dptr[0]=ptr;
  dptr[1]=ptr;
  dptr[2]=ptr;
  dptr[3]=ptr;
  dptr[4]=ptr;
  dptr[5]=ptr;

  return read_file(fp,6,decode,dptr);
}

int grid_seek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,
              struct file_index *inx,double *aval) {

  return seek_file(fp,yr,mo,dy,hr,mt,sc,inx,aval);
}


















