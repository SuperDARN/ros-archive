/* fte_4204.c
   ========== */

/* 
 $Log: fte_4204.c,v $
 Revision 1.9  2000/11/29 17:16:37  barnes
 Fixed possible buffer overflows with comment buffer.

 Revision 1.8  2000/11/20 16:27:14  barnes
 Added missing headers and added reporting of synthesizer status.

 Revision 1.7  2000/11/20 15:32:09  barnes
 Fixed numerous bugs.

 Revision 1.6  2000/11/14 22:15:50  barnes
 Changed the reporting of the program name.

 Revision 1.5  2000/11/14 21:41:09  barnes
 Modification to improve error reporting.

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



 /* fte specific */

  int cs_flag=0;
  int beam_step_quick=0;
  int start_beam_quick=0;
  int end_beam_quick=0;
  int day_start_beam_quick=0;
  int day_end_beam_quick=0;
  int night_start_beam_quick=0;
  int night_end_beam_quick=0;
  int start_beam_temp=0;
  int end_beam_temp=0;
  int night_start_beam=0;
  int night_end_beam=0;
  int special_beam=0;
  int day_special_beam=0;
  int night_special_beam=0;


int set_fte(int st_id) {

  #define RSEP 30
  #define INTT 3
  #define BEAM_STEP 2
  #define COMPLETE_SCAN 1

  scan_period=180;

  intt=INTT;
  rsep = RSEP;  
  cs_flag = COMPLETE_SCAN;

  beam_step_quick = BEAM_STEP;
  

  if (st_id==1) {
    start_beam_temp = 0;
    end_beam_temp = 13;
    start_beam_quick = 5;
    end_beam_quick = 9;
    day_start_beam_quick = 5;
    day_end_beam_quick = 9;
    night_start_beam_quick = 5;
    night_end_beam_quick = 9;
    special_beam = 5;
    day_special_beam = 5;
    night_special_beam = 5;
  }


  if (st_id==3) {
    start_beam_temp = 15;
    end_beam_temp = 2;
    start_beam_quick = 12;
    end_beam_quick = 8;
    day_start_beam_quick = 12;
    day_end_beam_quick = 8;
    night_start_beam_quick = 12;
    night_end_beam_quick = 8;
    special_beam = 12;
    day_special_beam = 12;
    night_special_beam = 12;
  }

  return 0;

}

#define CP_ID 4204

#define TEST_DATA "/data/test/test.dat"

/* name of the driver tasks */

char *a_d_name={A_D_NAME};
char *dio_name={DIO_NAME};

char *tasklist[]=
 { TASK_NAMES,
  0};


char errlog_name[40]={ERRLOG_NAME};
char scheduler_name[40]={SCHEDULER_NAME};
char program_name[40]={"fte_4204"};
char program_id[80]={"$Id: fte_4204.c,v 1.9 2000/11/29 17:16:37 barnes Exp $"};

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

  /* fte stuff */

  int cs_step=11;
  int beam_step=1;
  int scan_count=0;
  int sub_scan_count=0;
  int ns_beam_count=0;
  int store_beam=0;



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

  set_fte(st_id);

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

 log_error(etsk,program_name,"Resetting tasks.");


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


   /* fte specific stuff */

    if(cs_flag == 1) {
      if (scan_count % cs_step == 0)  { /* We're in a complete scan */
        start_beam = start_beam_temp;
        end_beam = end_beam_temp;
        beam_step = 1;
        usr_resL1 = 1;
      } else {
        start_beam = start_beam_quick;
        end_beam = end_beam_quick;
        beam_step = beam_step_quick;
        usr_resL1 = 0;
      }
    }

    skip_beam=0;
    ns_beam_count = 0;


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

    /* commented out as fte uses special beams
    if ((sbm !=start_beam) || (ebm !=end_beam)) {
       usr_resS2=sbm;
       usr_resS3=ebm;
    } else {
      usr_resS2=0;
      usr_resS3=0;
    }
    */
  

    do {
       
      /* fte stuff */

      if (usr_resL1 == 1 && ns_beam_count % 3 == 1) {
        store_beam = bmnum;
        bmnum = special_beam;
        scan = -32768;
      }



     if (day_time()==1) {
         start_freq=day_start_freq;
         mpinc=day_mpinc;
         frang=day_frang;

         /* fte specific stuff */
         start_beam_quick = day_start_beam_quick;
         end_beam_quick = day_end_beam_quick;
         special_beam = day_special_beam;

      } else {
         start_freq=night_start_freq;
         mpinc=night_mpinc;
         frang=night_frang;
         /* fte specific stuff */
         start_beam_quick = night_start_beam_quick;
         end_beam_quick = night_end_beam_quick;
         special_beam = night_special_beam;

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

      /* fte stuff */

       if (usr_resL1 == 1 && ns_beam_count % 3 == 1) {
         if (backward) bmnum = store_beam +1;
         else bmnum = store_beam -1;
       }

       ns_beam_count++;


      scan=0;

      if (backward) {
        bmnum--;
        beam_loop=(bmnum>=ebmvl);
      } else {
        bmnum++;
        beam_loop=(bmnum<=ebmvl);
      }
    } while (beam_loop==1);

    /* fte specific stuff */
  
    if (exit_poll==0 && sub_scan_count % (cs_step-1) == (cs_step-2))
       wait_boundary(scan_period); /* wait for three minutes */

    if (start_beam == start_beam_quick)
      sub_scan_count++;

    scan_count++;


  } while (exit_poll==0);

  for (i=0;i<tnum;i++) {
    task_close(tsk[i],year,month,day,hour,minut,sec);
  }

  end_program();

  terminate(sid);



  
} 
 


