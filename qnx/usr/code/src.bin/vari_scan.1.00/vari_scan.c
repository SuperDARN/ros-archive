/* vari_scan.c */
/* Last Modification 19-April-2002 */
/* By: Dieter Andre */
/* Make a program that allows easy implementation of various scan patterns */
/* through inclusion or read of the parameters from a file */

/*
 $Log: vari_scan.c,v $
 Revision 1.0 20020409 DA
 Initial Revision
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

#include "read_clock.h"
#include "log_error.h"
#include "task_write.h"
#include "build_raw.h"
#include "get_status.h"
#include "read_data.h"
#include "test_reopen.h"
#include "tmseq.h"

#include "radar_shell.h"

#include "global.h"
#include "setup.h"

#include "shell.h"

#include "calc_skip.h"
#include "wait_boundary.h"
#include "site.h"

#include "drivers.h"
#include "tasks.h"
#define TEST_DATA "/data/test/test.dat"

/* name of the driver tasks */
char *a_d_name={A_D_NAME};
char *dio_name={DIO_NAME};
char *tasklist[]={ TASK_NAMES, 0};
char errlog_name[40]={ERRLOG_NAME};
char scheduler_name[40]={SCHEDULER_NAME};

/* will be replaced by input from rcp file !!! DA */
char program_name[40]={"vari_scan"};
char program_id[80]={"$Id: vari_scan.c,v 1.00 20020419 DA $"};
  
char command[1024];
char logtxt[256];

struct rawdata raw;
struct fitdata fit;
struct task_block msg;
struct task_id **tsk=NULL;
struct task_id *etsk=NULL;

struct radar_hardware *hdw=NULL;
struct fit_block *fblk=NULL;

/* function to read rcp parameters !!! DA */
#include "read_rcp_parameters.c"
/* replacement for calc_skip !!! DA */
#include "vari_calc_skip.c"
/* maximum number of loops in the repetition !!! DA */
#define MAX_LOOP 200

