/* setup.c
   ======= */

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
 $Log: setup.c,v $
 Revision 1.5  2001/06/27 21:10:39  barnes
 Added license tag

 Revision 1.4  2000/11/20 14:53:41  barnes
 Fixed bug in setup_hardware.

 Revision 1.3  2000/09/22 18:57:41  barnes
 Added function for logging the program start.

 Revision 1.2  1999/12/15 22:14:05  barnes
 Moved to individual hardware files.

 Revision 1.1  1999/03/31 19:58:35  barnes
 Initial revision

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "read_clock.h"
#include "types.h"
#include "limit.h"
#include "message.h"

#include "rawdata.h"
#include "fitdata.h"
#include "hardware.h"
#include "radar_name.h"

#include "fitacf.h"

#include "task_msg.h"

#include "freq_lib.h"
#include "setup.h"
 
struct radar_hardware *setup_hardware() {
  FILE *fp;
  char *envstr;
  struct radar_hardware *hdw=NULL;
  struct radar_id *rid=NULL;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    return NULL;
  }
 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);

  
  envstr=getenv("SD_HARDWARE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HARDWARE' must be defined.\n");
    return NULL;
  }

  hdw=load_hdw(envstr,rid);
  free(rid);
  return hdw;
}

struct task_id **setup_task(char **tasklist,int *num) {
  struct task_id **tsk=NULL;
  int i,j=0;
  for (i=0;tasklist[i] !=0;i++);
  tsk=malloc(sizeof(struct task_id)*i);
  if (tsk==NULL) return NULL;
  for (i=0;tasklist[i] !=0;i++) {
    tsk[j]=make_task_id(tasklist[i]);
    if (tsk[j] !=NULL) j++;
  }
  *num=j;
  return tsk;
}

struct fit_block *setup_fit(int id,struct radar_hardware *hdw) {
  struct fit_block *ptr=NULL;
  int yr,mo,dy,hr,mt,sc,ms,us;
  struct radar_site *hdblk;

  ptr=malloc(sizeof(struct fit_block));
  if (ptr==NULL) return 0;
  read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
  hdblk=get_site(hdw,yr,mo,dy,hr,mt,sc,id);
  start_fit(hdblk,yr,ptr);
  return ptr;
}

struct freq_table *get_freq_table() {
  char *envstr;
  FILE *fp;
  struct freq_table *ptr=NULL;
  envstr=getenv("SD_FREQ_TABLE");
  if (envstr==NULL) return NULL;
  fp=fopen(envstr,"r");
  if (envstr==NULL) return NULL;
  ptr=load_ftable(fp);
  fclose(fp);
  return ptr;
}
   
int get_st_id() {
  int st_id;
  char *envstr;
  envstr=getenv("SD_RADARID");
  if (envstr==NULL) return -1;
  st_id=atoi(envstr);  
  return st_id;
}
 
void log_start(struct task_id *ptr,char *name,int argc,char *argv[]) {
  char buffer[4096];
  int i;
  sprintf(buffer,START_STRING);
  for (i=0;i<argc;i++) {
    if (i !=0) strcat(buffer," ");
    strcat(buffer,argv[i]);
  }
  log_error(ptr,name,buffer);
 

}

