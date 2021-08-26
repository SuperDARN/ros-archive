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
#include <string.h>
#include <stdlib.h>
#include <sys/qnxterm.h>
#include <sys/console.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/dev.h>
#include <sys/proxy.h>
#include <fcntl.h>
#include <unistd.h>
#include "rtypes.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitdata.h"
#include "radar.h"
#include "display.h"

#include "rmsg.h"
#include "rmsgrcv.h"
#include "echo.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: main.c,v $
 Revision 2.17  2007/08/09 13:31:30  code
 Fixed bug in detecting qnx terminal types.

 Revision 2.16  2006/03/29 13:33:29  barnes
 Modification to fix problems with Stereo.

 Revision 2.15  2004/06/22 00:16:09  barnes
 Added missing header.

 Revision 2.14  2004/05/11 22:07:29  barnes
 Modifications to deal with library changes.

 Revision 2.13  2002/04/08 17:15:27  barnes
 Fixed stereo bugs.

 Revision 2.12  2001/06/27 20:55:15  barnes
 Added license tag

 Revision 2.11  2001/01/29 18:12:05  barnes
 Added Author Name

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

extern struct term_state;

struct OptionData opt;

struct DMsg {
  int tag;
  struct RadarParm *prm;
  struct RawData *rdt;
  struct FitData *fdt;
  char *ndt;
};


int dnum=0;
int dptr=0;
struct DMsg dmsg[32];

extern double pwr_scale;
extern double vel_scale;
extern double wdt_scale;

int term_flag=0;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct RMsgBlock rblk;
char *store=NULL; 


int cpnum=0;
int cpid[4]={0,0,0,0};
int channel=0;

char *echoname;
char *echodname={ECHO_DATA};


