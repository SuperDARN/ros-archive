/* main.c
   ====== */

/*
 $Log: main.c,v $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "print_info.h"
#include "schedule.h"
#include "errstr.h"
#include "log_info.h"
#include "task_name.h"
#include "hlpstr.h"

#define SCHEDULE_PRIORITY 12

struct scd_blk schedule;

char schedule_name[32]={TASK_NAME};
extern char logname[256];

void main(int argc,char *argv[]) {
  FILE *fp;
  int tid;
  int tick;
  int help=0;
  char logtxt[256];
  schedule.refresh=10*60;

  if (argc>1) {
    int c;
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-d")==0) schedule.refresh=24*3600;
      else if (strcmp(argv[c],"-h")==0) schedule.refresh=3600;
      else if (strcmp(argv[c],"-t")==0) schedule.refresh=10*60;
      else if (strcmp(argv[c],"-m")==0) schedule.refresh=60;
      else if (strcmp(argv[c],"-l")==0) {
        strcpy(logname,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-n")==0) {
        strcpy(schedule_name,argv[c+1]);
        c++;
      }
     }
     strcpy(schedule.name,argv[argc-1]);
   } else {
     int i;
     for (i=0;errstr[i] !=0;i++) fprintf(stderr,errstr[i]);
     exit(-1);
   }
 
   if (help==1) {
     print_info(stdout,hlpstr);
     exit(0);
   }

   if(setprio(0,SCHEDULE_PRIORITY) == -1) {
     perror("unable to set priority\n");
	 exit(-1);
   }
   
   /* register the scheduler with a name */

   if ( (tid=qnx_name_attach(0,schedule_name) ) == -1) { 
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
     if (test_refresh(&schedule) !=0) {
        log_info(0,"Refreshing schedule");
        fp=fopen(schedule.name,"r");
        if (fp==NULL) {
          sprintf(logtxt,"Schedule file %s not found",schedule.name);
          log_info(0,logtxt); /* continue with old schedule */
        } else {
          if (load_schedule(fp,&schedule) !=0) {
            log_info(0,"Error reading schedule file");
            break; /* bad schedule so quit */
          }
	}
        schedule.cnt=set_schedule(&schedule);
        fprintf(stderr,"schedule.cnt=%d\n",schedule.cnt);
        check_program(&schedule,schedule.cnt-1);
        print_schedule(&schedule);
        if (fp !=NULL) fclose(fp);
     }
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
  
