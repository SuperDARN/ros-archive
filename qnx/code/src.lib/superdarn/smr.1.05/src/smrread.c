/* smrread.c
   =========
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

/*
 $Log: smrread.c,v $
 Revision 1.4  2004/05/05 15:52:13  barnes
 Added missing header.

 Revision 1.3  2004/05/04 18:35:57  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:33:46  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/


int SmrFreadData(FILE *fp,int ngood,struct RadarParm *prm,
                 struct FitData *fit) {

  int sptr;
   
  /* read in the fitacf data from the summary file*/

  int n;
  int index[MAX_RANGE]; /* array to store range indices*/
   
  sptr=ftell(fp);

  for (n=0;n<MAX_RANGE;n++) { /* reset the data array */
     fit->rng[n].qflg=0;
     fit->rng[n].gsct=0;
     fit->rng[n].p_l=0;
     fit->rng[n].v=0;
     fit->rng[n].w_l=0;
  }

  for(n=0;n<ngood;n++) if (fscanf(fp,"%d",&index[n]) !=1) return -1;
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&fit->rng[index[n]].p_l) !=1) return -1;  
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&fit->rng[index[n]].v) !=1) return -1;
  for(n=0;n<ngood;n++) 
    if (fscanf(fp,"%lf",&fit->rng[index[n]].w_l) !=1) return -1;
  for(n=0;n<ngood;n++) {
    fit->rng[index[n]].qflg=1;
    if ((fit->rng[index[n]].v<20) && 
       (fit->rng[index[n]].w_l<20)) fit->rng[index[n]].gsct=1;
  }
  return ftell(fp)-sptr;
} 
  
int SmrFread(FILE *fp,struct RadarParm *prm,struct FitData *fit,int beam) {
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
  int channel=0;
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
   
  TimeYrsecToYMDHMS(rec_time,year,&mo,&dy,&hr,&mt,&sc);
  if (bmnum==beam) prm->scan=1;
  else prm->scan=0;
  prm->nrang=MAX_RANGE;
  prm->stid=st_id;
  prm->atten=atten/10;
  prm->frang=frang;
  prm->rsep=rsep;     
  prm->bmnum=bmnum;
  prm->tfreq=tfreq;
  prm->noise.search=noise;
  prm->time.yr=year;
  prm->time.mo=mo;
  prm->time.dy=dy;
  prm->time.hr=hr;
  prm->time.mt=mt;
  prm->time.sc=sc;
  prm->time.us=0;
  prm->cp=cpid;
  prm->channel=channel;
  blen=ftell(fp)-sptr;
  status=SmrFreadData(fp,ngood,prm,fit);

  if (status ==-1) return -1;
  return blen+status;
}  




