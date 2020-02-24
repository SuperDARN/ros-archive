/* normal_scan.c
   ============= */

/* 
 $Log: normal_scan.c,v $
 Revision 1.21  2000/11/29 17:16:37  barnes
 Fixed possible buffer overflows with comment buffer.

 Revision 1.20  2000/11/20 16:27:14  barnes
 Added missing headers and added reporting of synthesizer status.

 Revision 1.19  2000/11/20 15:32:09  barnes
 Fixed numerous bugs.

 Revision 1.18  2000/11/14 22:17:38  barnes
 Changed the way the program name is reported.

 Revision 1.17  2000/11/14 21:39:46  barnes
 Modifications to tidy up error reporting.

 Revision 1.16  2000/10/23 20:07:17  barnes
 Minor tidying up.

 Revision 1.15  2000/09/22 19:07:08  barnes
 Added startup log function.

 Revision 1.14  2000/04/17 21:39:50  barnes
 Added log for bad integrations.

 Revision 1.13  2000/03/22 00:35:12  barnes
 Moved task names to a header.

 Revision 1.12  2000/03/15 14:03:25  barnes
 Fixed nasty bug that meant the timing sequence was not being updated
 when parameters were changed.

 Revision 1.11  1999/09/17 21:28:37  barnes
 Removed some debugging code.

 Revision 1.10  1999/09/17 21:27:48  barnes
 Fixed bug in the scheduler name.

 Revision 1.9  1999/09/17 17:37:59  barnes
 Added flag for setting discretionary mode.

 Revision 1.8  1999/09/17 17:31:49  barnes
 Moved day and night functions to the library.

 Revision 1.7  1999/09/17 03:01:26  barnes
 Added extra logging code.

 Revision 1.6  1999/09/16 22:42:34  barnes
 Tidied up some of the global features.

 Revision 1.5  1999/09/16 22:11:49  barnes
 Added support for site specific stuff.

 Revision 1.4  1999/09/10 00:24:20  barnes
 Modified the way the shell is created.

 Revision 1.3  1999/06/07 16:05:45  barnes
 Added support for day and night operations.

 Revision 1.2  1999/04/13 12:52:12  barnes
 Fixed bug introduced when testing the file re-open.

 Revision 1.1  1999/04/02 20:24:39  barnes
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

#define CP_ID 150

#define TEST_DATA "/data/test/test.dat"

/* name of the driver tasks */

char *a_d_name={A_D_NAME};
char *dio_name={DIO_NAME};

char *tasklist[]=
 { TASK_NAMES,
  0};


char errlog_name[40]={ERRLOG_NAME};
char scheduler_name[40]={SCHEDULER_NAME};
char program_name[40]={"normal_scan"};
char program_id[80]={"$Id: normal_scan.c,v 1.21 2000/11/29 17:16:37 barnes Exp $"};
char command[1024];

char logtxt[256];

struct rawdata raw;
struct fitdata fit;
struct task_block msg;
struct task_id **tsk=NULL;
struct task_id *etsk=NULL;

struct radar_hardware *hdw=NULL;
struct fit_block *fblk=NULL;    

