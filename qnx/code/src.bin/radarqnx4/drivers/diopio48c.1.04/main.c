/* main.c
   ======
   Author: R.J.Barnes & A. Sessai Yukimatu
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
#include <sys/qnxterm.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <sys/name.h>
#include <sys/dev.h>
#include "option.h"
#include "task_name.h"

#include "freq.h"
#include "do_op.h"
#include "tsg.h"
#include "main.h"
#include "reset.h"
#include "config.h"
#include "display.h"
#include "dio_msg.h"
#include "port.h"

#include "hlpstr.h"

/*
 $Log: main.c,v $
 Revision 1.3  2004/06/22 14:56:40  barnes
 Added the majority of Sessai's modifications.

 Revision 1.2  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

*/

struct OptionData opt;

char *frq_name=NULL;
char *cnf_name=NULL;
int dfrq=13000;


struct dio_param param;
extern struct dio_hdw hdw;
unsigned int AGC_stat_array[2][16][16];
unsigned int LOPWR_stat_array[2][16][16];
int stat_number[2][16][16];
char dio_name[32]={TASK_NAME};

int ticker=0;
unsigned char debug=0;
unsigned char silent=0;

struct FreqTable *fptr=NULL;

extern int watch_port;
extern int dio_adr[2];

void get_status(int chn) {

  /* get both status arrays */
  int beam,antenna;
  unsigned int agcstat, lopwrstat;
  beam=param.beam[chn];                                                         
  antenna=param.antenna[chn];          
  
  agcstat = read_agcstatus(chn);
  
  AGC_stat_array[chn][beam][antenna] += agcstat;
  lopwrstat = read_lopwrstatus(chn);
 
  LOPWR_stat_array[chn][beam][antenna] = lopwrstat;
  stat_number[chn][beam][antenna]++;
}

void zero_status() {
  int i,j,k;
  
  /* zero the contents of the status arrays */

  for (i=0;i<16;i++) {
    for (j=0;j<16;j++) {
      for (k=0;k<2;k++) {
  	    AGC_stat_array[k][i][j]=0;
	    LOPWR_stat_array[k][i][j]=0;
	    stat_number[k][i][j]=0;
      }
	} 
  }
}  	 

void send_status(pid_t pid,int full,int clear) {

  /* send the status array back to the main computer */

  struct _mxfer_entry reply_mx[3];
  char msg=DIO_OK;
  int agcstat,lowpwr,thresh,bit;
  int i,j,k;
  int agc_bits[16];
  int lowpwr_bits[16];
 
  agcstat = 0;
  for (i=0; i < 16; ++i) {
    agcstat = 0;
    lowpwr = 0;
    for (j=0; j < 16; ++j) {
	  /* do the agc status */
	  bit = (10 * AGC_stat_array[param.channel][i][j]);
	  thresh = STAT_THRESH*stat_number[param.channel][i][j];
	  bit = (bit > thresh) ? 1 : 0;
	  for (k=0; k < j; ++k) bit = bit << 1;
	  agcstat = agcstat | bit;
	
	  /* now do the lopwr status */
	  bit = (10 * LOPWR_stat_array[param.channel][i][j]);
	  bit = (bit > thresh) ? 1 : 0;
	  for (k=0; k < j; ++k) bit = bit << 1;
	  lowpwr = lowpwr | bit;
	}
    agc_bits[i] = agcstat;
    lowpwr_bits[i] = lowpwr;
  } 

  _setmx(&reply_mx[0],&msg,sizeof(char));    

  if (full) {
    _setmx(&reply_mx[1],agc_bits,16*sizeof(int));
	_setmx(&reply_mx[2],lowpwr_bits,16*sizeof(int));
  } else {
    agcstat = 0;
	lowpwr = 0;
	for (i=0; i< 16; ++i) {
		agcstat |=  agc_bits[i];
		lowpwr |=  lowpwr_bits[i];		
	}
    _setmx(&reply_mx[1],&agcstat,sizeof(int));
	_setmx(&reply_mx[2],&lowpwr,sizeof(int));	
  }
 
  Replymx(pid,3,reply_mx);
  if (clear) zero_status();   
}

