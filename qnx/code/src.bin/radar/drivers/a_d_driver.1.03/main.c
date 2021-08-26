/* main.c 
   ====== */

/*
 $Log: main.c,v $
 Revision 1.22  2000/06/16 19:23:57  barnes
 Added record of the proxy number to the start up messages.

 Revision 1.21  2000/04/17 21:03:29  barnes
 Added "scan fault" log message.

 Revision 1.20  2000/04/17 19:57:07  barnes
 Modification to include a time out if we lose the clock pulse.

 Revision 1.19  1999/04/15 15:42:38  barnes
 Fixed bug in reading the status bit.

 Revision 1.18  1999/04/10 20:26:58  barnes
 Added help message system.

 Revision 1.17  1999/03/30 19:08:26  barnes
 Moved task name into a header.

 Revision 1.16  1999/03/25 21:13:44  barnes
 Removed dt.dec and dt.ext.

 Revision 1.15  1998/11/06 15:21:16  barnes
 Added the ability to specify the card address, IRQ, and DMA channel
 on the command line.

 Revision 1.14  1998/09/16 19:27:37  barnes
 Moved away from 16 bit typing.

 Revision 1.13  1998/06/11 15:03:28  root
 Moved the usage message into another file.

 Revision 1.12  1998/06/03 19:06:01  root
 Modification to the version control system.

 Revision 1.11  1998/05/27 09:51:01  root
 Updated usage message.

 Revision 1.10  1998/05/27 09:48:05  root
 Modifications to allow the number of DMA buffers to be specified on the
 command line.

 Revision 1.9  1997/04/16 11:03:47  root
 Changed uasge message.

 Revision 1.8  1997/04/16 10:52:46  root
 Added code to specify debug mode at run time.

 * Revision 1.7  1996/09/23  14:13:22  root
 * Changed to global name location.
 *
 * Revision 1.6  1996/05/31  13:53:02  root
 * Added ability to specify the task name on the command line.
 *
 * Revision 1.5  1996/04/17  10:03:08  root
 * Modified proxy message to cope with 16 and 32 bit OS.
 *
 * Revision 1.4  1996/04/15  20:50:21  root
 * Added message to allow calling task to identify the
 * scan proxy so that a reset can be triggered automatically.
 *
 * Revision 1.3  1996/04/15  14:11:06  root
 * Modified proxy detection to allow a reset from another task.
 *
 * Revision 1.2  1996/03/14  10:24:31  root
 * Added debug code.
 *
 * Revision 1.1  1996/03/13  14:03:36  root
 * Initial revision
 *
*/

/* radar a_d_driver */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <signal.h>
#include <time.h>
#include "print_info.h"

#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"
#include "version.h"
#include "task_name.h"
#include "hlpstr.h"

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
  char code;
  int debug=0;
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
  char *tbuf;
  struct _mxfer_entry msg[4];
  int help=0;

  /* set priority */
 
  if(setprio(0,DRIVER_PRIORITY) == -1) {
    perror("unable to set priority\n");
	exit(1);
  }

  if (argc>1) { 
    int c;
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1; 
      if (strcmp(argv[c],"-db")==0) debug=1;
      else if (strcmp(argv[c],"-i")==0) {
        ad_irq=atoi(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-d")==0) {
        ad_dma=atoi(argv[c+1])-4;
        if (ad_dma<1) ad_dma=1;
        if (ad_dma>3) ad_dma=3;
        c++;
      } else if (strcmp(argv[c],"-p")==0) {
        sscanf(argv[c+1],"%x",&ad_port);
        c++;
      } else if (strcmp(argv[c],"-n")==0) {
        dma_buf_num=atoi(argv[c+1]);
        c++;
      } else strcpy(driver_name,argv[c]);
    }
  }
 
  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


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
     fprintf(stderr,"a_d_drive version: %g debug\n",VERSION);
  } else {
     fprintf(stderr,"a_d_drive version: %g\n",VERSION);
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
		    _setmx(&msg[1],&dma_buf[buffer],sizeof(int));
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
