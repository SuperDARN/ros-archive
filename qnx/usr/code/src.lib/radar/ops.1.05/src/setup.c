/* setup.c
   ======= 
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "radar.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "fitacf.h"
#include "radar.h"
#include "taskid.h"
#include "freq.h"
#include "rmsg.h"
#include "errlog.h"
#include "radarshell.h"
#include "taskid.h"
#include "global.h"
#include "setup.h"

/*
 $Log: setup.c,v $
 Revision 1.2  2004/05/11 15:12:40  barnes
 Modification to deal with new libraries.

 Revision 1.1  2004/05/06 20:10:19  barnes
 Initial revision

*/


int OpsSetupRadar() {
  FILE *fp;
  char *envstr;
  struct RadarNetwork *network;  
  struct Radar *radar;

  envstr=getenv("SD_RADARID");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARID' must be defined.\n");
    exit(-1);
  }
  stid=atoi(envstr);  

  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
 
  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);
  
  radar=RadarGetRadar(network,stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }
  /*
  RadarFree(network);
  */

  envstr=getenv("SD_FREQ_TABLE");
  if (envstr !=NULL) fp=fopen(envstr,"r");
  else fp=NULL;
  if (fp !=NULL) {
    ftable=FreqLoadTable(fp);
    fclose(fp);
  }
  return 0;
}

int OpsFitACFStart() {
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
  if (site==NULL) return -1;
  FitACFStart(site,yr,&fblk); 
  return 0;
}

int OpsSetupTask(char **tasklist) {
  int i,j=0;
  for (i=0;tasklist[i] !=0;i++);
  tlist=malloc(sizeof(struct TaskID)*i);
  if (tlist==NULL) return 0;
  for (i=0;tasklist[i] !=0;i++) {
    tlist[j]=TaskIDMake(tasklist[i]);
    if (tlist[j] !=NULL) j++;
  }
  tnum=j;
  return 0;
}
 


void OpsLogStart(struct TaskID *ptr,char *name,int argc,char *argv[]) {
  char buffer[4096];
  int i;
  sprintf(buffer,START_STRING);
  for (i=0;i<argc;i++) {
    if (i !=0) strcat(buffer," ");
    strcat(buffer,argv[i]);
  }
  ErrLog(ptr,name,buffer);
}
