/* main.c
   ======
   Author: R.J.Barnes
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <signal.h>
#include <time.h>
#include "rtypes.h"
#include "option.h"
#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"
#include "dt2828.h"
#include "version.h"
#include "task_name.h"
#include "hlpstr.h"

/*
 $Log: main.c,v $
 Revision 1.3  2004/06/22 12:33:00  barnes
 QNX4 code audit.

 Revision 1.2  2004/06/04 21:57:38  barnes
 Removed the multiplying factor or bytes*channels to be consistent with
 the DATEL driver - the multiplying factor will now go into the A/D interface
 library.

 Revision 1.1  2004/05/10 14:34:00  barnes
 Initial revision

*/


#define DRV_STAT_UNKNOWN 0
#define DRV_ERROR 'E'
#define GET_PROXY_PID 'p'
#define GET_BUF_NUM 'n'
#define GET_BUF_SIZE 's'
#define GET_BUF_ADR 'a'
#define GET_SCAN_STATUS 'S'
#define DO_SCAN 'd'

#define TIMEOUT 10

#define SCAN_OK 0
#define SCAN_ERROR 'F'

#define DRIVER_PRIORITY 15

#define DMA_BUF_NUM 2
#define DMA_BUF_MAX 8
#define DMA_BUF_SIZE 32768L

struct OptionData opt;


dma_t dma_buf[DMA_BUF_MAX];
int ad_irq=AD_IRQ;
int ad_port=AD_PORT;
int ad_dma=1;

char driver_name[32]={TASK_NAME};

union dt_adcsr_str dt_adcsr;          /* dt adcsr register */
union dt_chancsr_str dt_chancsr;      /* dt chancsr register */
union dt_dacsr_str dt_dacsr;          /* dt dacsr register */
union dt_supcsr_str dt_supcsr;        /* dt supcsr register */
union dt_tmrctr_str dt_tmrctr;        /* dt tmrctr register */

int dt_hint_id;			      /* hardware interrupt id */
pid_t dt_proxy;			      /* proxy */


void a_d_signal_handler(int signo) {
}

void do_scan(int buffer,int bytes,int mode,int channel,int channels) {

  /* start off the A/D DMA transfer */

  dma_init(dma_buf[buffer].phys_addr,channel,bytes,DMA_WRITE_TRANSFER);
  if (mode==0) dt_etrig_scan(channels);
    else dt_strig_scan(channels);
}

int wait_on_scan() {
  pid_t reply_id;
  timer_t timer_id=-1;
  struct itimerspec timer;
  struct sigevent event;

  void (*prev_handler)(int);
  
  /* now wait for the interrupt to tell us that the scan is complete */
  
  prev_handler=signal(SIGUSR1,a_d_signal_handler);
  
  if (prev_handler==SIG_ERR) return SCAN_ERROR; 

  timer.it_interval.tv_sec=0L;
  timer.it_interval.tv_nsec=0L;
  timer.it_value.tv_sec=(long int) TIMEOUT;
  timer.it_value.tv_nsec=0L;
  event.sigev_signo=SIGUSR1;

  if ( ((timer_id=timer_create(CLOCK_REALTIME,&event))==-1) ||
        (timer_settime(timer_id,0,&timer,NULL)==-1)) {
    signal(SIGUSR1,prev_handler);  
    return SCAN_ERROR;  
  }

  reply_id=Receive(dt_proxy,0,0); 
  if (timer_id !=-1) {
    timer_delete(timer_id);
    signal(SIGUSR1,prev_handler);
  } 

  if (reply_id == -1) return SCAN_ERROR;

  /* check for error status */
  
  dt_read_register(ad_port+ADCSR_PORT,&dt_adcsr);

  if(dt_adcsr.bit.a_derr) return SCAN_ERROR;
  return SCAN_OK;
}