void main(int argc,char *argv[]) {

   /* define pulse sequence and lag table */

  int ptab[7] = {0,9,12,20,22,26,27};
  int lags[2*18] ={ 
    0,26,20,9,22,22,20,20,12,0,12,9,0,9,12,12,9,9,
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

  /* get station identifier */

  st_id=get_st_id();
 

  /* declare operating parameters */
 
  cp=CP_ID;
  frang=day_frang;
  intt=7;
  mppul=7;
  mplgs=18;
 
  /* call site library */

  start_program();

  /* setup base shell variables */
 
  init_shell();

  /* set up the command line options */

  option_add(&cmdlne,"di",'x',&discretion);
  option_add(&cmdlne,"db",'x',&debug);
  option_add(&cmdlne,"e",'t',&errlog_ptr);
  option_add(&cmdlne,"sc",'t',&scheduler_ptr);
  option_add(&cmdlne,"tf",'t',&test_file); 
  option_add(&cmdlne,"intt",'i',&intt);
  option_add(&cmdlne,"frang",'i',&frang);
  option_add(&cmdlne,"rsep",'i',&rsep);

  option_add(&cmdlne,"dt",'i',&day_start_hr);
  option_add(&cmdlne,"nt",'i',&night_start_hr);
  option_add(&cmdlne,"df",'i',&day_start_freq);
  option_add(&cmdlne,"nf",'i',&night_start_freq);
  option_add(&cmdlne,"dr",'i',&day_frang);
  option_add(&cmdlne,"nr",'i',&night_frang);
  option_add(&cmdlne,"dm",'i',&day_mpinc);
  option_add(&cmdlne,"nm",'i',&night_mpinc);
  option_add(&cmdlne,"xcf",'i',&xcount);
  option_add(&cmdlne,"sb",'i',&start_beam);
  option_add(&cmdlne,"eb",'i',&end_beam);

  option_add(&cmdlne,"freq_range",'i',&freq_range);

 /* setup the remaining shell variables */

  radar_shell_parse(&rstable,                                      
                    "sbm l ebm l \                           
                     day_start_freq l night_start_freq l \               
                     day_frang l night_frang l day_mpinc l \             
                     night_mpinc l freq_range l xcf l",                        
                     &sbm,&ebm,                              
                     &day_start_freq,&night_start_freq,                  
                     &day_frang,&night_frang,                            
                     &day_mpinc,&night_mpinc,                            
                     &freq_range,
                     &xcf);        

  /* process the command line options */

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

  txpl=(rsep*20)/3;

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
    task_open(tsk[i],strlen(command),
                     command,year,month,day,hour,minut,sec);
  }

  read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);

 

  if (backward) usr_resS1=-1;
  else usr_resS1=1;

  sbm=start_beam;                                                               
  ebm=end_beam;                                                                 
 

  do {

    read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);

    if (start_scan()==0) continue;

    /* test and re-open files if necessary */

    if (test_reopen(2,0,0) !=0) {

      for (i=0;i<tnum;i++) {
        log_error(etsk,program_name,"Opening new files.");
         task_close(tsk[i],year,month,day,hour,minut,sec);
         task_open(tsk[i],strlen(command),command,
                   year,month,day,hour,minut,sec);
      }
    }

  
     if (xcount>0) {
      count++;
      if (count==xcount) {
        xcf=1;
        count=0;
      } else xcf=0;
    } else xcf=0;

     /* work out how many beams to skip */

    skip_beam=calc_skip(scan_period);

    scan=1;
    
    ebmvl=ebm;
    if (backward) {
      sbmvl=sbm-skip_beam;
      if (sbmvl<ebmvl) sbmvl=sbm;
    } else {
      sbmvl=sbm+skip_beam;
      if (sbmvl>ebmvl) sbmvl=sbm+1;
    }
    bmnum=sbmvl;

    log_error(etsk,program_name,"Starting scan.");

    if ((sbm !=start_beam) || (ebm !=end_beam)) {
       usr_resS2=sbm;
       usr_resS3=ebm;
    } else {
      usr_resS2=0;
      usr_resS3=0;
    }

  

    do {
   
     if (day_time()==1) {
         start_freq=day_start_freq;
         mpinc=day_mpinc;
         frang=day_frang;
      } else {
         start_freq=night_start_freq;
         mpinc=night_mpinc;
         frang=night_frang;
      } 
     
      read_clock(&year,&month,&day,&hour,&minut,&sec,&msec,&usec);
      
      integrate();


      /* set beam */
      set_beam(dio_id,bmnum);

      /* do fclr */
  
      tfreq=fclr(debug,start_freq,start_freq+freq_range,5);
      if (tfreq==-1) tfreq=frq_table->dfrq;
  
      if (set_freq(dio_id,tfreq)==FREQ_LOCAL) 
         log_error(etsk,program_name,
                   "Frequency Synthesizer in local mode.\n");



    

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
        task_add(&msg,sizeof(struct rawdata),(unsigned char *) &raw,0,
		     year,month,day,hour,minut,sec);
        task_add(&msg,sizeof(struct fitdata),(unsigned char *) &fit,1,
			 year,month,day,hour,minut,sec); 
        task_add(&msg,strlen(program_name)+1,program_name,2,
                         year,month,day,hour,minut,sec);          
    
        for (i=0;i<tnum;i++) task_send(tsk[i],&msg);
      }
      exit_poll=radar_shell(sid,&rstable);
      

      if (exit_poll !=0) break;
      scan=0;

      if (backward) {
        bmnum--;
        beam_loop=(bmnum>=ebmvl);
      } else {
        bmnum++;
        beam_loop=(bmnum<=ebmvl);
      }
    } while (beam_loop==1);
    if (exit_poll==0) wait_boundary(scan_period);

  } while (exit_poll==0);

  for (i=0;i<tnum;i++) {
    task_close(tsk[i],year,month,day,hour,minut,sec);
  }

  end_program();

  terminate(sid);



  
} 
 
