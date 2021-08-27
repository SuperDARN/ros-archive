/* test_scan.c
   =========== */

/* 
 $Log: test_scan.c,v $
 Revision 1.10  2000/03/15 14:04:06  barnes
 Fixed nasty bug that meant the timing sequence was not being updated when parameters were changed.

 Revision 1.9  1999/09/17 21:41:20  barnes
 Fixed bug in name of scheduler.

 Revision 1.8  1999/09/17 03:01:36  barnes
 Re-arranged the order of the code again.

 Revision 1.7  1999/09/17 02:55:11  barnes
 Re-arranged some of the code.

 Revision 1.6  1999/09/16 00:41:56  barnes
 Added sensible defaults.

 Revision 1.5  1999/09/15 23:54:04  barnes
 Changed ordering of the shell variables.

 Revision 1.4  1999/09/15 19:46:23  barnes
 Added command line options and generally tidied up.

 Revision 1.3  1999/09/10 00:20:20  barnes
 Modifications to the way the shell is controlled.

 Revision 1.2  1999/08/09 01:53:58  barnes
 Revisions to sync with analysis libraries.

 Revision 1.1  1999/04/02 20:24:22  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "option.h"

#include "limit.h"
#include "message.h"
#include "a_d_drive.h"
#include "dio.h"
#include "tsg.h"

#include "rawdata.h"
#include "fitdata.h"
#include "hardware.h"
#include "radar_name.h"
#include "fitacf.h"
#include "raw_read.h"

#include "task_msg.h"
#include "freq_lib.h"

#include "radar.h"

#include "fclr_set.h"
#include "fclr.h"

#include "radar_shell.h"

#include "global.h"
#include "setup.h"
#include "shell.h"


/* this is the list of tasks accessed */

#define A_D_NAME "a_d_drive"
#define DIO_NAME "/dio_drive"
#define ERRLOG_NAME "errlog"
#define SCHEDULER_NAME "schedule"
#define CONTROL_NAME "control_program"
#define TASK_NAMES "echo_data","raw_write","fit_write"


/* the control program name as recorded in the data files */

#define PROGRAM_NAME "test_scan"
#define CP_ID 0

#define TEST_DATA "/data/test/test.dat"

/* name of the driver tasks */

char *a_d_name={A_D_NAME};
char *dio_name={DIO_NAME};

char *tasklist[]=
 { TASK_NAMES,
  0};


char errlog_name[40]={ERRLOG_NAME};
char scheduler_name[40]={SCHEDULER_NAME};
char program_name[40]={PROGRAM_NAME};

char logtxt[256];



struct rawdata raw;
struct fitdata fit;
struct task_block msg;
struct task_id **tsk=NULL;
struct task_id *etsk=NULL;

struct radar_hardware *hdw=NULL;
struct fit_block *fblk=NULL;    
	
