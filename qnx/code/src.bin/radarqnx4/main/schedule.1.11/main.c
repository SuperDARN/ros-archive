/* main.c
   ======
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sched.h>
#include <sys/name.h>
#include <unistd.h>
#include "option.h"
#include "schedule.h"
#include "refresh.h"
#include "check_program.h"
#include "errstr.h"
#include "log_info.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: main.c,v $
 Revision 1.12  2004/06/22 13:13:39  barnes
 Added missing header.

 Revision 1.11  2004/06/22 13:12:00  barnes
 QNX4 Code audit.

 Revision 1.10  2004/05/10 15:19:06  barnes
 Modifcation to deal with new libraries.

 Revision 1.9  2002/02/08 03:55:43  barnes
 Added Paul Hamadyk's scheduler patch to fix Leicester's problems.

 Revision 1.8  2001/06/27 20:56:24  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 1.6  2000/11/20 14:39:48  barnes
 Fixed minor bugs in the code.

 Revision 1.5  2000/10/30 17:25:51  barnes
 Fixed possible fault if the schedule file is missing.

 Revision 1.4  2000/09/18 20:45:40  barnes
 Added code so that the schedule is reloaded the moment it is altered.

 Revision 1.3  1999/04/10 20:50:43  barnes
 Added help message system.

 Revision 1.2  1999/03/30 19:26:10  barnes
 Moved task name into a heade.

 Revision 1.1  1999/03/30 18:31:14  barnes
 Initial revision

 */


#define SCHEDULE_PRIORITY 12

struct scd_blk schedule;
struct scd_blk save_sched;


char dscdname[32]={TASK_NAME};
char dlogname[256]={"scdlog"};

char *scdname=NULL;
char *logname=NULL;

int arg=0;
struct OptionData opt;


void main(int argc,char *argv[]) {

  unsigned char dyflg=0;
  unsigned char hrflg=0;
  unsigned char tmtflg=0;
  unsigned char mtflg=0;

  FILE *fp;
  int tid;
  int tick;
  int help=0;
  char logtxt[256];
  

  schedule.refresh=10*60;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"d",'x',&dyflg);
  OptionAdd(&opt,"h",'x',&hrflg);
  OptionAdd(&opt,"t",'x',&tmtflg);
  OptionAdd(&opt,"m",'x',&mtflg);
  OptionAdd(&opt,"l",'t',&logname);
  OptionAdd(&opt,"n",'t',&scdname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (argc==arg) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }
 
  if (dyflg) schedule.refresh=24*3600;
  if (hrflg) schedule.refresh=3600;
  if (tmtflg) schedule.refresh=10*60;
  if (mtflg) schedule.refresh=60;
  if (logname==NULL) logname=dlogname;
  if (scdname==NULL) scdname=dscdname;
  strcpy(schedule.name,argv[arg]);

  

  if(setprio(0,SCHEDULE_PRIORITY) == -1) {
    perror("unable to set priority\n");
    exit(-1);
  }
   
  /* register the scheduler with a name */

  if ( (tid=qnx_name_attach(0,scdname) ) == -1) { 
    perror("Unable to attach scheduler");
    exit(-1);
  }

  schedule.num=0;
  schedule.pid=-1;
  schedule.cnt=0;
  strcpy(schedule.command,"Null"); 
  strcpy(schedule.path,"/");
  fp=fopen(schedule.name,"r");  
  if (fp==NULL) {
    log_info(0,"Schedule file not found");
    exit(-1);
  }
  if (load_schedule(fp,&schedule) !=0) {
    log_info(0,"Error reading schedule file");
    exit(-1);
  }
  fclose(fp);
  schedule.cnt=set_schedule(&schedule);
  check_program(&schedule,schedule.cnt-1);
  print_schedule(&schedule);
   /* okay this is the main loop */

   do {

     /* this bit of code from Paul Hamadyk - it fixes Leicester's 
        problem of trying to upload the schedule into the current
        schedule file. eg. FTP'ing into the existing schedule - This
        is a BAD thing to do. The fix works, and is the only possible
        software fix.
     */

      if (test_refresh(&schedule) != 0) {
        struct stat nstat1, nstat2;
        log_info(0, "Waiting for schedule file.");
        stat(schedule.name, &nstat1);
        sleep(10);
        stat(schedule.name, &nstat2);
        while (nstat1.st_mtime != nstat2.st_mtime) {
          log_info(0, "File is being modified.");
          stat(schedule.name, &nstat1);
          sleep(10);
          stat(schedule.name, &nstat2);
        }
        log_info(0, "Refreshing schedule");
        fp = fopen(schedule.name, "r");
        if (fp == NULL) {
           sprintf(logtxt, "Schedule file %s not found", schedule.name);
           log_info(0, logtxt);
        } else {
          memcpy(&save_sched, &schedule, sizeof(schedule));
          if (load_schedule(fp, &schedule) != 0) {
            log_info(0, "Error reading updated schedule file");
            memcpy(&schedule, &save_sched, sizeof(schedule));
          }
          fclose(fp);
        }
        schedule.cnt = set_schedule(&schedule);
        fprintf(stderr, "schedule.cnt=%d\n", schedule.cnt);
        check_program(&schedule, schedule.cnt - 1);
        print_schedule(&schedule);
      }

  
      /*
     if (test_refresh(&schedule) !=0) {
        log_info(0,"Refreshing schedule");
        fp=fopen(schedule.name,"r");
        if (fp==NULL) {
          sprintf(logtxt,"Schedule file %s not found",schedule.name);
          log_info(0,logtxt); 
        } else {
          if (load_schedule(fp,&schedule) !=0) {
            log_info(0,"Error reading schedule file");
            break; 
          }
	}
        schedule.cnt=set_schedule(&schedule);
        fprintf(stderr,"schedule.cnt=%d\n",schedule.cnt);
        check_program(&schedule,schedule.cnt-1);
        print_schedule(&schedule);
        if (fp !=NULL) fclose(fp);
     }
     */
     if (test_schedule(&schedule) !=0) {
       check_program(&schedule,schedule.cnt);
       schedule.cnt=set_schedule(&schedule);
     }
     sleep(1);
     tick++;
     tick=tick % 10;
     if ((tick==0) && (schedule.pid !=0)) { 
        /* check the programs health */
        if (getprio(schedule.pid)==-1) {
           schedule.pid=-1;
           log_info(0,"Program has died");
           check_program(&schedule,schedule.cnt-1);
        }
     }
   } while (1);     


}
  
