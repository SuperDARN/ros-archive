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
#include <i86.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <sys/pci.h>

#include <signal.h>
#include <time.h>
#include "rtypes.h"
#include "option.h"
#include "dma-alloc.h"
#include "pci.h"
#include "dma.h"
#include "gc214.h"
#include "gc4016.h"
#include "build.h"
#include "filter.h"
#include "version.h"
#include "taskname.h"

#define DRIVER_PRIORITY 15

/*
 $Log: main.c,v $
 Revision 1.3  2004/06/22 12:38:15  barnes
 QNX4 Code audit.

 Revision 1.2  2004/05/11 14:21:55  barnes
 Changed version handling.

 Revision 1.1  2004/05/10 15:03:40  barnes
 Initial revision

*/

#define GET_BUF_NUM 'n'
#define GET_BUF_ADR 'a'
#define GET_BUF_SIZE 's'
#define GET_SCAN_STATUS 'S'
#define DRV_ERROR 'e'
#define DO_SCAN 'd'
#define SET_SRATE 'r'
#define DO_FCLR_SCAN 'f'


#define SCAN_OK 0
#define SCAN_ERROR 'F'

#define GAIN_ADJ 1
#define GAIN_BACK_ARRAY 1

int cfircoeffs[11];
int pfircoeffs[32];
int resampcoeffs[256];

int irq;
long write_PCI9080_CNTRL;
long BASE1,BASE2,BASE3,PCICR;
long  baseGC214=0;

struct GC4016Global global;
struct GC4016Channel chana;
struct GC4016Channel chanb;
struct GC4016Channel chanc;
struct GC4016Channel chand;

int dmabufnum=DMA_BUF_MAX;
dma_t dmabuf[DMA_BUF_MAX];

char driver_name[256]={TASK_NAME};

struct OptionData opt;

void setcard(int Fsamp,int Fstop,int Fpass,int freqout) {

  int decimation;
  int coarse;
  float CFIRgain,PFIRgain,cicgain;
  int cicscale;
  int bigscale=0,shift=2,scale=0;
  int useshift=1;
  float gain;
  int finegain;
  int mix20b=0;

  float bigtmp;

  decimation=(int) ((float) Fsamp/(4*freqout));
  
  bigtmp=62+log10(pow(decimation,-5))/log10(2);
  bigscale=(int) floor(bigtmp/6);
  scale=(int) floor(1+bigtmp-shift-bigscale*6);

  if (bigscale<0) bigscale=0;
  if (scale<0) scale=0;
  if (bigscale>7) bigscale=7;
  if (scale>5) scale=5;

  cicscale=((mix20b<<6)+(bigscale<<3))+(scale) & 0x7f;
  if (decimation>4095) decimation=4095;

  cicgain=pow((float) decimation,5)*pow(2,shift+scale+6*bigscale-62);

  if (decimation<1000) {
    coarse=(int)(log10(1.0/cicgain)/log10(2));
    if (coarse<0) coarse=0;
  } else {
   /* Gain boost due to flat power over 
      wide bandwidth and significant decimation
   */
   coarse=(int) log10(pow((float) decimation,0.5))/log10(2)+2;
  }
  CFIRgain=setCFIR(cfircoeffs,Fsamp,Fpass,Fstop,decimation);
  PFIRgain=setPFIR(pfircoeffs,Fsamp,Fpass,Fstop,decimation);

  chana.chreset=(useshift<<0x03) | shift;
  chana.decratio=0x7000 | (decimation-1);
  chana.cicscale=cicscale;
  chana.cfir=0x00 | coarse<<0x04;

  chanb.chreset=(useshift<<0x03) | shift;
  chanb.decratio=0x7000 | (decimation-1);
  chanb.cicscale=cicscale;
  chanb.cfir=0x00 | coarse<<0x04;

  chanc.chreset=(useshift<<0x03) | shift;
  chanc.decratio=0x7000 | (decimation-1);
  chanc.cicscale=cicscale;
  chanc.cfir=0x00 | coarse<<0x04;

  chand.chreset=(useshift<<0x03) | shift;
  chand.decratio=0x7000 | (decimation-1);
  chand.cicscale=cicscale;
  chand.cfir=0x00 | coarse<<0x04;

  gain=CFIRgain*PFIRgain; /* removed *cicgain */

  finegain=(int) (GAIN_ADJ*1024/gain);
  if (finegain>=16384) finegain=16384;
  if (finegain<=0) finegain=0;
 
  chana.finegain=finegain;

  finegain=(int) (GAIN_BACK_ARRAY*GAIN_ADJ*1024/gain);
  if (finegain>=16384) finegain=16384;
  if (finegain<=0) finegain=0;

  chanb.finegain=finegain;

  chanc.finegain=finegain;
  chand.finegain=finegain;

}