void main(int argc,char *argv[]) {

  int ptab[7] = {0,9,12,20,22,26,27};
  int lags[2*18] ={ 
    0,26,20,9,22,22,20,20,12,0,12,9,0,9,12,12,9,9,

    0,27,22,12,26,27,26,27,20,9,22,20,12,22,26,27,26,27};



  int i;
  int status;
  int argnum=0;
  unsigned char debug=0;
  FILE *optfp;
  char *test_file=NULL;

  char *errlog_ptr=errlog_name; /* the default names */
  char *scheduler_ptr=scheduler_name;
 
  struct rawfp *rfp=NULL;

  int exit_poll=0;

  int tnum;

  pid_t sid;
 

  st_id=get_st_id();

  ad_id=locate_task_id(a_d_name);
  dio_id=locate_task_id(dio_name);
  reset_id=get_scan_reset(ad_id);

  bufnum=get_buf_num(ad_id);
  bufsze=get_buf_size(ad_id);

  for (i=0;i<bufnum;i++) {
    bufadr[i]=get_buf_adr(ad_id,i); 
  }

  tsg_table=make_tsg_table(MAX_TSG);

  reset_dio(dio_id);

  fclr_set(dio_id,tsg_table,fclr_id);

  hdw=setup_hardware();
  tsk=setup_task(tasklist,&tnum);
  fblk=setup_fit(st_id,hdw);
  frq_table=get_freq_table();
 
  /* setup base shell variables */
 
  init_shell();



  /* declare default parameters */ 
 
  cp=0;
  intt=3;

  /* set up the command line options */

  option_add(&cmdlne,"db",'x',&debug);
  option_add(&cmdlne,"e",'t',&errlog_ptr);
  option_add(&cmdlne,"sc",'t',&scheduler_ptr);
  option_add(&cmdlne,"tf",'t',&test_file); 
  option_add(&cmdlne,"intt",'i',&intt);
  option_add(&cmdlne,"frang",'i',&frang);
  option_add(&cmdlne,"rsep",'i',&rsep);
  option_add(&cmdlne,"start_freq",'i',&start_freq);
  option_add(&cmdlne,"freq_range",'i',&freq_range);

  /* set up remaining shell variables */

  radar_shell_add(&rstable,"start_freq",var_LONG,&start_freq);
  radar_shell_add(&rstable,"freq_range",var_LONG,&freq_range);

  /* declare default parameters */ 
 
  cp=0;
  intt=3;

  argnum=option_process(argc,argv,&cmdlne,cmdlne_err);
  if (argnum<argc) {
     
     optfp=fopen(argv[argnum],"r");
     if (optfp !=NULL) {
        option_file(optfp,&cmdlne,cmdlne_err);
        fclose(optfp);
     } else fprintf(stderr,"Failed to read option file.\n");
  }   

 /* register with the error log and the scheduler */

  etsk=make_task_id(errlog_ptr);
  sid=register_program(scheduler_ptr,CONTROL_NAME);


  mppul=7;
  mplgs=18;
  txpl=(rsep*20)/3;

 
  /* setup the comment block */

   strcpy(combf,PROGRAM_NAME);   


   /* log startup */  

  log_error(etsk,program_name,"Starting program.");
  log_error(etsk,program_name,argv[0]); 
  
  /* open dummy data file */

  if (test_file !=NULL) {
    log_error(etsk,program_name,"Opening test data file.");
    rfp=raw_open(test_file,NULL);
  }

  read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);
  
  log_error(etsk,program_name,"Resetting tasks.");

  /* open the first set of files */

  for (i=0;i<tnum;i++) {
    task_reset(tsk[i]);
    task_open(tsk[i],strlen(program_name),
                     program_name,year,month,day,hour,minut,sec);
  }

  do {

    /* test and re-open files if necessary */

    if (test_reopen(2,0,0) !=0) {
      log_error(etsk,program_name,"Opening new files.");
      for (i=0;i<tnum;i++) {
         task_close(tsk[i],year,month,day,hour,minut,sec);
         task_open(tsk[i],strlen(program_name),program_name,
                   year,month,day,hour,minut,sec);
      }
    }
    
    scan=1;

    log_error(etsk,program_name,"Starting scan.");

    for (bmnum=0;bmnum<16;bmnum++) {
     
      read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);

      /* log current beam */

      sprintf(logtxt,"Integrating beam:%d intt:%d (%d:%d:%d)",bmnum,
                      intt,hour,minut,sec);

      log_error(etsk,program_name,logtxt);

      
      /* do clear frequency search */
  

      tfreq=fclr(debug,start_freq,start_freq+freq_range,5);
	  if (tfreq==-1) tfreq=frq_table->dfrq;

      /* set beam */

      set_beam(dio_id,bmnum);
      set_freq(dio_id,tfreq);

      /* do integration */
      tsg_id=time_seq(dio_id,tsg_table,ptab);

      nave=radar(debug,lags);
      get_status(0);
     
      if (rfp !=NULL) {
        if (read_data(rfp) !=0) {
          raw_close(rfp);
          rfp=raw_open(TEST_DATA,NULL);
        }
      }
     
      /* build the raw data structure */
     
      build_raw(&raw,ptab,lags);

      /* perform fitting */

      fitacf(&raw,fblk,&fit);

      /* transmit data to other tasks */

      msg.num=0;
      msg.tsize=0;
      task_add(&msg,sizeof(struct rawdata),(unsigned char *) &raw,0,
		     year,month,day,hour,minut,sec);
      task_add(&msg,sizeof(struct fitdata),(unsigned char *) &fit,1,
			 year,month,day,hour,minut,sec);
      task_add(&msg,strlen(program_name)+1,program_name,2,
                         year,month,day,hour,minut,sec);          
      for (i=0;i<tnum;i++) task_send(tsk[i],&msg);


      /* check for shell or scheduler events */

      exit_poll=radar_shell(sid,&rstable);

    

      if (exit_poll !=0) break;
      scan=0;
    }
  } while (exit_poll==0);

  /* close all open files */

  for (i=0;i<tnum;i++) {
    task_close(tsk[i],year,month,day,hour,minut,sec);
  }

  log_error(etsk,program_name,"Ending program.");

  /* tidy up and shutdown */


  terminate(sid);

  
} 
 
