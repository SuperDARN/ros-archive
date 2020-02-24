/* fast_scan_1200.c
   ================== */

/*
 $Log: fast_scan_1200.c,v $
 Revision 1.1  2002/06/07 20:02:40  barnes
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

#define CP_ID 162

#define TEST_DATA "/data/test/test.dat"

/* name of the driver tasks */

char *a_d_name={A_D_NAME};
char *dio_name={DIO_NAME};

char *tasklist[]=
 { TASK_NAMES,
  0};


char errlog_name[40]={ERRLOG_NAME};
char scheduler_name[40]={SCHEDULER_NAME};
char program_name[40]={"fast_scan_1200"};
char program_id[80]={"$Id: fast_scan_1200.c,v 1.1 2002/06/07 20:02:40 barnes Exp $"};
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

  int day_mpincA,night_mpincA,mpincA;
  int day_mpincB,night_mpincB,mpincB;
  int mppulA,mplgsA;
  int mppulB,mplgsB;
  int tsg_idA,tsg_idB; 


  int ptabA[7] = {0,9,12,20,22,26,27};
  int lagsA[2*18] ={ 
    0,26,20,9,22,22,20,20,12,0,12,9,0,9,12,12,9,9,
    0,27,22,12,26,27,26,27,20,9,22,20,12,22,26,27,26,27};


  int ptabB[8] = {0,19,28,31,39,41,45,46};
  int lagsB[2*19] ={
    
    0,45,39,28,41,41,39,39,31,19,31,28,19,28,31,31,28,28,0,
    0,46,41,31,45,46,45,46,39,28,41,39,31,41,45,46,45,46,19};

 
  /* define other variables */

  int toggle=0;
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
  intt=3;
  scan_period=60;

  mppulA=7;
  mplgsA=18;
  mppulB=8;
  mplgsB=19;
 
  day_mpincB=1200;
  night_mpincB=1200;
  day_mpincA=day_mpinc;
  night_mpincA=night_mpinc;

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
         if (toggle==0) mpinc=day_mpincA;
         else mpinc=day_mpincB;
         frang=day_frang;
      } else {
         start_freq=night_start_freq;
         if (toggle==0) mpinc=night_mpincA;
         else mpinc=night_mpincB;
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
     
      if (toggle==0) {
        mppul=mppulA;
        mplgs=mplgsA;
        tsg_id=time_seq(dio_id,tsg_table,ptabA);
      } else {
        mppul=mppulB;
        mplgs=mplgsB;
        tsg_id=time_seq(dio_id,tsg_table,ptabB);
   
      }
     

      if (toggle==0) nave=radar(debug,lagsA);
      else nave=radar(debug,lagsB);
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
    
        if (toggle==0) build_raw(&raw,ptabA,lagsA);
        else build_raw(&raw,ptabB,lagsB);
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
    toggle=! toggle;
  } while (exit_poll==0);

  for (i=0;i<tnum;i++) {
    task_close(tsk[i],year,month,day,hour,minut,sec);
  }

  end_program();

  terminate(sid);



  
} 
 
