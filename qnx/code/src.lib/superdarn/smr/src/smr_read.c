/* smr_read.c
   ========== */


/*
 $Log: smr_read.c,v $
 Revision 1.2  1999/10/14 14:13:11  barnes
 Fixed bug in reading attenuation.

 Revision 1.1  1999/03/16 20:09:54  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

#define MAX_RANGE 75

int smr_data_fread(FILE *fp,int ngood,struct fitdata *ptr) {

  int sptr;
   
  /* read in the fitacf data from the summary file*/

  int n,r,c;
  int index[MAX_RANGE]; /* array to store range indices*/
   
  sptr=ftell(fp);

  for (n=0;n<MAX_RANGE;n++) { /* reset the data array */
     ptr->rng[n].qflg=0;
     ptr->rng[n].gsct=0;
     ptr->rng[n].p_l=0;
     ptr->rng[n].v=0;
     ptr->rng[n].w_l=0;
  }

  for(n=0;n<ngood;n++) if (fscanf(fp,"%d",&index[n]) !=1) return -1;
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&ptr->rng[index[n]].p_l) !=1) return -1;  
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&ptr->rng[index[n]].v) !=1) return -1;
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&ptr->rng[index[n]].w_l) !=1) return -1;
  for(n=0;n<ngood;n++) {
    ptr->rng[index[n]].qflg=1;
    if ((ptr->rng[index[n]].v<20) && 
       (ptr->rng[index[n]].w_l<20)) ptr->rng[index[n]].gsct=1;
  }
  return ftell(fp)-sptr;
} 
  
int smr_fread(FILE *fp,struct fitdata *ptr,int beam) {
  int c;
  int sptr; 
  int blen;
   
  int year,mo,dy,hr,mt,sc;
  int rec_time; 
  int ngood;
  int atten,frang,rsep;
  int tfreq;
  int noise; 
  int bmnum;
  int nrang;
  int st_id;
  int status;

  /* skip beginning of line */
  sptr=ftell(fp);
  while((c=fgetc(fp)) != '=') if (c==EOF) return -1;
  if (fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d",
         &year,
         &rec_time,
         &st_id,
         &bmnum,
         &tfreq,
         &noise,
         &ngood,
         &atten,
         &nrang,
         &frang,
         &rsep) != 11) return -1;
    
  yrsec_time(rec_time,year,&mo,&dy,&hr,&mt,&sc);
  if (bmnum==beam) ptr->prms.SCAN=1;
  else ptr->prms.SCAN=0;
  ptr->prms.ST_ID=st_id;
  ptr->prms.ATTEN=atten/10;
  ptr->prms.FRANG=frang;
  ptr->prms.RSEP=rsep;     
  ptr->prms.BMNUM=bmnum;
  ptr->prms.TFREQ=tfreq;
  ptr->prms.NOISE=noise;
  ptr->prms.YEAR=year;
  ptr->prms.MONTH=mo;
  ptr->prms.DAY=dy;
  ptr->prms.HOUR=hr;
  ptr->prms.MINUT=mt;
  ptr->prms.SEC=sc;
  blen=ftell(fp)-sptr;
  status=smr_data_fread(fp,ngood,ptr);

  if (status ==-1) return -1;
  return blen+status;
}  








