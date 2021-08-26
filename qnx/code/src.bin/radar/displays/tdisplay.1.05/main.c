/* terminal.c
   ========= */

/*
 $Log: main.c,v $
 Revision 2.10  2000/11/20 14:32:19  barnes
 Fixed couple of minor bugs.

 Revision 2.9  2000/11/14 20:07:12  barnes
 Added the ability to select which channel is displayed when running in
 stereo mode.

 Revision 2.8  2000/11/01 17:54:06  barnes
 Modified code to use the hardware tables to work out what attenuation to use.

 Revision 2.7  1999/09/15 15:44:41  barnes
 Added mechanism to display control program name.

 Revision 2.6  1999/07/30 20:23:37  barnes
 Minor tidy upos.

 Revision 2.5  1999/03/30 19:20:46  barnes
 Moved task names into a header.

 Revision 2.4  1998/11/09 23:15:05  barnes
 Incorporated rawdata header.

 Revision 2.3  1998/06/12 10:15:54  root
 Changed the command line option that specifies the version of echo_data.

 Revision 2.2  1998/06/11 16:20:40  root
 Use the new radar_name function and changed the task name to 'terminal'.

 Revision 2.1  1998/06/05 11:18:35  root
 Modifications to use the improved message passing mechanism.

 Revision 1.16  1997/11/18 15:42:44  root
 Fixed bug in defining command line options.

 Revision 1.15  1997/11/18 15:31:03  root
 Fixed typo in usage message.

 Revision 1.14  1997/11/18 15:23:04  root
 Extended possible string length for echo_data name.

 Revision 1.13  1997/04/18 09:17:04  root
 Changed registered name.

 Revision 1.12  1997/04/17 17:20:41  root
 Moved under echo_data distribution system.

 Revision 1.11  1997/01/23 22:53:25  root
 Changed exit code to zero.

 Revision 1.10  1996/10/29 20:18:24  root
 Added code to cope with qansi terminal type.

 * Revision 1.9  1996/09/23  15:55:53  root
 * Corrected bug in selecting parameter.
 *
 * Revision 1.8  1996/09/23  14:32:18  root
 * Made display task independant from sd_radar_id.h
 *
 * Revision 1.7  1996/04/16  15:41:36  root
 * Corrected bug in the ACF display
 *
 * Revision 1.6  1996/04/16  15:02:44  root
 * Fixed another bug that slowed down keyboard response.
 *
 * Revision 1.5  1996/04/16  14:36:52  root
 * Corrected problem with slow screen updates over a modem.
 *
 * Revision 1.4  1996/04/16  14:07:05  root
 * Major modifications to the colour bars, scaling and keyboard controls.
 *
 * Revision 1.3  1996/04/12  17:22:31  root
 * Added code to change priority.
 *
 * Revision 1.2  1996/04/12  16:45:17  root
 * Corrected bug that prevented the correct
 * radar parameters being displayed for the raw data.
 *
 * Revision 1.1  1996/04/12  15:13:30  root
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/qnxterm.h>
#include <sys/console.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/dev.h>
#include <sys/proxy.h>
#include <fcntl.h>
#include <unistd.h>
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "radar_name.h"
#include "hardware.h"
#include "display.h"

#include "message.h"
#include "task_msg.h"
#include "decode_msg.h"
#include "echo_util.h"
#include "task_name.h"
#include "hlpstr.h"

#define NAME_TYPE 0x02

extern struct term_state;

struct rawdata *rdt;
struct fitdata *fdt;

extern double pwr_scale;
extern double vel_scale;
extern double wdt_scale;

int term_flag=0;

struct radar_id *rid;
struct radar_hardware *hdt=NULL;
struct radar_site *hdw;


struct task_decode tblk;
struct task_decode *ptr; 

int cpnum=0;
int cpid[4]={0,0,0,0};
int channel=0;

char echo_name[64]={ECHO_DATA};

void main(int argc,char *argv[]) {
  char *envstr;
  char *ndt;
  FILE *fp;
  char text[32];
  char program[32]={"unknown"};
  char data[16];
  char str[32];
  char msg,rmsg;
  pid_t terminal_id,pid,in_proxy;
  int in_fd;
  short int error_flag;
  int range=0;
  int help=0;
  int wait=1;
  int status=0;
  int state;
  int mode=1;
  int changed=1;
  int banner_changed=0;
  int index=0,i,j;
  char *prg_ptr=NULL;
  int extra=0;
  int aflg=0;
  long int sze;
  char echo_pass[4]={FIT_TYPE,RAW_TYPE,NAME_TYPE,0};

  ptr=&tblk;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }
 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);

  envstr=getenv("SD_HARDWARE");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_HARDWARE' must be defined.\n");
    exit(1);
  }
  
  hdt=load_hdw(envstr,rid); 


  if (argc > 1) {
    /* parse the command line arguments */
	int c=1;
	float scale=0.0;
	while (c<argc) {
      if (strcmp(argv[c],"--help")==0) help=1; 
	  else if (strcmp(argv[c],"-a")==0) mode=-1; /* acf */
	  else if (strcmp(argv[c],"-v")==0) mode=1; /* velocity */ 
	  else if (strcmp(argv[c],"-p")==0) mode=0; /* power */
	  else if (strcmp(argv[c],"-w")==0) mode=2; /* spectral width */
	  else if ((strcmp(argv[c],"-m")==0) && (mode==-1)) range=-1;  
	  else if ((scale=atof(argv[c])) !=0) { 
              /* decode the scale parameter */
       if (mode==-1) range=(int) scale;
	   if (mode==1) vel_scale=scale;
	   if (mode==0) pwr_scale=scale;
	   if (mode==2) wdt_scale=scale;
	  } else if (strcmp(argv[c],"-ec")==0) {
        strncpy(echo_name,argv[c+1],64); /* use this version of echo_data */
        c++;
      }
	  c++;
	}
  }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  if (pwr_scale<1.0) pwr_scale=1.0;
  if (vel_scale<10.0) vel_scale=10.0;
  if (wdt_scale<1.0) wdt_scale=1.0;
  if (range<-1) range=-1;
  if (range>MAX_RANGE) range=MAX_RANGE;

  /* set up the QNX terminal functions */

  if (echo_register(TASK_NAME,echo_name,echo_pass,3) !=0) {
     fprintf(stderr,"Could not register with echo_data task.\n");
     exit(1);
   }
  
  if (term_load() == -1) {
    fprintf(stderr,"term_load error, exiting now. \n");
    exit(-1);
  }
  
  if ((strcmp(term_state.name,"qnx")==0) ||
      (strcmp(term_state.name,"qansi")==0)) term_flag=0;
     else term_flag=1;
 
  in_fd=STDIN_FILENO;
  
  in_proxy=qnx_proxy_attach(0,0,0,-1); /* create the keyboard proxy */
  
  dev_mode(in_fd,0,_DEV_MODES); /* set up the keyboard proxy */
  dev_state(in_fd,0, _DEV_EVENT_INPUT);
  dev_arm(in_fd,in_proxy,_DEV_EVENT_INPUT);
  
  term_clear(TERM_CLS_SCR);
  if (mode <0) draw_acf_banner(echo_name,range); /* show that we are running */
    else draw_banner(echo_name,mode); 
  if (mode <0) draw_labels("range:");
    else draw_labels("scale:");

 

  while (wait) {

  /* scheduling */

    pid=Receive(0,&msg,sizeof(char));
    if (pid !=in_proxy) {
      rmsg=decode_msg(pid,msg,ptr);
      Reply(pid,&rmsg,sizeof(char));
      if (msg==TASK_QUIT) {
        wait=0;
        break; 
      } else if (msg==TASK_DATA) {
       
        for (i=0;i<ptr->block.num;i++) {
          switch (ptr->block.data[i].type) {
            case FIT_TYPE: /* display fitted data */
              fdt=(struct fitdata *) 
                         (ptr->block.ptr[ptr->block.data[i].index]);
	      if (mode >=0) {
		      if (changed==1) {
		        term_clear(TERM_CLS_SCR);
			    draw_banner(echo_name,mode);
		        draw_frame(fdt->prms.NRANG);
                draw_labels("scale:");
                if (index !=0) {
                  term_type(1,66,str,0,TERM_WHITE | TERM_HILIGHT);
                  term_flush();
                }              
	      }

	      for (j=0;j<cpnum;j++) {
		if (fdt->prms.CP==cpid[j]) break;
	      }
              if (cpnum==0) {
                cpid[0]=fdt->prms.CP;
                cpnum=1;
              } else if (j==cpnum) {
                if (cpnum==2) cpnum=0;
                cpid[cpnum]=fdt->prms.CP;
                cpnum++;
	      }
              if (fdt->prms.CP !=cpid[channel]) continue;

              hdw=get_site(hdt,fdt->prms.YEAR,
                               fdt->prms.MONTH,
                               fdt->prms.DAY,
                               fdt->prms.HOUR,
                               fdt->prms.MINUT,
                               fdt->prms.SEC,
                               fdt->prms.ST_ID);
              if (hdw->max_atten>3) aflg=1;

              if (cpnum>1) draw_channel();


	      if (extra !=0) 
                      plot_param(rid,&fdt->prms,text,data,prg_ptr,aflg);
              else plot_param(rid,&fdt->prms,NULL,NULL,prg_ptr,aflg);
              if (mode==0) plot_power(fdt);
		      else if (mode==1) plot_velocity(fdt);
		      else plot_width(fdt);
		      changed=0;
            }
  	        break;
	      case RAW_TYPE : /* display the ACF data */	
            rdt=(struct rawdata *) 
                      (ptr->block.ptr[ptr->block.data[i].index]);
            if (mode == -1) {
		      if (rdt->PARMS.NRANG < range) {
		        range=rdt->PARMS.NRANG;
			    changed=1;
		      }
		      if (changed==1) {
		        term_clear(TERM_CLS_SCR);
			    draw_acf_banner(echo_name,range);
		        draw_axis(rdt,range);
                draw_labels("range:");
                if (index !=0) {
                  term_type(1,66,str,0,TERM_WHITE | TERM_HILIGHT);
                  term_flush();
                }              
	      }

              for (j=0;j<cpnum;j++) {
		if (rdt->PARMS.CP==cpid[j]) break;
	      }
              if (cpnum==0) {
                cpid[0]=rdt->PARMS.CP;
                cpnum=1;
              } else if (j==cpnum) {
                if (cpnum==2) cpnum=0;
                cpid[cpnum]=rdt->PARMS.CP;
                cpnum++;
	      }
              if (rdt->PARMS.CP !=cpid[channel]) continue;

              hdw=get_site(hdt,rdt->PARMS.YEAR,
                               rdt->PARMS.MONTH,
                               rdt->PARMS.DAY,
                               rdt->PARMS.HOUR,
                               rdt->PARMS.MINUT,
                               rdt->PARMS.SEC,
                               rdt->PARMS.ST_ID);
              if (hdw->max_atten>3) aflg=1;

              if (cpnum>1) draw_channel();

	      if (extra !=0) 
                        plot_param(rid,&rdt->PARMS,text,data,prg_ptr,aflg);
              else plot_param(rid,&rdt->PARMS,NULL,NULL,prg_ptr,aflg);
              plot_acf(rdt,range);
		      changed=0;
             
            }
            break; 	
  	    case NAME_TYPE: /* add the program name */
              ndt=(char *) (ptr->block.ptr[ptr->block.data[i].index]);
              strcpy(program,ndt);
              prg_ptr=program;  	 	   	 
              break;
	    default :
	        break;
	      }
        }
        if (ptr->store !=NULL) free (ptr->store);
      }
	} else {
	   /* the keyboard has changed */
	   int old_mode=0;
       int new_scale=-1;
	   state=dev_state(in_fd,0,_DEV_EVENT_INPUT);
	   if (state & _DEV_EVENT_INPUT) {
		 char byte;		 
		 read(in_fd,&byte,1);/* what key was pressed */
		 if ((byte==3) || (byte=='q') || (byte=='Q')) {
            term_clear(TERM_CLS_SCR);
            term_restore();
            fprintf(stderr,"Terminal has been stopped.\n");
            exit(0);
         }
        
         if ((index < 4) && (byte>='0') && (byte<='9')) {
           str[index]=byte;
           term_type(1,66+index,&byte,1,TERM_WHITE | TERM_HILIGHT);
           term_flush();
           index++;
           str[index]=0;
         }
           
         if ((byte==127) && (index >0)) {
           index--;
           str[index]=0;
           term_type(1,66+index," ",1,TERM_WHITE | TERM_HILIGHT);
           term_flush();
         }
          
         if ((byte==13) && (index !=0)) {
            str[index]=0;
            new_scale=atoi(str);
            index=0;
            term_type(1,66,"     ",0,TERM_WHITE | TERM_HILIGHT);
            term_flush();
         }              

         old_mode=mode;
         if (((byte=='c') || (byte=='C')) && (cpnum>1)) {
             channel=!channel;
             draw_channel();
	 }
         if ((byte=='v') || (byte=='V')) mode=1;
         if ((byte=='w') || (byte=='W')) mode=2;
         if ((byte=='p') || (byte=='P')) mode=0;
         if ((byte=='a') || (byte=='A')) mode=-1;		
		 if (byte==164) mode--;
		 if (byte==166) mode++;	
		 if (mode<-1) mode=-1;
		 if (mode>2) mode=2;  
		 if (mode !=old_mode) {
            changed=1;
            banner_changed=1;
         }

		 if (mode==-1) {
		   int old_range;
		   old_range=range;
           if (new_scale != -1) range=new_scale;
		   if (byte==169) range--;
		   if (byte==161) range++;
           if ((byte=='m') || (byte=='M')) range=-1;
		   if (range<-1) range=-1;
		   if (range>MAX_RANGE) range=MAX_RANGE;
           if ((banner_changed !=0) || (old_range !=range)) {
		     draw_acf_banner(echo_name,range);
             draw_labels("range:");
		     changed=1;
             banner_changed=0;
           }
		 } else if (mode==0) {
		   float old_scale;
		   old_scale=pwr_scale;		
           if (new_scale != -1) pwr_scale=new_scale; 
		   if (byte==169) pwr_scale-=1.0;
		   if (byte==161) pwr_scale+=1.0;
		   if (pwr_scale<1.0) pwr_scale=1.0;
		   if ((banner_changed !=0) || (old_scale !=pwr_scale)) {
             draw_banner(echo_name,mode);
             draw_labels("scale:");
		     changed=1;
             banner_changed=0;
           }
		 } else if (mode==1) {
		   float old_scale;
		   old_scale=vel_scale;
           if (new_scale !=-1) vel_scale=new_scale;
		   if (byte==169) vel_scale-=10.0;
		   if (byte==161) vel_scale+=10.0;
		   if (vel_scale<10.0) vel_scale=10.0;
		   if ((banner_changed !=0) || (old_scale !=vel_scale)) {
             draw_banner(echo_name,mode);
             draw_labels("scale:");
		     changed=1;
             banner_changed=0;
           }
		 } else if (mode==2) {
		   float old_scale;
		   old_scale=wdt_scale;
           if (new_scale !=-1) wdt_scale=new_scale;
		   if (byte==169) wdt_scale-=1.0;
		   if (byte==161) wdt_scale+=1.0;
		   if (wdt_scale<1.0) wdt_scale=1.0;
		   if ((banner_changed !=0) || (old_scale !=wdt_scale)) {
             draw_banner(echo_name,mode);
             draw_labels("scale:");
		     changed=1;
             banner_changed=0;
           }
		 }
   		dev_arm(in_fd,in_proxy,_DEV_EVENT_INPUT); /* reset the proxy */
	   }
	 }
   }     
}
