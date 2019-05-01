/* check_program.c
   ===============
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
#include <unistd.h>
#include <sys/sched.h>
#include <sys/types.h>
#include "log_info.h"
#include "schedule.h"
#include "execute.h"

/*
 $Log: check_program.c,v $
 Revision 1.6  2004/06/22 13:12:00  barnes
 QNX4 Code audit.

 Revision 1.5  2004/05/10 15:19:06  barnes
 Modifcation to deal with new libraries.

 Revision 1.4  2001/06/27 20:56:24  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 1.2  2000/09/18 20:45:40  barnes
 Added code so that the schedule is reloaded the moment it is altered.

 Revision 1.1  1999/03/30 18:29:38  barnes
 Initial revision

 */


int confirm_program(char *path,char *prog) {
  char fname[2*SCHED_LINE_LENGTH+1];
  char *fptr;
  FILE *fp;
  sprintf(fname,"%s/%s",path,prog);
  fptr=strtok(fname," "); 
  fp=fopen(fptr,"r");
  if (fp !=NULL) fclose(fp);
  return (fp !=NULL);
}  
  



void check_program(struct scd_blk *ptr,int cnt) {
  char txt[1024];
  if (cnt<0) cnt=0;
  if ((ptr->pid !=-1) && (getprio(ptr->pid) !=-1) &&
      (strcmp(ptr->command,ptr->entry[cnt].command)==0)) return;
   
  if (confirm_program(ptr->path,ptr->entry[cnt].command)==0) {
    sprintf(txt,"Program %s not found",ptr->entry[cnt].command);
    log_info(0,txt);
     return;
  }
  /* stop the old program here */
  if (ptr->pid !=-1) {
    log_info(0,"Stopping current program");
    terminate(ptr->pid);
  } 
  sprintf(txt,"Starting program:%s",ptr->entry[cnt].command);
  log_info(0,txt);
  if ((ptr->pid=execute(ptr->path,ptr->entry[cnt].command))==-1) {
    log_info(0,"Program failed to start - trying default program");
    if (confirm_program(ptr->path,ptr->entry[0].command)==0) {
      log_info(0,"Default program not found");  
      return;
    }
    if ((ptr->pid=execute(ptr->path,ptr->entry[0].command))==-1) {
      log_info(0,"Default program failed to start");
      return;
    }
  }  

  strcpy(ptr->command,ptr->entry[cnt].command);
  return;
}