void send_fclr(pid_t pid) {

  int status,i;
  int *frq_table[2]={NULL,NULL};
  char reply_msg;  
  
  struct {
    char stereo;
    unsigned char seq[2];
    unsigned int delay[2];
    int frq_num[2];
  } header;

   
  header.frq_num[0]=-1;
  header.frq_num[1]=-1;
  header.seq[0]=0;
  header.seq[1]=0;

  status=(Readmsg(pid,sizeof(char),&header,sizeof(header))!=sizeof(header));

  if (header.stereo==0) { /* mono mode */

    if (status ==0) 
         status=(header.frq_num[0] <=0); /* invalid frequency table*/
    if (status ==0)
	status=((frq_table[0]=malloc(sizeof(int)*header.frq_num[0])) ==NULL);
    if (status ==0)
	    status=(Readmsg(pid,sizeof(char)+sizeof(header),frq_table[0],
	            sizeof(int)*header.frq_num[0]) != 
                sizeof(int)*header.frq_num[0]);
    if (status==0) {
      for (i=0;i<header.frq_num[0];i++) {
        if ((status=put_Hz(param.channel,frq_table[0][i])) !=0) break;
        send_seq_mono(param.channel,header.seq[0],0,NULL); 
      }
    } 
  } else {


    if (status ==0) 
         status=(header.frq_num[0] <=0); /* invalid frequency table*/
    if (status ==0) 
         status=(header.frq_num[1] <=0); /* invalid frequency table*/

    if (status ==0)
	status=((frq_table[0]=malloc(sizeof(int)*header.frq_num[0])) ==NULL);
    if (status ==0)
	status=((frq_table[1]=malloc(sizeof(int)*header.frq_num[1])) ==NULL);

    if (status ==0)
	    status=(Readmsg(pid,sizeof(char)+sizeof(header),frq_table[0],
	            sizeof(int)*header.frq_num[0]) != 
                sizeof(int)*header.frq_num[0]);
    if (status ==0)
	    status=(Readmsg(pid,sizeof(char)+sizeof(header)+
                            sizeof(int)*header.frq_num[0],frq_table[1],
	                    sizeof(int)*header.frq_num[1]) != 
                            sizeof(int)*header.frq_num[1]);

    if (status==0) { 
     
      i=0;
     
      while ((i<header.frq_num[0]) || (i<header.frq_num[1])) {
	if (i<header.frq_num[0]) {        
          if ((status=put_Hz(0,frq_table[0][i])) !=0) break;
         
        }
        if (i<header.frq_num[1]) {        
          if ((status=put_Hz(1,frq_table[1][i])) !=0) break;
     
        }
        if ((i<header.frq_num[0]) && (i<header.frq_num[1])) {
          send_seq_stereo(header.seq,header.delay,NULL,NULL);
        } else if (i<header.frq_num[0]) {
          send_seq_mono(0,header.seq[0],0,NULL); 
        } else if (i<header.frq_num[1]) {
          send_seq_mono(1,header.seq[1],0,NULL); 
        } 
        i++;
      }
      
    
     
    }

  }


  if (frq_table[0] !=NULL) free(frq_table[0]);
  if (frq_table[1] !=NULL) free(frq_table[1]);
	  
  if (status ==0) reply_msg=DIO_OK;
  else {
     reply_msg=DIO_ERR;	
     if (status==LOCAL_FREQ) reply_msg=LOCAL_FREQ;
    }
  Reply(pid,&reply_msg,sizeof(reply_msg));  
}



