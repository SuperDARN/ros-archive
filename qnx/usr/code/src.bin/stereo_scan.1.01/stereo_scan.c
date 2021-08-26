/* test_scan.c
   =========== */

/* 
 $Log: stereo_scan.c,v $
 Revision 1.3  2000/09/19 13:16:54  barnes
 Provided for separate average counts for each channel.

 Revision 1.2  2000/04/17 21:43:54  barnes
 Added test for bad integrations.

 Revision 1.1  2000/03/19 20:38:42  barnes
 Initial revision

 Revision 1.1  2000/03/16 02:00:52  barnes
 Initial revision

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


#include "hardware.h"
#include "radar_name.h"



#include "task_msg.h"
#include "freq_lib.h"
#include "fclr_set.h"

#include "radar_s.h"
#include "tmseq_s.h"
#include "radar_shell.h"


#include "setup.h"

#include "shell_s.h"
#include "global_s.h"
#include "build_raw_s.h"
#include "read_data_s.h"
#include "fclr_s.h"

/* this is the list of tasks accessed */

#define DELAY_A 100
#define DELAY_B 0

#define CHN_A 0
#define CHN_B 1

#define A_D_NAME_A "a_d_drive_A"
#define A_D_NAME_B "a_d_drive_B"
#define DIO_NAME "/dio_drive"
#define ERRLOG_NAME "errlog"
#define SCHEDULER_NAME "schedule"
#define CONTROL_NAME "control_program"
#define TASK_NAMES "echo_data","raw_write","fit_write"

/* the control program name as recorded in the data files */

#define PROGRAM_NAME "stereo_scan"
#define CP_ID 0

#define TEST_DATA "/data/test/test.dat"

/* name of the driver tasks */

char *a_d_name[2]={A_D_NAME_A,A_D_NAME_B};
char *dio_name={DIO_NAME};

int cbmnum=8;

char errlog_name[40]={ERRLOG_NAME};
char scheduler_name[40]={SCHEDULER_NAME};
char program_name[40]={PROGRAM_NAME};

char logtxt[256];


char *tasklist[]=
 { TASK_NAMES,
  0};

	
struct rawdata raw_A;
struct rawdata raw_B;

struct fitdata fit_A;
struct fitdata fit_B;

struct task_block msg;
struct task_id **tsk=NULL;
struct task_id *etsk=NULL;