void main(int argc,char *argv[]) {
  int arg;
  char code;
  unsigned  char debug=0;
  char *ad_port_str=NULL;
  pid_t driver_id,pid;
  int dma_buf_num=DMA_BUF_NUM;
  int size;
  int proxy_id;
  int loop;
  int status;
  int scan_status=SCAN_OK;
  int buffer;
  int mode;
  int bytes=0;
  int  channels=0;

  struct _mxfer_entry msg[4];

  int16 *ptr;
  int16 rval;

  float nscl=10.0;
  int n;

  int help=0;



  /* set priority */
 
  if(setprio(0,DRIVER_PRIORITY) == -1) {
    perror("unable to set priority\n");
	exit(1);
  }

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"db",'x',&debug);
  OptionAdd(&opt,"i",'i',&ad_irq);
  OptionAdd(&opt,"d",'i',&ad_dma);
  OptionAdd(&opt,"p",'i',&ad_port_str);
  OptionAdd(&opt,"n",'i',&dma_buf_num);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (ad_port_str !=NULL) sscanf(ad_port_str,"%x",&ad_port);

  if (arg<argc) strcpy(driver_name,argv[arg]);

  if (dma_buf_num<2) dma_buf_num=2;
  else if (dma_buf_num>DMA_BUF_MAX) dma_buf_num=DMA_BUF_MAX;
 
  /* register name */
  
  if ( (driver_id=qnx_name_attach(0,driver_name) ) == -1) { 
    perror("Unable to attach A/D driver");
    exit(1);
  }

  /* set up proxy and interrupt handler */

  if (debug==0) {
    if (dt_int(ad_irq) !=0) {
      perror("Unable to set up interrupt handler");
      exit(1);
    }
  }

  /* claim DMA buffers */

  for (loop=0;loop<dma_buf_num;loop++) {
    if (alloc_dmabuf(&dma_buf[loop],DMA_BUF_SIZE) !=0) {
      perror("Unable to allocate DMA buffer");
      exit(1);
    }
  }  

  if (debug !=0) {
     fprintf(stderr,"dt2828 version: %d:%d debug\n",
	     MAJOR_VERSION,MINOR_VERSION);
  } else {
     fprintf(stderr,"dt2828 version: %d:%d\n",MAJOR_VERSION,MINOR_VERSION);
  }
  fprintf(stderr,"Registered under the name: %s\n",driver_name);
  fprintf(stderr,"Number of DMA Buffers:%d\n",dma_buf_num);
  fprintf(stderr,"irq=%.2d\n",ad_irq);
  fprintf(stderr,"base io port=0x%x\n",ad_port);
  fprintf(stderr,"DMA channel=%d\n",ad_dma+4);
  fprintf(stderr,"Proxy PID=%d\n",dt_proxy);


  if (debug==0) {
    dt_reset();
    dt_set_clock(CLOCK_FREQ);
  }

  while (1) {
    pid=Receive(0,&code,sizeof(code));

    /* if we receive the interrupt proxy here then something has gone
     * wrong */

    if (pid==dt_proxy) {
      fprintf(stderr,"proxy fault \n");
    } else {   
      switch (code) {

      case GET_PROXY_PID :
        proxy_id=(long int) dt_proxy;
        status=0;
        _setmx(&msg[0],&status,sizeof(status)); 
        _setmx(&msg[1],&proxy_id,sizeof(proxy_id));
        Replymx(pid,2,msg);  
        break;
      case GET_BUF_NUM :

        /* how many data buffers are there */

        buffer=dma_buf_num;
        status=0;
        _setmx(&msg[0],&status,sizeof(status)); 
        _setmx(&msg[1],&buffer,sizeof(buffer));
        Replymx(pid,2,msg);  
        break;

      case GET_BUF_SIZE :

        /* how big are the data buffers */

        status=0;
        size=DMA_BUF_SIZE;
        _setmx(&msg[0],&status,sizeof(status)); 
        _setmx(&msg[1],&size,sizeof(size));
        Replymx(pid,2,msg);  
        break;
      case GET_BUF_ADR :

        /* what is the address of the data buffers */

        _setmx(&msg[0],&buffer,sizeof(buffer));
        Readmsgmx(pid,sizeof(code),1,msg);
        status=0;
        if ((buffer <0) || (buffer>=dma_buf_num)) status=DRV_ERROR;
        _setmx(&msg[0],&status,sizeof(int));
        if (status==0) {
	  _setmx(&msg[1],&dma_buf[buffer],sizeof(dma_t));
	  Replymx(pid,2,msg);
        } else Replymx(pid,1,msg);			   
        break;
      case DO_SCAN :

        /* perform a new scan */
        _setmx(&msg[0],&buffer,sizeof(buffer));
        _setmx(&msg[1],&bytes,sizeof(bytes));
        _setmx(&msg[2],&mode,sizeof(mode));
        _setmx(&msg[3],&channels,sizeof(channels));
        Readmsgmx(pid,sizeof(code),4,msg);
        status=0;
       
        if ((buffer <0) || (buffer>=dma_buf_num)) status=DRV_ERROR;	    
        if ((bytes <=1) || (bytes>=DMA_BUF_SIZE)) status=DRV_ERROR;
        if (channels <=0) status=DRV_ERROR;

        /* let the task no whether we had a succesful start of scan */

        if (debug !=0) {          
          ptr=(int16 *) V_ADDR(&dma_buf[buffer]);
          for (n=0;n<bytes;n+=2) {
             rval=(int16) (nscl*rand()/RAND_MAX-nscl);
             ptr[n/2]=rval;
	  }
          _setmx(&msg[0],&status,sizeof(status)); 
          Replymx(pid,1,msg);  
        } else {         
          if (status==0) do_scan(buffer,bytes-1,mode,ad_dma,channels);
          _setmx(&msg[0],&status,sizeof(status)); 
          Replymx(pid,1,msg);  
          if (status ==0) scan_status=wait_on_scan();
        }  
        break;
      case GET_SCAN_STATUS :
        /* return the status of the last scan */
        if (debug !=0) scan_status=SCAN_OK;
        _setmx(&msg[0],&scan_status,sizeof(scan_status)); 
        Replymx(pid,1,msg);  
        break;
      default :
        status=DRV_STAT_UNKNOWN;
	_setmx(&msg[0],&status,sizeof(status));
	Replymx(pid,1,msg);
	break;
      }
    }
   
    /* if a scan error occured then reset the board */
    if ((scan_status !=SCAN_OK) && (debug==0)) {
      fprintf(stderr,"scan fault\n");
      dt_reset();
    }
  }
}