void valid_id(pid_t pid) {

  /* check for a valid sequence id */

  struct {
    char op_type;
    int length;
  }reply_msg;
  int status =0;
  char seq;
  
  status=(Readmsg(pid,sizeof(char),&seq,sizeof(seq))!=sizeof(seq));
     
  reply_msg.length=get_length(seq);
  reply_msg.op_type=DIO_OK;
  Reply(pid,&reply_msg,sizeof(reply_msg));  
}

void send_freq_mode(pid_t pid) {

  /* send the mode of the frequency synthesizer back to the main computer */

  struct {
    char op_type;
    int mode;
  } reply_msg;
 
  
  reply_msg.mode=get_freq_mode(param.channel);

  param.fstatus[param.channel]=reply_msg.mode;
  show_values();

  reply_msg.op_type=DIO_OK;
  Reply(pid,&reply_msg,sizeof(reply_msg));  
}

void read_beam(pid_t pid) {

  /* read the beam number that dio_driver is using */

  struct {
    char op_type;
	char bmnum;
  } reply_msg;
  
  
  reply_msg.bmnum=param.beam[param.channel];
  reply_msg.op_type=DIO_OK;
  
  Reply(pid,&reply_msg,sizeof(reply_msg));
  
}

void get_tsg_status(pid_t pid) {

  /* read the beam number that dio_driver is using */
 
  struct {
    char op_type;
	char status;
  } reply_msg;
 
  
  reply_msg.status=(param.status !=0);
  reply_msg.op_type=DIO_OK;
  
  Reply(pid,&reply_msg,sizeof(reply_msg));
  
}




void read_filter(pid_t pid) {

  /* read the filter that dio_driver is using */

  struct {
    char op_type;
	char filter;
  } reply_msg;
 
  
  reply_msg.filter=param.filter[param.channel];
  reply_msg.op_type=DIO_OK;
  
  Reply(pid,&reply_msg,sizeof(reply_msg));
  
}

void read_freq(pid_t pid) {

   /* read the frequency that dio_driver is using */

  struct {
    char op_type;
    int freq;
  }reply_msg;
 
  
  reply_msg.freq=param.freq[param.channel];
  reply_msg.op_type=DIO_OK;
  Reply(pid,&reply_msg,sizeof(reply_msg));
}

void read_gain(pid_t pid) {

   /* read the attenuation that dio_driver is using */

  struct {
    char op_type;
	char gain;
  }reply_msg;
 
 
  reply_msg.gain=param.gain[param.channel]; 
  reply_msg.op_type=DIO_OK;
  Reply(pid,&reply_msg,sizeof(reply_msg));
}

void set_beam(pid_t pid) {

  /* set the beam number */

  int status=0;
  char bmnum; 
  
  status=(Readmsg(pid,sizeof(char),&bmnum,sizeof(bmnum))!=sizeof(bmnum));
    
  if (status==0) status=put_beam(param.channel,bmnum);
  if ((status==0) && 
      (param.antenna_mode[param.channel]==ANTENNA_BEAM)) /* set antenna */
     status=put_antenna(param.channel,bmnum);
  if (status !=0) bmnum=DIO_ERR;
  else {
    param.beam[param.channel]=bmnum;
    bmnum=DIO_OK;
	show_values();
  }
  Reply(pid,&bmnum,sizeof(bmnum));
}	 

void set_filter(pid_t pid) {

  /* set the beam number */

  int status=0;
  char filter; 
  
  status=(Readmsg(pid,sizeof(char),&filter,sizeof(filter))!=sizeof(filter));
  if (status==0) status=put_filter(param.channel,(unsigned int) filter); 
  if (status !=0) filter=DIO_ERR;
  else {
    param.filter[param.channel]=filter;
    filter=DIO_OK;
	show_values();
  }
  Reply(pid,&filter,sizeof(filter));
}	 
 