struct radar_hardware *hdw=NULL;
struct fit_block *fblk_A=NULL;    
struct fit_block *fblk_B=NULL;    
	
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
 
  int exit_poll=0;

  int tnum;

  pid_t sid;
  struct rawfp *rfp=NULL;

  st_id=get_st_id();

  ad_id_s[CHN_A]=locate_task_id(a_d_name[CHN_A]);
  ad_id_s[CHN_B]=locate_task_id(a_d_name[CHN_B]);
  dio_id_s=locate_task_id(dio_name);
  reset_id_s[CHN_A]=get_scan_reset(ad_id_s[CHN_A]);
  reset_id_s[CHN_B]=get_scan_reset(ad_id_s[CHN_B]);

  bufnum_s[CHN_A]=get_buf_num(ad_id_s[CHN_A]);
  bufsze_s[CHN_A]=get_buf_size(ad_id_s[CHN_A]);

  bufnum_s[CHN_B]=get_buf_num(ad_id_s[CHN_B]);
  bufsze_s[CHN_B]=get_buf_size(ad_id_s[CHN_B]);


  for (i=0;i<bufnum_s[CHN_A];i++) {
    bufadr_s[MAX_AD_BUF*CHN_A+i]=get_buf_adr(ad_id_s[CHN_A],i); 
  }

  for (i=0;i<bufnum_s[CHN_B];i++) {
    bufadr_s[MAX_AD_BUF*CHN_B+i]=get_buf_adr(ad_id_s[CHN_B],i); 
  }


  tsg_table=make_tsg_table(MAX_TSG);
  reset_dio(dio_id_s);

  
  fclr_set(dio_id_s,tsg_table,fclr_id);
  
  frq_table=get_freq_table();
  

  hdw=setup_hardware();

  tsk=setup_task(tasklist,&tnum);

  fblk_A=setup_fit(st_id,hdw);
  fblk_B=setup_fit(st_id,hdw);
    
  /* setup base shell variables */
 
  init_shell_s();

  cp=0;
  intt=3;

  start_freq_A=14000;

  option_add(&cmdlne,"db",'x',&debug);
  option_add(&cmdlne,"e",'t',&errlog_ptr);
  option_add(&cmdlne,"sc",'t',&scheduler_ptr);
  option_add(&cmdlne,"tf",'t',&test_file); 
  option_add(&cmdlne,"intt",'i',&intt);
  option_add(&cmdlne,"start_freq_A",'i',&start_freq_A);
  option_add(&cmdlne,"start_freq_B",'i',&start_freq_B);

  option_add(&cmdlne,"freq_range_A",'i',&freq_range_A);
  option_add(&cmdlne,"freq_range_A",'i',&freq_range_B);

  /* set up remaining shell variables */

  radar_shell_add(&rstable,"start_freq_A",var_LONG,&start_freq_A);
  radar_shell_add(&rstable,"freq_range_A",var_LONG,&freq_range_B);
  radar_shell_add(&rstable,"start_freq_B",var_LONG,&start_freq_A);
  radar_shell_add(&rstable,"freq_range_B",var_LONG,&freq_range_B);

  argnum=option_process(argc,argv,&cmdlne,cmdlne_err);
  if (argnum<argc) {
     
     optfp=fopen(argv[argnum],"r");
     if (optfp !=NULL) {
        option_file(optfp,&cmdlne,cmdlne_err);
        fclose(optfp);
     } else fprintf(stderr,"Failed to read option file.\n");
  }   

  etsk=make_task_id(errlog_ptr);
  sid=register_program(scheduler_ptr,CONTROL_NAME);

  delay_s[0]=DELAY_A;
  delay_s[1]=DELAY_B;

  mppul_A=7;
  mplgs_A=18;
  txpl_A=(rsep_A*20)/3;


  frang_B=1350;
  rsep_B=15;  
  mppul_B=7;
  mplgs_B=18;
  txpl_B=(rsep_B*20)/3;


  tfreq_A=12300; /* just a dummy frequency */
  tfreq_B=12300; 

  /* setup the comment block */

   strcpy(combf_A,PROGRAM_NAME);   
   strcpy(combf_B,PROGRAM_NAME);   


   /* log startup */  

  log_error(etsk,program_name,"Starting program.");
  log_error(etsk,program_name,argv[0]); 
  

  /* open dummy data file */

  if (test_file !=NULL) {
    log_error(etsk,program_name,"Opening test data file.");
    rfp=raw_open(test_file,NULL);
  }


  read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);
  
  for (i=0;i<tnum;i++) {
    task_reset(tsk[i]);
    task_open(tsk[i],strlen(program_name),
                     program_name,year,month,day,hour,minut,sec);
  }

   

  bmnum_B=8;

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

    scan_A=1;
    scan_B=1;

    log_error(etsk,program_name,"Starting scan.");

    /* set the stereo channel to a camping beam */

    set_channel(dio_id_s,CHN_B);
    set_beam(dio_id_s,bmnum_B);
    set_freq(dio_id_s,tfreq_B);
    set_channel(dio_id_s,CHN_A);

    for (bmnum_A=0;bmnum_A<16;bmnum_A++) {
     
      read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);

      /* log current beam */

      sprintf(logtxt,"Integrating beams:%d,%d intt:%d (%d:%d:%d)",bmnum_A,
                      bmnum_B,
                      intt,hour,minut,sec);

      log_error(etsk,program_name,logtxt);

      /* set beam */

      set_channel(dio_id_s,CHN_B);
      set_beam(dio_id_s,bmnum_B);
      set_channel(dio_id_s,CHN_A);
      set_beam(dio_id_s,bmnum_A);


      fclr_s(0,start_freq_A,start_freq_A+freq_range_A,5,
               start_freq_B,start_freq_B+freq_range_B,5,
               &tfreq_A,&tfreq_B);

      if (tfreq_A==-1) tfreq_A=frq_table->dfrq;
      if (tfreq_B==-1) tfreq_B=frq_table->dfrq;
       
      /* set beam */

      set_channel(dio_id_s,CHN_B);
      set_freq(dio_id_s,tfreq_B);
      set_channel(dio_id_s,CHN_A);
      set_freq(dio_id_s,tfreq_A);

      /* do integration */

      tsg_id_A=time_seq(0,dio_id_s,tsg_table,ptab);
      tsg_id_B=time_seq(1,dio_id_s,tsg_table,ptab);
      

      radar_s(debug,lags,lags);

      if (nave_A==0) log_error(etsk,program_name,"Integration A Failed.");
      if (nave_B==0) log_error(etsk,program_name,"Integration B Failed.");

      get_status_s(0,0);
      get_status_s(1,0);
   
      if (nave_A>0) {       

        if (rfp !=NULL) { /* read in test data */
          if (read_data_s(0,rfp) !=0) {
            raw_close(rfp);
            rfp=raw_open(TEST_DATA,NULL);
          }
          if (read_data_s(1,rfp) !=0) {
            raw_close(rfp);
            rfp=raw_open(TEST_DATA,NULL);
          }
        }
     
     
       
        build_raw_s(0,&raw_A,ptab,lags);
        build_raw_s(1,&raw_B,ptab,lags);
      
      
        fitacf(&raw_A,fblk_A,&fit_A);
        fitacf(&raw_B,fblk_B,&fit_B);
      
      

        msg.num=0;
        msg.tsize=0;
        task_add(&msg,sizeof(struct rawdata),(unsigned char *) &raw_A,0,
		     year,month,day,hour,minut,sec);
        task_add(&msg,sizeof(struct rawdata),(unsigned char *) &raw_B,0,
		     year,month,day,hour,minut,sec);

        task_add(&msg,sizeof(struct fitdata),(unsigned char *) &fit_A,1,
			 year,month,day,hour,minut,sec);
        task_add(&msg,sizeof(struct fitdata),(unsigned char *) &fit_B,1,
			 year,month,day,hour,minut,sec);

        task_add(&msg,strlen(program_name)+1,program_name,2,
                         year,month,day,hour,minut,sec);          
        for (i=0;i<tnum;i++) task_send(tsk[i],&msg);
      }


    
  
      exit_poll=radar_shell(sid,&rstable);
      
      if (exit_poll !=0) break;
      scan_A=0;
      scan_B=0;
    }
  } while (exit_poll==0);



  for (i=0;i<tnum;i++) {
    task_close(tsk[i],year,month,day,hour,minut,sec);
  }

  log_error(etsk,program_name,"Ending program.");

  /* tidy up and shutdown */


  terminate(sid);
  
} 
 
