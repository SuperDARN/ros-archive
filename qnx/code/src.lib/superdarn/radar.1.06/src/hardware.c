/* hardware.c
   ========== */

/*
 $Log: hardware.c,v $
 Revision 1.6  2000/11/20 14:29:20  barnes
 Fixed bug in setting max atten if missing from file.

 Revision 1.5  2000/11/01 17:29:01  barnes
 Added the maximum attenuation to the hardware file.

 Revision 1.4  1999/12/15 21:46:57  barnes
 Added code so that multiple hardware files can be read.

 Revision 1.3  1999/09/07 12:42:32  barnes
 Added test to make sure the site has a valid location.

 Revision 1.2  1998/06/11 17:04:03  barnes
 Fixed problem of location list not being incorrectly initialized.

 Revision 1.1  1998/06/11 15:52:05  barnes
 Initial revision

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "radar_name.h"
#include "hardware.h"


struct radar_hardware *load_hardware(FILE *fp) {
  int i,num=0;
  struct radar_hardware *ptr=NULL;
  char line[256];

  int st_id,year,yrsec;
  double geo_lat, geo_long, altitude; 
  double boresite, beam_sep; 
  double vdir,atten,tdiff,phidiff;
  double interfer_pos[3]; 
  double rec_rise;
  int status;
  int max_atten=3;


  ptr=malloc(sizeof(struct radar_hardware));
  if (ptr==NULL) return NULL;
  ptr->site=NULL;
  ptr->num=0;

  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]=='\n') || (line[i]==' '));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;
    status=sscanf(line+i, "%d%d%ld%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%d",
	          &st_id, &year, &yrsec, &geo_lat, &geo_long,
	          &altitude, &boresite, &beam_sep,
	          &vdir, &atten, &tdiff, &phidiff,
	          &interfer_pos[0], &interfer_pos[1], 
	          &interfer_pos[2],
		  &rec_rise,&max_atten);

    if (status<16) continue;
    

    if (ptr->site==NULL) ptr->site=malloc(sizeof(struct radar_site));
    else ptr->site=realloc(ptr->site,sizeof(struct radar_site)*(num+1));
    if (ptr->site==NULL) break;

    ptr->site[num].st_id=st_id;
    ptr->site[num].year=year;
    ptr->site[num].yrsec=yrsec;
    ptr->site[num].geo_lat=geo_lat;
    ptr->site[num].geo_long=geo_long;
    ptr->site[num].altitude=altitude;
    ptr->site[num].boresite=boresite;
    ptr->site[num].beam_sep=beam_sep;
    ptr->site[num].vdir=vdir;
    ptr->site[num].atten=atten;
    ptr->site[num].tdiff=tdiff;
    ptr->site[num].phidiff=phidiff;
    ptr->site[num].interfer_pos[0]=interfer_pos[0];
    ptr->site[num].interfer_pos[1]=interfer_pos[1];
    ptr->site[num].interfer_pos[2]=interfer_pos[2];
    ptr->site[num].rec_rise=rec_rise;
    if (status==17) ptr->site[num].max_atten=max_atten;
    else ptr->site[num].max_atten=3;
    num++;
    ptr->num=num;
  }
  return ptr;
}


int read_hardware(FILE *fp,struct radar_hardware *ptr) {
  int i,num=0;
  char line[256];

  int st_id,year,yrsec;
  double geo_lat, geo_long, altitude; 
  double boresite, beam_sep; 
  double vdir,atten,tdiff,phidiff;
  double interfer_pos[3]; 
  double rec_rise;
  int status;
  int max_atten=3;

  if (ptr==NULL) {
    ptr=malloc(sizeof(struct radar_hardware));
    if (ptr==NULL) return -1;
    ptr->site=NULL;
    ptr->num=0;
  } else num=ptr->num;

  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]=='\n') || (line[i]==' '));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

   status=sscanf(line+i, "%d%d%ld%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%d",
	          &st_id, &year, &yrsec, &geo_lat, &geo_long,
	          &altitude, &boresite, &beam_sep,
	          &vdir, &atten, &tdiff, &phidiff,
	          &interfer_pos[0], &interfer_pos[1], 
	          &interfer_pos[2],
		  &rec_rise,&max_atten);

    if (status<16) continue;
   
   
    if (ptr->site==NULL) ptr->site=malloc(sizeof(struct radar_site));
    else ptr->site=realloc(ptr->site,sizeof(struct radar_site)*(num+1));
    if (ptr->site==NULL) break;

    ptr->site[num].st_id=st_id;
    ptr->site[num].year=year;
    ptr->site[num].yrsec=yrsec;
    ptr->site[num].geo_lat=geo_lat;
    ptr->site[num].geo_long=geo_long;
    ptr->site[num].altitude=altitude;
    ptr->site[num].boresite=boresite;
    ptr->site[num].beam_sep=beam_sep;
    ptr->site[num].vdir=vdir;
    ptr->site[num].atten=atten;
    ptr->site[num].tdiff=tdiff;
    ptr->site[num].phidiff=phidiff;
    ptr->site[num].interfer_pos[0]=interfer_pos[0];
    ptr->site[num].interfer_pos[1]=interfer_pos[1];
    ptr->site[num].interfer_pos[2]=interfer_pos[2];
    ptr->site[num].rec_rise=rec_rise;
    if (status==17) ptr->site[num].max_atten=max_atten;
    else ptr->site[num].max_atten=3;

    num++;
    ptr->num=num;
  }
  return 0;
}



struct radar_hardware *load_hdw(char *path,struct radar_id *rid) {

  /* this version of the hardware routine reads in files for all sites */

  FILE *fp;
  int i;
  char fname[256];
  struct radar_hardware *ptr=NULL;
 

  ptr=malloc(sizeof(struct radar_hardware));
  if (ptr==NULL) return NULL;
  ptr->site=NULL;
  ptr->num=0;

  /* okay use the table of radar id codes to build the filenames */

  for (i=0;i<rid->num;i++) {
    sprintf(fname,"%s.%c",path,rid->radar[i].code);
    fp=fopen(fname,"r");
    if (fp !=NULL) {
      read_hardware(fp,ptr);
      fclose(fp);
    }
  }
  return ptr;
}


struct radar_site *get_site(struct radar_hardware *ptr,
				int yr,int mo,int dy,int hr,int mt,int sc,
				int st_id) {
  int i=0;
  int yrsec;
  yrsec=time_yrsec(yr,mo,dy,hr,mt,sc);
  for (i=0;(i<ptr->num) && (
    (ptr->site[i].st_id !=st_id) || (ptr->site[i].year < yr)
    || ((ptr->site[i].year==yr) && (ptr->site[i].yrsec < yrsec)));i++);
  if ((i<ptr->num) && (ptr->site[i].geo_lat==0) && 
       (ptr->site[i].geo_long==0)) return NULL;
  if (i<ptr->num) return &ptr->site[i];
  return NULL;
}