void set_Hz(pid_t pid) {

  /* set the frequency */

  int status=0;
  char reply_msg;
  int freq=0; 
   
  status=(Readmsg(pid,sizeof(char),&freq,sizeof(freq))!=sizeof(freq));
  if (status==0) status=put_Hz(param.channel,freq);
  reply_msg=DIO_OK;
  param.fstatus[param.channel]=0;
  if (status !=0) {
    reply_msg=DIO_ERR;  /* frequency synthesizer is in local mode */
    if (status==LOCAL_FREQ) { 
      reply_msg=LOCAL_FREQ;
      param.fstatus[param.channel]=LOCAL_FREQ;
    } 
  } else param.freq[param.channel]=freq;
  show_values();
  Reply(pid,&reply_msg,sizeof(reply_msg)); 
}

void set_antenna(pid_t pid) {

  /* set the antenna number */

  int status=0;
  char mode; 
  char antenna;
    
  status=(Readmsg(pid,sizeof(char),&mode,sizeof(mode))!=sizeof(mode));
  
  if (status==0) {
    if ( (mode != ANTENNA_BEAM) && 
       (mode !=ANTENNA_AUTO) && (mode !=ANTENNA_FIX) ) status=1;
     
    if (mode == ANTENNA_FIX) { 
	   status=(Readmsg(pid,2*sizeof(char),&antenna,
	   				   sizeof(antenna))!=sizeof(antenna));
	   if (status ==0) status=put_antenna(param.channel,antenna);		   
	}
  }			   
  	 
  if (status !=0) mode=DIO_ERR;
  else {
    param.antenna_mode[param.channel]=mode;
	if (mode==ANTENNA_FIX) param.antenna[param.channel]=antenna;
    mode=DIO_OK;
	show_values();
  }
  Reply(pid,&mode,sizeof(mode));
}

void set_gain(pid_t pid) {

  /* set the attenuation */

  int status=0;
  char gain; 
  
  status=(Readmsg(pid,sizeof(char),&gain,sizeof(gain))!=sizeof(gain));
  if (status==0) status=put_gain(param.channel,gain);
  if (status !=0) gain=DIO_ERR;
  else {
    param.gain[param.channel]=gain;
    gain=DIO_OK;
	show_values();
  }
  Reply(pid,&gain,sizeof(gain));
}	 
  
void set_test_mode(pid_t pid) {

  /* put the BAS box into test mode */

  int status=0;
  char mode; 
  
  status=(Readmsg(pid,sizeof(char),&mode,sizeof(mode))!=sizeof(mode));
  if (status==0) status=put_test_mode(param.channel,mode);
  if (status !=0) mode=DIO_ERR;
  else {
    param.test_mode[param.channel]=mode;
    mode=DIO_OK;
	show_values();
  }
  Reply(pid,&mode,sizeof(mode));
}	   

void set_channel(pid_t pid) {

  /* set the channel on the interface box */

  int status=0;
  char channel; 
  
  status=(Readmsg(pid,sizeof(char),&channel,sizeof(channel))!=sizeof(channel));
  if (param.stereo==1) {
    param.channel=channel;
    channel=DIO_OK;
    show_values();
  } else channel=DIO_ERR;
  Reply(pid,&channel,sizeof(channel));
}	   

void read_channel(pid_t pid) {

   /* read the attenuation that dio_driver is using */

  struct {
    char op_type;
	char channel;
  } reply_msg;
  if (param.stereo==1) reply_msg.channel=param.channel; 
  else reply_msg.channel='m';
  reply_msg.op_type=DIO_OK;
  Reply(pid,&reply_msg,sizeof(reply_msg));
}
   
