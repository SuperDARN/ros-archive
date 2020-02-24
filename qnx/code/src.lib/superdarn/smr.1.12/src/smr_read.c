/* smr_read.c
   ==========
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
 $Log: smr_read.c,v $
 Revision 1.8  2002/02/06 17:43:09  barnes
 Simplified the channel filtering code.

 Revision 1.7  2002/02/06 16:19:19  barnes
 Added routines that can filter based on channel.

 Revision 1.6  2002/02/01 20:55:43  barnes
 Removed debug code.

 Revision 1.5  2002/01/31 16:13:57  barnes
 Added channel and program id to file format.

 Revision 1.4  2001/06/27 20:50:13  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.2  1999/10/14 14:13:11  barnes
 Fixed bug in reading attenuation.

 Revision 1.1  1999/03/16 20:09:54  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  int st_id=0;
  int cpid=0; 
  int channel;
  int status;
  char line[1024];
  char *tok;

  /* skip beginning of line */
  sptr=ftell(fp);
  while((c=fgetc(fp)) != '=') if (c==EOF) return -1;

  /* read the line and then decode */

  fgets(line,1024,fp);
  tok=strtok(line," ");
  if (tok==NULL) return -1;
  year=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  rec_time=atoi(tok);
   
  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  st_id=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  bmnum=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  tfreq=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  noise=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  ngood=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  atten=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  nrang=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  frang=atoi(tok);

  tok=strtok(NULL," ");
  if (tok==NULL) return -1;
  rsep=atoi(tok);


  tok=strtok(NULL," ");
  if (tok !=NULL) cpid=atoi(tok);
  if ((tok !=NULL) && ((tok=strtok(NULL," ")) !=NULL)) channel=atoi(tok);
   
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
  ptr->prms.CP=cpid;
  ptr->prms.CHANNEL=channel;
  blen=ftell(fp)-sptr;
  status=smr_data_fread(fp,ngood,ptr);

  if (status ==-1) return -1;
  return blen+status;
}  