void main(int argc,char *argv[]) {
 
  char *envstr;
  FILE *fp;
  char text[32];
  char data[16];
  char str[32];
  char msg,rmsg;
  pid_t pid,in_proxy;
  int in_fd;

  int arg;
  float scale;
  int range=0;
  int help=0;
  int acfflg=0;
  int pwrflg=0;
  int velflg=0;
  int wdtflg=0;
  int mxpwrflg=0;
  int wait=1;
  int state;
  int mode=1;
  int changed=1;
  int stflg=0;
  int banner_changed=0;
  int index=0,i,j;
  char program[64]={"unknown"};
  char *prg_ptr=NULL;
  int extra=0;
  int aflg=0;
  int roff=0;
  int max;
  char echo_pass[8]={PRM_TYPE,FIT_TYPE,RAW_TYPE,NME_TYPE,0};

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"a",'x',&acfflg);
  OptionAdd(&opt,"v",'x',&velflg);
  OptionAdd(&opt,"p",'x',&pwrflg);
  OptionAdd(&opt,"w",'x',&wdtflg);
  OptionAdd(&opt,"m",'x',&mxpwrflg);
  OptionAdd(&opt,"ec",'t',&echoname);
  OptionAdd(&opt,"o",'i',&roff);
  arg=OptionProcess(1,argc,argv,&opt,NULL);  

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);
 
  if (acfflg) mode=-1;
  if (velflg) mode=1;
  if (pwrflg) mode=0;
  if (wdtflg) mode=2;
  if (mxpwrflg) range=-1;

  if (arg<argc) { 
    scale=atof(argv[arg]);
    if (mode==1) vel_scale=scale;
    if (mode==0) pwr_scale=scale;
    if (mode==2) wdt_scale=scale;
  }

  if (echoname==NULL) echoname=echodname;

  if (pwr_scale<1.0) pwr_scale=1.0;
  if (vel_scale<10.0) vel_scale=10.0;
  if (wdt_scale<1.0) wdt_scale=1.0;
  if (range<-1) range=-1;
  if (range>MAX_RANGE) range=MAX_RANGE;

  /* set up the QNX terminal functions */

  if (EchoRegister(TASK_NAME,echoname,echo_pass,4) !=0) {
    fprintf(stderr,"Could not register with echo_data task.\n");
    exit(1);
  }
  
  if (term_load() == -1) {
    fprintf(stderr,"term_load error, exiting now. \n");
    exit(-1);
  }

  if ((strcmp(term_state.name,"qnx",3)==0) ||
      (strcmp(term_state.name,"qansi")==0)) term_flag=0;
  else term_flag=1;
 
  in_fd=STDIN_FILENO;
  
  in_proxy=qnx_proxy_attach(0,0,0,-1); /* create the keyboard proxy */
  
  dev_mode(in_fd,0,_DEV_MODES); /* set up the keyboard proxy */
  dev_state(in_fd,0, _DEV_EVENT_INPUT);
  dev_arm(in_fd,in_proxy,_DEV_EVENT_INPUT);
  
  term_clear(TERM_CLS_SCR);
  if (mode <0) draw_acf_banner(echoname,range); /* show that we are running */
    else draw_banner(echoname,mode); 
  if (mode <0) draw_labels("range:");
    else draw_labels("scale:");

  while (wait) {

    /* scheduling */

    pid=Receive(0,&msg,sizeof(char));
    if (pid !=in_proxy) {
      rmsg=TASK_OK;
      switch (msg) {
      case TASK_OPEN:
	break;
      case TASK_CLOSE:
        break;
      case TASK_QUIT:
        wait=0;
        break;
      case TASK_RESET:
        break;
      case TASK_DATA:
        rmsg=RMsgRcvDecodeData(pid,&rblk,&store);
      default:
        break;
      }
      Reply(pid,&rmsg,sizeof(char));
      if (msg==TASK_DATA) { 
        dnum=0;
        for (i=0;i<rblk.num;i++) {
          for (dptr=0;dptr<dnum;dptr++) 
            if (dmsg[dptr].tag==rblk.data[i].tag) break;
          if (dptr==dnum) {
            dmsg[dptr].tag=rblk.data[i].tag;
            dmsg[dptr].prm=NULL;
            dmsg[dptr].rdt=NULL;
            dmsg[dptr].fdt=NULL;
            dmsg[dptr].ndt=NULL;
            dnum++;
	  }
          switch (rblk.data[i].type) {
	  case PRM_TYPE:
            dmsg[dptr].prm=(struct RadarParm *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case RAW_TYPE:
            dmsg[dptr].rdt=(struct RawData *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case FIT_TYPE:
            dmsg[dptr].fdt=(struct FitData *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
	  case NME_TYPE:
            dmsg[dptr].ndt=(char *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          default:
            break;
	  }
	} 

        for (i=0;i<dnum;i++) {
	      if (dmsg[i].prm !=NULL) {
            for (j=0;j<cpnum;j++) if (dmsg[i].prm->cp==cpid[j]) break;
            if (cpnum==0) {
              cpid[0]=dmsg[i].prm->cp;
              cpnum=1;
            } else if (j==cpnum) {
              if (cpnum==2) cpnum=0;
              cpid[cpnum]=dmsg[i].prm->cp;
              cpnum++;
	        }
            if (dmsg[i].prm->channel !=0) {
              stflg=1;
              if ((channel==0) && (dmsg[i].prm->channel==2)) continue;
              if ((channel==1) && (dmsg[i].prm->channel==1)) continue;
  	        }
            
            radar=RadarGetRadar(network,dmsg[i].prm->stid);
            site=RadarYMDHMSGetSite(radar,dmsg[i].prm->time.yr,
				    dmsg[i].prm->time.mo,
		                    dmsg[i].prm->time.dy,dmsg[i].prm->time.hr,
                                    dmsg[i].prm->time.mt,dmsg[i].prm->time.sc);
            if (site->maxatten>3) aflg=1;

            if (mode >=0) {
	        if (changed==1) {
	          term_clear(TERM_CLS_SCR);
  		      draw_banner(echoname,mode);
              max=dmsg[i].prm->nrang;
              if (max>75) max=75;
		      draw_frame(roff,max);
              draw_labels("scale:");
              if (index !=0) {
                 term_type(1,66,str,0,TERM_WHITE | TERM_HILIGHT);
                 term_flush();
               }              
	        }
              changed=0;
	    } else {
              if (dmsg[i].prm->nrang < range) {
	            range=dmsg[i].prm->nrang;
		        changed=1;
	          }
              if (changed==1) {
	            term_clear(TERM_CLS_SCR);
		        draw_acf_banner(echoname,range);
		        draw_axis(dmsg[i].prm,range);
                draw_labels("range:");
                if (index !=0) {
                  term_type(1,66,str,0,TERM_WHITE | TERM_HILIGHT);
                  term_flush();
                }              
	      }
              changed=0;
	    }
            if (stflg) draw_channel();
            if (extra !=0) 
              plot_param(network,dmsg[i].prm,text,data,prg_ptr,aflg);
            else plot_param(network,dmsg[i].prm,NULL,NULL,prg_ptr,aflg);
	  }
          if ((mode>=0) && (dmsg[i].fdt !=NULL)) {
            if (mode==0) plot_power(dmsg[i].prm,dmsg[i].fdt,roff);
	    else if (mode==1) plot_velocity(dmsg[i].prm,dmsg[i].fdt,roff);
	    else plot_width(dmsg[i].prm,dmsg[i].fdt,roff);
	  }
          if ((mode==-1) &&  (dmsg[i].rdt !=NULL)) 
	      plot_acf(dmsg[i].prm,dmsg[i].rdt,range,roff);
	  if (dmsg[i].ndt !=NULL) {
            strcpy(program,dmsg[i].ndt);
            prg_ptr=program;  
	  }
	}
        if (store !=NULL) free (store);
        store=NULL;
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
        if (((byte=='c') || (byte=='C')) && (stflg !=0)) {
          channel=!channel;
          draw_channel();
	}

        if (byte=='.') {
           roff+=5;
           changed=1;
           if (roff>300) roff=300;
	}
        if (byte==',') {
            roff-=5;
            changed=1;
            if (roff<0) roff=0;
	}


        if (byte==']') {
           roff+=75;
           changed=1;
           if (roff>300) roff=300;
	}
        if (byte=='[') {
            roff-=75;
            changed=1;
            if (roff<0) roff=0;
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
	    draw_acf_banner(echoname,range);
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
            draw_banner(echoname,mode);
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
            draw_banner(echoname,mode);
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
            draw_banner(echoname,mode);
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