void main(int argc,char *argv[]) {
  int arg;
 
  char *watch_port_str=NULL;
  FILE *fp;
  char msg;
  char *port=NULL;
  pid_t dio_id,pid;
 
 
  int wait=1;
  int dump=0;
  int help=0;

  param.stereo=0;      

  if(setprio(0,DIO_PRIO)==(-1)){
	 perror("Unable to set priority");
	 exit(-1);
  }

  default_config();
  param.channel=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"db",'x',&debug);
  OptionAdd(&opt,"q",'x',&silent);
  OptionAdd(&opt,"s",'x',&param.stereo);  
  OptionAdd(&opt,"c",'x',&ticker);
  OptionAdd(&opt,"d",'x',&dump);
  OptionAdd(&opt,"wd",'i',&watch_port_str);
  OptionAdd(&opt,"p",'t',&port);
  OptionAdd(&opt,"cf",'t',&cnf_name);
  OptionAdd(&opt,"ft",'t',&frq_name);
    
  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (arg<argc) strcpy(dio_name,argv[arg]);
  if (watch_port_str !=NULL) sscanf(watch_port_str,"%x",&watch_port);

  if (port !=NULL) {
    char *ptmp;
    int i=0;
    ptmp=strtok(port,",");
    while ((ptmp !=NULL) && (i<2)) {
      sscanf(ptmp,"%x",&dio_adr[i]);
      ptmp=strtok(NULL,",");
      i++;
    }
  }   
   
  if (frq_name !=NULL) {
    fp=fopen(frq_name,"r");
    if (fp==NULL) {
      fprintf(stderr,"Could not load frequency table.\n");
      exit(1);
    }
    fptr=FreqLoadTable(fp);
    dfrq=fptr->dfrq;
    fclose(fp);
  }

  if (cnf_name !=NULL) {
    fp=fopen(cnf_name,"r");
    if (fp==NULL) {
      fprintf(stderr,"Could not load hardware configuration file.\n");
      exit(1);
    }
    strncpy(hdw.name,cnf_name,63);    
    if (load_config(fp) !=0) {
      fclose(fp);
      fprintf(stderr,"Could not load hardware configuration file.\n");
      exit(1);
    }
    fclose(fp);
  }

  if (dump==1) {
     fprintf(stdout,"DIO port addresses=");
     if (dio_adr[0] !=0) fprintf(stdout,"0x%.3x,",dio_adr[0]);
     else fprintf(stdout,"unset,");
     if (dio_adr[1] !=0) fprintf(stdout,"0x%.3x\n",dio_adr[1]);
     else fprintf(stdout,"unset\n");    
     if (watch_port !=0) fprintf(stdout,"Watchdog port=%.3x\n",watch_port); 
     print_config(stdout,param.stereo);
     exit(0);
  }
  
  if ( (dio_id=qnx_name_attach(0,dio_name) ) == -1) { 
    perror("Unable to attach dio_driver task");
    exit(1);
  }

  if (reset(param.stereo) !=0) { 
    fprintf(stderr,"Error setting dio_driver, exiting now. \n");
    exit(-1);
  }
  
  /* set the default parameters */

   
  param.beam[0]=8;
  param.beam[1]=8;
  param.freq[0]=dfrq;
  param.freq[1]=dfrq;
  param.gain[0]=3;
  param.gain[1]=3; 
  param.filter[0]=0;
  param.filter[1]=0;
  param.antenna_mode[0]=ANTENNA_AUTO;
  param.antenna_mode[1]=ANTENNA_AUTO;
  param.seq_id[0]=-1;
  param.seq_id[1]=-1;
  param.fstatus[0]=0;
  param.fstatus[1]=0;


  init_tsg();
  zero_status();
  put_beam(0,8);
  if (put_Hz(0,dfrq)==LOCAL_FREQ) param.fstatus[0]=LOCAL_FREQ;
  put_gain(0,3);   
  put_filter(0,0);
  put_test_mode(0,0);

  if (param.stereo==1) {
    put_beam(1,8);
    if (put_Hz(1,dfrq)==LOCAL_FREQ) param.fstatus[1]=LOCAL_FREQ;
    put_gain(1,3);   
    put_filter(1,0);
  }
  

  if (silent==0) {
    /* set up the terminal for display */
    if (term_load() == -1) {
      fprintf(stderr,"term_load error, exiting now. \n");
      exit(-1);
    }
  }

  if (silent==0) {   
    term_clear(TERM_CLS_SCR);
    term_box_on();
    draw_display(ticker);
    show_values();
    get_status(param.channel);
    display_status();
  }
  /* wait for a message from the main computer */

  while (wait) {
    pid=Receive(0,&msg,sizeof(msg));

    /* reset the sequence counter as we are no longer sending out
       a timing sequence */

    if (msg !=SEND_TSG) param.seq_no=0;
    if (silent==0) {
      if (msg !=SEND_TSG) display_status();
      if (ticker !=0)  show_command(msg);
    }
    switch (msg) {
	  case SET_GAIN:
	    set_gain(pid); 
		break;
	  case SET_FREQ:
	    set_Hz(pid);
		break;
      case SET_BEAM:
	    set_beam(pid);     
		break;
      case SET_FILTER:
	    set_filter(pid);     
		break;
      case READ_GAIN:
	    read_gain(pid);
		break;
      case READ_FREQ:
	    read_freq(pid);
		break;
      case READ_BEAM:
	    read_beam(pid);
		break;
      case READ_FILTER:
	    read_filter(pid);
		break;
      case RESET_XT:
	if (reset(param.stereo) !=0) msg=DIO_ERR;
            else {
	  msg=DIO_OK;
          param.beam[0]=8;
          param.beam[1]=8;
          param.freq[0]=dfrq;
          param.freq[1]=dfrq;
          param.gain[0]=3;
          param.gain[1]=3; 
          param.filter[0]=0;
          param.filter[1]=0;
          param.fstatus[0]=0;
          param.fstatus[1]=0;

          param.seq_id[0]=-1;
          param.seq_id[1]=-1;

          param.antenna_mode[0]=ANTENNA_AUTO;
          param.antenna_mode[1]=ANTENNA_AUTO;

          param.channel=0;
		  zero_status();
		  zero_tsg();

          put_beam(0,8);
          if (put_Hz(0,dfrq)==LOCAL_FREQ) param.fstatus[0]=LOCAL_FREQ;
          put_gain(0,3);   
          put_filter(0,0);

          if (param.stereo==1) {
            put_beam(1,8);
            if (put_Hz(1,dfrq)==LOCAL_FREQ) param.fstatus[1]=LOCAL_FREQ;;
            put_gain(1,3);   
            put_filter(1,0);
          }
	  if (silent==0) show_values();   
	} 
        Reply(pid,&msg,sizeof(msg));
        break; 
      case SET_ANTENNA:
        set_antenna(pid);
	break;
      case TEST_MODE:
	set_test_mode(pid);
        break;	
      case SET_CHANNEL:
        set_channel(pid);
        break;
      case READ_CHANNEL:
        read_channel(pid);
        break;
	  case SET_TSG:
	    download(pid); 
	    break;
	  case SEND_TSG:
	    if ((param.status=send(pid))==0) param.seq_no++;
	    if (silent==0) display_status();
        break;
	  case GET_TSTATUS:
	    get_tsg_status(pid);
		break;	
	 case VALID_ID:
	    valid_id(pid);
		break;	
	 case GET_STATUS_NOFULL_NOCLR:
	    send_status(pid,0,0);
		if (silent==0) display_status();
		break;
	 case GET_STATUS_FULL_NOCLR:
	    send_status(pid,1,0);
		if (silent==0) display_status();
		break;
	 case GET_STATUS_NOFULL_CLR:
	    send_status(pid,0,1);
		if (silent==0) display_status();
		break;	
	 case GET_STATUS_FULL_CLR:
	    send_status(pid,1,1);
		if (silent==0) display_status();
		break;
	  case GET_FREQ_MODE:
	    send_freq_mode(pid);
		break;	

      case SEND_FCLR:
        send_fclr(pid);
        break;	 	   
	  default :
	    msg=UNKNOWN_TYPE;
		Reply(pid,&msg,sizeof(msg));		
		break;		  
    }
  }
}