void main(int argc,char *argv[]) {

   /* define pulse sequence and lag table */
  int ptab[7] = {0,9,12,20,22,26,27};
  int lags[2*18] ={ 0,26,20,9,22,22,20,20,12,0,12,9,0,9,12,12,9,9,
                    0,27,22,12,26,27,26,27,20,9,22,20,12,22,26,27,26,27};
  /* define other variables */
  int i,c;
  int status;
  int debug=0;
  FILE *optfp;
  char *test_file=NULL;
  struct rawfp *rfp=NULL;
  int exit_poll=0;
  int argnum=0;
  int tnum;
  int sbm,ebm;
  int sbmvl,ebmvl,skip_beam,beam_loop;
  char *errlog_ptr=errlog_name; /* the default names */
  char *scheduler_ptr=scheduler_name;
  pid_t sid;

  /* parameters for vari_scan !!! DA */
  int nloop;
  int bmnum_arr[MAX_LOOP];
  int frang_arr[MAX_LOOP];
  int rsep_arr[MAX_LOOP];
  int nrang_arr[MAX_LOOP];
  int intt_arr[MAX_LOOP];
  int xcf_arr[MAX_LOOP];
  int mpinc_arr[MAX_LOOP];
  int day_freq_arr[MAX_LOOP];
  int night_freq_arr[MAX_LOOP];
  int iloop; /* loop parameter for vari_scan */

  /* get station identifier */
  st_id=get_st_id();
  /* declare operating parameters */
/* this should not be necessary !!! DA
  cp=CP_ID;
  frang=day_frang;
  intt=7;
*/
  mppul=7;
  mplgs=18;
  /* call site library */
  start_program();
  /* setup base shell variables */
  /* read the rcp parameters */
  read_rcp_parameters( argv[1],
			  program_name,
			  program_id,
			  &cp,
			  &scan_period,
			  &nloop,
			  bmnum_arr,
			  frang_arr,
			  rsep_arr,
			  nrang_arr,
			  intt_arr,
			  xcf_arr,
			  mpinc_arr,
			  day_freq_arr,
			  night_freq_arr
			);

  /* register with the error log and the scheduler */
  etsk=make_task_id(errlog_ptr);
  sid=register_program(scheduler_ptr,CONTROL_NAME);
/* this should not be necessary !!! DA
  txpl=(rsep*20)/3;
*/
  /* setup the comment block */
  strcpy(command,argv[0]);
  for (c=1;c<argc;c++) {
    strcat(command," ");
    strcat(command,argv[c]);
  } 
  strncpy(combf,program_id,80);   
  /* log startup */
  log_start(etsk,program_name,argc,argv);
  /* configure the hardware */
  ad_id=locate_task_id(a_d_name);
  dio_id=locate_task_id(dio_name);
  if ((ad_id==-1) || (dio_id==-1)) {
    log_error(etsk,program_name,"Failed to locate hardware.");
    exit(-1);
  }
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
    task_open(tsk[i],strlen(command), command,year,month,day,hour,minut,sec);
  }
  read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);
  if (backward) usr_resS1=-1;
  else usr_resS1=1;
  sbm=start_beam;                                                               
  ebm=end_beam;
  do {  /*================== start of main loop =======================*/
    read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);
    if (start_scan()==0) continue;
    /* test and re-open files if necessary */
    if (test_reopen(2,0,0) !=0) {
      for (i=0;i<tnum;i++) {
        log_error(etsk,program_name,"Opening new files.");
         task_close(tsk[i],year,month,day,hour,minut,sec);
         task_open(tsk[i],strlen(command),command, year,month,day,hour,minut,sec);
      }
    }

    /* work out how many beams to skip */
    skip_beam= vari_calc_skip(scan_period, intt_arr, nloop);
    scan=1;

    log_error(etsk,program_name,"Starting scan.");

    usr_resS2= bmnum_arr[ 0];
    usr_resS3= bmnum_arr[ nloop - 1];

    iloop= skip_beam; /* !!! */
    do {  /*===================== inner loop ===================*/
      if (day_time()==1) {
         start_freq= day_freq_arr[ iloop];
         mpinc= mpinc_arr[ iloop];
         frang= frang_arr[ iloop];
      } else {
         start_freq= night_freq_arr[ iloop];
         mpinc= mpinc_arr[ iloop];
         frang= frang_arr[ iloop];
      }
      bmnum= bmnum_arr[ iloop];
      rsep= rsep_arr[ iloop];
      nrang= nrang_arr[ iloop];
      intt= intt_arr[ iloop];
      xcf= xcf_arr[ iloop]; 

      read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);
      /* THIS IS EMPTY */
      integrate();
      /* set beam */
      set_beam(dio_id,bmnum);
      /* do fclr */
      tfreq=fclr(debug,start_freq,start_freq+freq_range,5);
      if (tfreq==-1) tfreq=frq_table->dfrq;
      if (set_freq(dio_id,tfreq)==FREQ_LOCAL) log_error(etsk,program_name, "Frequency Synthesizer in local mode.\n");
      /* do integration */
      tsg_id=time_seq(dio_id,tsg_table,ptab);
      nave=radar(debug,lags);
      if (nave==0) log_error(etsk,program_name,"Integration Failed.");
      get_status(0); 
      if (nave>0) {
        if (rfp !=NULL) {
          if (read_data(rfp) !=0) {
            raw_close(rfp);
            rfp=raw_open(TEST_DATA,NULL);
          }
        }
        /* transmit data */
        build_raw(&raw,ptab,lags);
        fitacf(&raw,fblk,&fit);
        msg.num=0;
        msg.tsize=0;
        task_add(&msg,sizeof(struct rawdata),(unsigned char *) &raw,0,year,month,day,hour,minut,sec);
        task_add(&msg,sizeof(struct fitdata),(unsigned char *) &fit,1,year,month,day,hour,minut,sec); 
        task_add(&msg,strlen(program_name)+1,program_name,2,year,month,day,hour,minut,sec);          
        for (i=0;i<tnum;i++) task_send(tsk[i],&msg);
      }
      exit_poll=radar_shell(sid,&rstable);
      if (exit_poll !=0) break;
      scan=0;

      iloop= iloop + 1; /* !!! */
    } while (iloop < nloop); /*===== !!! end of inner loop ======*/

    if (exit_poll==0) wait_boundary(scan_period);
  } while (exit_poll==0); /*===================== end of main loop =============================*/
  for (i=0;i<tnum;i++) {
    task_close(tsk[i],year,month,day,hour,minut,sec);
  }
  end_program();
  terminate(sid);
} /* vari_scan_rcp */