int gatescan() { 
  int r;
  struct timespec start;
  struct timespec stop;
 
  clock_gettime(CLOCK_REALTIME,&start);
  
  while (1) {    
    r=pollGC214(baseGC214);
    if ((r & 0x3000)==0x3000) break;
    delay(5);
    clock_gettime(CLOCK_REALTIME,&stop);
    if ((stop.tv_sec-start.tv_sec)>1) return SCAN_ERROR;
  }
  return SCAN_OK;
}

int burstscan() { 
  int r;
  struct timespec start;
  struct timespec stop;
 
  clock_gettime(CLOCK_REALTIME,&start);
  
  while (1) {    
    r=pollGC214(baseGC214);
    if ((r & 0x1000)==0x1000 ) break;   
    delay(5);
    clock_gettime(CLOCK_REALTIME,&stop);
    if ((stop.tv_sec-start.tv_sec)>1) return SCAN_ERROR;
  }
  return SCAN_OK;
}




void main(int argc,char *argv[]) {

  unsigned char debug=0;
  int arg;

  int n; 

  char code;
  unsigned char *baseGC214pointer;
  int fd;
  int status;
  int loop;
  struct timespec res;
  pid_t tid,pid;
  int buffer,size,channel,sample,bandwidth;
  int scanstatus=0;

  int i;  

  struct _mxfer_entry msg[4];

  int16 *ptr;
  int16 rval;
 
  float nscl=10.0;

  int freqout=3333;

  int Fpass=2000;
  int Fstop=4000;
  int Fsamp=50625000;


  OptionAdd(&opt,"db",'x',&debug);
  arg=OptionProcess(1,argc,argv,&opt,NULL);   
  if (arg<argc) strcpy(driver_name,argv[arg]);


  if (debug !=0) {
     fprintf(stderr,"gc214 version: %d.%.2d debug\n",MAJOR_VERSION,
	     MINOR_VERSION);
  } else {
     fprintf(stderr,"gc214 version: %d.%.2d\n",MAJOR_VERSION,MINOR_VERSION);
  }
  fprintf(stderr,"Registered under the name: %s\n",driver_name);
  fprintf(stderr,"Number of DMA Buffers:%d\n",dmabufnum);



  /* set priority */
 
  if(setprio(0,DRIVER_PRIORITY) == -1) {
    perror("unable to set priority\n");
	exit(-1);
  }

  if ( (tid=qnx_name_attach(0,driver_name) ) == -1) { 
    perror("Unable to attach driver");
    exit(-1);
  }
  
  /* initialize PCI link to GC-214 reciever */

  if (debug==0) {
  
    if((write_PCI9080_CNTRL=initPCI9080(&irq,
                                        &BASE1,&BASE2,&BASE3,&PCICR))==-1) {
	perror("Unable to initialize PCI Bus");	
        exit(-1);
    }
 
    fd=shm_open("Physical",O_RDWR,0777);
    baseGC214pointer=mmap(0,0x80000,PROT_EXEC|PROT_READ|PROT_WRITE,
                          MAP_SHARED,fd,BASE3);
    baseGC214=(long int) baseGC214pointer;
  }
  res.tv_nsec=500000;
  if(clock_setres(CLOCK_REALTIME,&res)==-1){
    perror("Unable to set system clock resolution to 1 ms");
    exit(1);
  }
  clock_getres(CLOCK_REALTIME,&res);

  for (loop=0;loop<dmabufnum;loop++) {
    if (alloc_dmabuf(&dmabuf[loop],DMA_BUF_SIZE) !=0) {
      perror("Unable to allocate DMA buffer");
      exit(-1);
    }
  }  
  
  if (debug==0) {
    buildGC4016Global(&global);
    buildGC4016Channel(&chana);
    buildGC4016Channel(&chanb);
    buildGC4016Channel(&chanc);
    buildGC4016Channel(&chand);

    /* modify the default configuration - only parameter is "freqout" */

    setcard(Fsamp,Fstop,Fpass,freqout);
    for (i=0;i<256;i++) resampcoeffs[i]=0;
    resampcoeffs[0]=1024;
  }

  while (1) {
    pid=Receive(0,&code,sizeof(code));
    switch (code) {
     case GET_BUF_NUM :
       /* how many data buffers are there */

       buffer=dmabufnum;
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

       _setmx(&msg[0],&buffer,sizeof(int));
       Readmsgmx(pid,sizeof(code),1,msg);
       status=0;
       if ((buffer <0) || (buffer>=dmabufnum)) status=DRV_ERROR;	    
   
       _setmx(&msg[0],&status,sizeof(int));
       if (status==0) {
         _setmx(&msg[1],&dmabuf[buffer],sizeof(dma_t));
	      Replymx(pid,2,msg);
       } else Replymx(pid,1,msg);			   
       break;
     case GET_SCAN_STATUS :
       /* what is the status of the last scan */
       _setmx(&msg[0],&scanstatus,sizeof(scanstatus)); 
       Replymx(pid,1,msg);  
       break;
    case SET_SRATE:
       _setmx(&msg[0],&freqout,sizeof(freqout));  
       Readmsgmx(pid,sizeof(code),1,msg);

       Fstop=freqout+800;
       Fpass=freqout+800;
       status=0;
       _setmx(&msg[0],&status,sizeof(int));   
       Replymx(pid,1,msg);
       break;
     case DO_SCAN :
    
       /* perform a scan */
       _setmx(&msg[0],&buffer,sizeof(buffer));
       _setmx(&msg[1],&sample,sizeof(sample));
       _setmx(&msg[2],&channel,sizeof(channel));
       if (debug==0) { 
         setcard(Fsamp,Fstop,Fpass,freqout);
         setupGC214(baseGC214,0x000f1,0x202,0,0);
         setupGC4016(baseGC214,&global);
         setupGC4016channel(baseGC214,0,&chana,cfircoeffs,pfircoeffs);
         setupGC4016channel(baseGC214,1,&chanb,cfircoeffs,pfircoeffs);
         setupGC4016channel(baseGC214,2,&chanc,cfircoeffs,pfircoeffs);
         setupGC4016channel(baseGC214,3,&chand,cfircoeffs,pfircoeffs);
         releaseGC4016(baseGC214,resampcoeffs);

         enableGC214(baseGC214);
       }
       Readmsgmx(pid,sizeof(code),3,msg);

       status=0;
       _setmx(&msg[0],&status,sizeof(int));   
       Replymx(pid,1,msg);
            
       if (debug==0) {
         scanstatus=gatescan();
         if (scanstatus==SCAN_OK) {
           memcpy((char *) dmabuf[buffer].bufp,
                  (char *) (baseGC214+0x1000),sample*4);
           if (channel>1) 
             memcpy((char *) (dmabuf[buffer].bufp+(sample*4)),
		    (char *) (baseGC214+0x1000),sample*4);
         }
       } else {
         /* fill buffer with debug values */
         scanstatus=SCAN_OK;
         ptr=(int16 *) dmabuf[buffer].bufp;
         for (n=0;n<sample*2;n++) {
           rval=(int16) (nscl*rand()/RAND_MAX-nscl);
           ptr[n]=rval;
           ptr[n+2*sample]=rval;
	 }
       }  
       break;
       case DO_FCLR_SCAN :
       
         _setmx(&msg[0],&buffer,sizeof(buffer));
         _setmx(&msg[1],&sample,sizeof(sample));
         _setmx(&msg[2],&bandwidth,sizeof(bandwidth));

         Readmsgmx(pid,sizeof(code),3,msg);
         if (debug==0) {

  	   /* filter corner frequency=mean of setcard args 2&3 */
           setcard(Fsamp,0.85*bandwidth,0.85*bandwidth,bandwidth);
           setupGC214(baseGC214,0x000f4,0x202,sample,0);
           setupGC4016(baseGC214,&global);
           setupGC4016channel(baseGC214,0,&chana,cfircoeffs,pfircoeffs);
           setupGC4016channel(baseGC214,1,&chanb,cfircoeffs,pfircoeffs);
           setupGC4016channel(baseGC214,2,&chanc,cfircoeffs,pfircoeffs);
           setupGC4016channel(baseGC214,3,&chand,cfircoeffs,pfircoeffs);
           releaseGC4016(baseGC214,resampcoeffs);
           enableGC214(baseGC214);
         
           triggerGC214(baseGC214);
         
           scanstatus=burstscan();
         
           if (scanstatus==SCAN_OK) 
              memcpy((char *) dmabuf[buffer].bufp,(char *) (baseGC214+0x1000),
		     sample*4);
	 } else {
           /* fill with debug values */
           ptr=(int16 *) dmabuf[buffer].bufp;
           for (n=0;n<sample*2;n++) {
             rval=(int16) (nscl*rand()/RAND_MAX-nscl);
             ptr[n]=rval;
	   }
	 }
         _setmx(&msg[0],&scanstatus,sizeof(int));   
         Replymx(pid,1,msg);
    }
  }
}
