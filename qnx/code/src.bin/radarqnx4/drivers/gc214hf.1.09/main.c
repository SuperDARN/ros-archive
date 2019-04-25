/* main.c 
   ====== 
   Author: R.Todd Parris, R.J.Barnes & D.Andre
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
#include "gc214card.h"
#include "gc4016.h"
#include "build.h"
#include "filter.h"
#include "version.h"
#include "taskname.h"


#define DRIVER_PRIORITY 15
#define METEOR_OVERSAMP_FACTOR 10
#define RESAMP_BYPASS 0
#define MATCHED 1

/*
 $Log: main.c,v $
 Revision 1.8  2008/03/14 18:26:49  code
 Removed debug code.

 Revision 1.7  2008/03/12 21:46:06  code
 Adapted Todd's IF code to HF.

 Revision 1.6  2007/11/19 16:43:31  code
 Removed redundant code.

 Revision 1.5  2007/03/28 17:48:34  code
 Changed the sample rate to a float (Todd Parris correction).

 Revision 1.4  2006/02/07 16:09:43  barnes
 Used fixed coefficient set from the data sheet.

 Revision 1.3  2006/01/25 16:36:18  barnes
 Added test for receiver overload

 Revision 1.2  2005/07/25 15:30:37  barnes
 Removed redundant variable.

 Revision 1.1  2005/07/25 15:12:45  barnes
 Initial revision

*/


#define GAIN_ADJ 3
#define FINAL_SHIFT_ADJ 1
#define GAIN_BACK_ARRAY 1


int cfircoeffsAB[11]={ -24, 74, 494, 548, -977, -3416, -3672, 1525, 13074, 26547, 32767};
int cfircoeffsCD[11]={ -24, 74, 494, 548, -977, -3416, -3672, 1525, 13074, 26547, 32767};
int pfircoeffsAB[32]={ 14, 30, 41, 27, -29, -118, -200, -212, -95, 150,
                     435, 598, 475, 5, -680, -1256, -1330, -653, 669,
                     2112, 2880, 2269, 101, -2996, -5632, 6103,
                     -3091, 3666, 13042, 22747, 30053, 32767};
int pfircoeffsCD[32]={ 14, 30, 41, 27, -29, -118, -200, -212, -95, 150,
                     435, 598, 475, 5, -680, -1256, -1330, -653, 669,
                     2112, 2880, 2269, 101, -2996, -5632, 6103,
                     -3091, 3666, 13042, 22747, 30053, 32767};
int resampcoeffs[256];


int irq, matched=0;
long write_PCI9080_CNTRL;
long BASE1,BASE2,BASE3,PCICR;
long  baseGC214=0;
float freqoutCD=50000;

float Tdelay,smpdlyF,Trise=1e-6;
int smpdlyAB=18,smpdlyCD=18,smperr=0,fclrflag=0;

struct GC4016Global global;
struct GC4016Channel chana;
struct GC4016Channel chanb;
struct GC4016Channel chanc;
struct GC4016Channel chand;

int dmabufnum=DMA_BUF_MAX;
dma_t dmabuf[DMA_BUF_MAX];

char driver_name[256]={TASK_NAME};

struct OptionData opt;

void setcard(int Fsamp,int Fstop,int Fpass,float freqout) {

  int decimation;
  int coarse,finalshift;
  float CFIRgain, CFIRfreq;
  float PFIRgain, PFIRfreq;
  float cicgain;
  float resampgain, resampfreq;
  int cicscale;
  int bigscale,shift,scale=0,ndelay;
  float gain;
  int finegain;

  


  /* CALCULATE PARAMETERS FOR CHANNELS A&B ******************/

  /* Channels A and B are set up for use with typical superDARN
     modes.  Channel A is dedicated to input CH1, which is connected
     to the main array of the radar.  Channel B is dedicated to input
     CH2, which is connected to the secondary/interferometry array.
  */

  /* If a matched filter is desired, then the sample rate into
     the CFIR filter must be great enough to allow a good filter
     with a cutoff at the TX bandwidth to be implemented.  This
     requres calculating the decimation assuming a basband rate
     equale to the TX bandwidth (50kHz for most radars).  The re-
     sampler is used to decimate to the desired baseband rate. If
     a matched filter is not desired, then out if band rejection 
     can be improved by setting the bandwidths of the CFIR and PFIR
     to be similar.  The necessitates calculating the decimation
     differently when a matched filter is used.  
  */

  if(RESAMP_BYPASS){
    decimation=(int) ((float) Fsamp/(4*freqout));
    ndelay=1;
    if(matched){
#ifdef LOGERROR
      tod=time(NULL);
      fprintf(gcerrf,"Cannot use matched filter with resampler bypass: %s\n",
              ctime(&tod));
#endif
	}
        matched=0;
  }
  else if(matched){
    decimation=(int) ((float) Fsamp/(4*50000));
    ndelay=32;
    matched=1;
  } else{
    /* If the resampler is used, then the decimation must be
       calculated differently.  The resampler performance
       decreases when its input rate is less than 1.5 times
       the signal bandwidth.  This deicmation calculation 
       assures that the input bandwidth is always at least
       1.5 (6/4) times the desired output rate (BW). See
       note 1 on page 12 of the GC4016 manual. It should be
       noted, that when the resampler is used, there is an
       introduced jitter of 1/(2*Ndelay)*output rate. See 
        page 12.  For our settings, this is a max jitter of
        1/64th the output rate.
    */

    decimation=(int) ((float) Fsamp/(6*freqout));
    ndelay=32;
    matched=0;
  }

  /* Now that the decimation has been calculated, apply the restrictions
     defined in the GC4096 Manual, pg.7. It should be noted that when the 
     resampler is used, the decimation is less than when no resampler is used
     (this is due to the factor of 10 in the decimation calculation, vs the
     factor of 4 when the resampler is not used).  This means that the
     decimation may be too small at large output frequencies.  This is not a
     significant problem, so long as the output sample rate is >= Fsamp/32.
     If the decimation is too small, it will be limited to the lowest allowed
     value, and the performance of the resampler filter will degrade.  However,
     if an output rate of >= Fsamp/32 is requested, then there will be 
     significant aliasing within the band of interest.
  */
  if (decimation>4096){
    decimation=4096;
#ifdef LOGERROR
    tod=time(NULL);
    fprintf(gcerrf, "Decimation too great: %s\n",ctime(&tod));
#endif
  }
  if (decimation<8){
    decimation=8;
#ifdef LOGERROR
    tod=time(NULL);
    fprintf(gcerrf, "Decimation too small: %s\n",ctime(&tod));
#endif
  }

  /* Now that the decimation is known, the sample delay through
     the receiver card can be calculated.  This is basedd on the delay
     throught tht receiver as defined in the GC4016 manula on pg. 25.
     This delay is calculated (now that we use the resampler, the output
     data rate is NOT directly related to the decimation, so this is NOT
     a fixed number of samples, regardless of output rate).  Two 'samples'
     are added to account for the header (64 bits), and the rise time of the
     TX pulse is removed (if we are sampleing very fast, the those samples
     in the rise of the pulse are not considered - this is crucial for 
     range calculations in the oversampled data).
  */

  Tdelay=((2.5*decimation)+(0.5*decimation*21)+(decimation*63)+
          (2*decimation*8)+40)/Fsamp;

  smpdlyF=Tdelay*(float)freqout;
  smpdlyF+=2;
  smpdlyF+=(Trise/(1/(float)freqout));
  smpdlyAB=(int)ceil(smpdlyF);


  /* If the decimation is greater than 3104, then the gain through
     the CIC decimator will be greater than 1 if MIX20 is not disabled
     and shift is not reduced.  A gain of greater than 1 in the CIC
     will overflow the output of the CIC, clipping either I, Q, or both
     if the input signal it large.  This next  routine calculates when
     this condition may occur, and prevents sets MIX20 and shift 
     appropriately (see pages 8,39 in the GC4016 Manual
  */

  if (decimation>3104){
    shift=0;
    bigscale=(int)((62-5*log10(decimation)/log10(2)-shift-scale)/6);
    if(bigscale>7) bigscale=7;
    if(bigscale<0) bigscale=0;
    cicscale=((bigscale<<3)+(scale)) & 0x3f;
  } else{
    shift=4;
    bigscale=(int)((62-5*log10(decimation)/log10(2)-shift-scale)/6);
    if(bigscale>7) bigscale=7;
    if(bigscale<0) bigscale=0;
    cicscale=((0x1<<6)+(bigscale<<3))+(scale) & 0x7f;
  }

  /* Calculate the cicgain, and use coarse to adjust it accordingly.
     See page 8 in GC4016 Manual
  */

  cicgain=pow((float) decimation,5)*pow(2,shift+scale+6*bigscale-62);
  coarse=(int)(log10(1.0/cicgain)/log10(2));  
  if(coarse>7) coarse=7;
  if(coarse<0) coarse=0;

  /* Calculate the sample rates for the inputs of the CFIR, PFIR, and
     resampler
  */

  CFIRfreq=(float)Fsamp/(float)decimation;
  PFIRfreq=CFIRfreq/2;
  resampfreq=ndelay*PFIRfreq/2;


  /* Again, if a matched filter is desired, then the CFIR and resampler will
     be set to 50kHz BW (to match the filter on the transmitter), while the 
     PFIR will be set to match the sinc spectral shape of the ideal TX pulse.
     The last parameter of 'setPFIR' tells the routine
     to set the filter response equal to the spectrum of a pulse 
     width of 1/freqout.
     Setting that parameter to 0 tells 'setPFIR' to use an ideal lorpass 
     filter with a two-sided BW of freqout. There is also a provision
     in 'setresamp' to set the resampler coeffs appropriately for 
     the resampler to be bypassed (see page 15 in GC4016 manual).  
     IF THE RESAMPLER IS BYPASSED, THEN THE OUTPUT DATA RATE
     OF THE GC214PCI/TS WILL NOT NECESSARILY BE EXACTLY THE RATE REQUESTED.
  */

  if(matched){
    CFIRgain=setCFIR(cfircoeffsAB,CFIRfreq,50000,50000);
    PFIRgain=setPFIR(pfircoeffsAB,PFIRfreq,freqout,freqout,1);  
    resampgain=setresamp(resampcoeffs,resampfreq,25000,25000,0,RESAMP_BYPASS);
  } else{
    CFIRgain=setCFIR(cfircoeffsAB,CFIRfreq,freqout,freqout);
    PFIRgain=setPFIR(pfircoeffsAB,PFIRfreq,freqout,freqout,0);
    resampgain=setresamp(resampcoeffs,resampfreq,freqout,freqout,0,
                         RESAMP_BYPASS);
  }

  /* Calculate a number of parameters for the GC4016 Channels */

  finalshift=(int)( log10((float)ndelay/resampgain)/log10(2) );		

  /* see reampler gain on page 15 of the GC4016 manual */

  finalshift=finalshift+1;  						

  /* the +1 was added to keep the gain of the resampler > 1 to 
     prevent saturation in the PFIR stage (where the finegain is added).
     the FINAL_SHIFT_ADJ adds an extra layer of narrow band gain
  */

  if(finalshift>15) finalshift=15; 

  /*  limit on final shift (page 14 in GC4016 manual */

  if(finalshift<0) finalshift=0;			
	
  /*  limit on final shift (page 14 in GC4016 manual */


  resampgain=(resampgain/ndelay)*pow(2,finalshift);

  gain=cicgain * pow(2,coarse) * CFIRgain * PFIRgain * resampgain;

  /* calculate the gain without fine gain set to allow fine gain to be
      used to achieve a tital gain of 1
  */

  finegain=(int) (2*GAIN_ADJ*1024/gain); 		
  finalshift=finalshift+FINAL_SHIFT_ADJ;  						
  if(finalshift>15) finalshift=15; 
  if(finalshift<0) finalshift=0;			

  /*  the factor of 2 compensates for adding 1 to finalshift, giving a 
      gain of 1. GAIN_ADJ allows the narrow band gain to be increased to allow
      small signals to be pulled out of the noise
  */


  if (finegain>=16383) finegain=16383; 

  /* limit on finegain (pg.9 in GC4016 manual) */

  if (finegain<=1) finegain=1024;		

  /* if negative or zero gain is needed, then signals will be wrong, 
    so set fine gain to 1 
  */

  /* Set the parameters to the values calculated */
  chana.decratio=0x7000 | (decimation-1);
  chanb.decratio=0x7000 | (decimation-1);
  chana.cicscale=cicscale;
  chanb.cicscale=cicscale;
  chana.cfir=0x00 | coarse<<4;
  chanb.cfir=0x00 | coarse<<4;
  chana.finegain=finegain;
  chanb.finegain=finegain;
  chana.input= 0x00;
  chanb.input= 0x01;
  chana.chreset=0x08+shift & 0x0f;
  chanb.chreset=0x08+shift & 0x0f;


  /* This routine sets the sampler appropriately if the reampler is to be
     bypassed (see page 15 in the GC4016 manual).  IF THE RESAMPLER IS 
     BYPASSED, THEN THE OUTPUT DATA RATE IS EQUAL TO FCLOCK/(4*DECIMATION).
     IF IT IS NOT BYPASSES, THEN THE OUTPUT DATA RATE IS EQUAL TO 'freqout'.
  */

  if(RESAMP_BYPASS){
    global.nmult=0x46;
    global.filtslct=0x00;
    global.finalshft=0x15;
    global.ratio0=0x04000000;
    global.ratio1=0x04000000;
  } else {
    global.nmult=0x07;
    global.filtslct=0x50;
    global.finalshft=0x10 | finalshift;
    global.ratio0=(int)((((float)Fsamp/(4*(float)decimation))/
                          (float)freqout)*(float)0x04000000);
    global.ratio1=(int)((((float)Fsamp/(4*(float)decimation))/
                          (float)freqout)*(float)0x04000000);
  }


  /* CALCULATE PARAMETERS FOR CHANNELS C&D ******************************/
  /* Channels C and D are set up for use with Todd Parris' meteor detection
     code.  Channel C is dedicated to input CH1, which is connected
     to the main array of the radar.  Channel D is dedicated to input
     CH2, which is connected to the secondary/interferometry array.
     These channels oversample the echoes with broader filters to allow
     edge detection of echoes from 'point' targets (such as meteor trails).
     Channels C and D are set just the same as channels A and B, except the
     output frequecny, and filter frequencies are multiplied by 'mosf' (meteor
     oversampling factor), and matched filtering is used.
  */

  if(RESAMP_BYPASS) {
    decimation=(int) ((float) Fsamp/(4*freqoutCD));	       

    /* the factor of 10 allows the input data rate to the resampler, 
       CFIR and PFIR to be fast enough to allow effective filter 
       coeffs to be used
     */
    ndelay=1;
  } else if(matched){
    decimation=(int) ((float) Fsamp/(4*50000));
    ndelay=32;
    matched=1;
  } else{
    decimation=(int) ((float) Fsamp/(6*freqoutCD));		

    /* the factor of 6 allows the input data rate to the resampler, 
       CFIR and PFIR to be fast enough to allow effective filter coeffs
       to be used
    */
    ndelay=32;
  }

  if (decimation>4096){
    decimation=4096;
#ifdef LOGERROR
    tod=time(NULL);
    fprintf(gcerrf, "Decimation on channels C&D too great: %s\n",ctime(&tod));
#endif
  }
  if (decimation<8){
    decimation=8;
#ifdef LOGERROR
    tod=time(NULL);
    fprintf(gcerrf, "Decimation on channels C&D too small: %s\n",ctime(&tod));
#endif
  }

  Tdelay=((2.5*decimation)+(0.5*decimation*21)+(decimation*63)+
          (2*decimation*8)+40)/Fsamp;

  smpdlyF=Tdelay*(float)freqoutCD;
  smpdlyF+=2;
  smpdlyF+=(Trise/(1/(float)freqoutCD));
  smpdlyCD=ceil(smpdlyF);

  if (decimation>3104){
    shift=0;
    bigscale=(int)((62-5*log10(decimation)/log10(2)-shift-scale)/6);
    if(bigscale>7) bigscale=7;
    if(bigscale<0) bigscale=0;
    cicscale=((bigscale<<3)+(scale)) & 0x3f;
  } else {
    shift=4;
    bigscale=(int)((62-5*log10(decimation)/log10(2)-shift-scale)/6);
    if(bigscale>7) bigscale=7;
    if(bigscale<0) bigscale=0;
    cicscale=((0x1<<6)+(bigscale<<3))+(scale) & 0x7f;
  }

  cicgain=pow((float) decimation,5)*pow(2,shift+scale+6*bigscale-62);
  coarse=(int)(log10(1.0/cicgain)/log10(2));  
  if(coarse>7) coarse=7;
  if(coarse<0) coarse=0;

  CFIRfreq=(float)Fsamp/(float)decimation;
  PFIRfreq=CFIRfreq/2;
  resampfreq=ndelay*PFIRfreq/2;



  if (matched) {
    CFIRgain=setCFIR(cfircoeffsCD,CFIRfreq,50000,50000);
    PFIRgain=setPFIR(pfircoeffsCD,PFIRfreq,freqout,freqout,1);
    resampgain=setresamp(resampcoeffs,resampfreq,25000,25000,1,
                             RESAMP_BYPASS);
  } else{
    CFIRgain=setCFIR(cfircoeffsCD,CFIRfreq,freqout,freqout);
    PFIRgain=setPFIR(pfircoeffsCD,PFIRfreq,freqout,freqout,0);
    resampgain=setresamp(resampcoeffs,resampfreq,freqout,freqout,1,
                             RESAMP_BYPASS);
  }






  /* Finalshift calculation ommitted for channels C & D because a 
     change would affect the integrity of the data in channels A and B.  
     The value calculated for channels A and B is used for channels C and D
    (this value is NOT independendent for each channel)
  */

  resampgain=(resampgain/ndelay)*pow(2,finalshift);

  gain=cicgain * pow(2,coarse) * CFIRgain * PFIRgain * resampgain;      
  finegain=(int) (2*GAIN_ADJ*1024/gain); 				
  if (finegain>=16384) finegain=16384;					
  if (finegain<=1) finegain=1024;					

  /* Set the parameters to the values calculated */

  chanc.decratio=0x7000 | (decimation-1);
  chand.decratio=0x7000 | (decimation-1);
  chanc.cicscale=cicscale;
  chand.cicscale=cicscale;
  chanc.cfir=0x00 | coarse<<4;
  chand.cfir=0x00 | coarse<<4;
  chanc.finegain=finegain;
  chand.finegain=finegain;
  chanc.input= 0x00;
  chand.input= 0x01;
  chanc.chreset=0x08+shift & 0x0f;
  chand.chreset=0x08+shift & 0x0f;


  /* This routine sets the sampler appropriately if the reampler is to be
     bypassed (see page 15 in the GC4016 manual).  IF THE RESAMPLER IS 
     BYPASSED, THEN THE OUTPUT DATA RATE IS EQUAL TO FCLOCK/(4*DECIMATION).
     IF IT IS NOT BYPASSES, THEN THE OUTPUT DATA RATE IS EQUAL TO 'freqout'.
  */

  if(RESAMP_BYPASS){
    global.ratio2=0x04000000;
    global.ratio3=0x04000000;
  } else {
    global.ratio2=(int)((((float)Fsamp/(4*(float)decimation))/
                        (float)(freqoutCD))*(float)0x04000000);
    global.ratio3=(int)((((float)Fsamp/(4*(float)decimation))/
                        (float)(freqoutCD))*(float)0x04000000);
  }
}


int gatescan() { 
  int r;
  struct timespec start;
  struct timespec stop;
 
  clock_gettime(CLOCK_REALTIME,&start);
  
  while (1) {    
    r=pollGC214(baseGC214);
    if ((r & 0x3000)==0x3000) break;
    if ((r & 0x0300)!=0) return SCAN_OVERFLOW; 
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
    if ((r & 0x0300)!=0) return SCAN_OVERFLOW;
    delay(5);
    clock_gettime(CLOCK_REALTIME,&stop);
    if ((stop.tv_sec-start.tv_sec)>1) return SCAN_ERROR;
  }
  return SCAN_OK;
}




void main(int argc,char *argv[]) {

  unsigned char debug=0;
  int arg;

  int n, x, tmp; 

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

  float freqout=3333;
  int tfreq=0;

  int Fpass=2000;
  int Fstop=4000;
  int Fsamp=40625000;

  OptionAdd(&opt,"db",'x',&debug);
  OptionAdd(&opt,"clock",'i',&Fsamp);

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
    chanb.input= 0x01;
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

       Fstop=(int)freqout;
       Fpass=(int)freqout;

       status=0;
       _setmx(&msg[0],&status,sizeof(int));   
       Replymx(pid,1,msg);
       break;

    case SET_TX_FREQ:
       _setmx(&msg[0],&tfreq,sizeof(tfreq));  
       Readmsgmx(pid,sizeof(code),1,msg);
       
       chana.freq= pow( 2, 32)* 
            ( (double) ( tfreq* 1000))/ ( (double) Fsamp);
       chanb.freq= chana.freq;
       chanc.freq= chana.freq;
       chand.freq= chana.freq;
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
	 if (MATCHED) matched=1;
	 else matched=0;
         setcard(Fsamp,Fstop,Fpass,freqout);
         setupGC214(baseGC214,0x000f1,0x202,0,0);
         setupGC4016(baseGC214,&global, resampcoeffs);
         setupGC4016channel(baseGC214,0,&chana,cfircoeffsAB,pfircoeffsAB);
         setupGC4016channel(baseGC214,1,&chanb,cfircoeffsAB,pfircoeffsAB);
         setupGC4016channel(baseGC214,2,&chanc,cfircoeffsCD,pfircoeffsCD);
         setupGC4016channel(baseGC214,3,&chand,cfircoeffsCD,pfircoeffsCD);
         releaseGC4016(baseGC214);

         enableGC214(baseGC214);
       }
       Readmsgmx(pid,sizeof(code),3,msg);

       status=0;
       _setmx(&msg[0],&status,sizeof(int));   
       Replymx(pid,1,msg);
            
       if (debug==0) {
	     for(x=0;x<smpdlyAB+30;x++) 
                  tmp=*((unsigned long *)(baseGC214+0x1000));
	     for(x=0;x<smpdlyAB+30;x++) 
                  tmp=*((unsigned long *)(baseGC214+0x20000));
         scanstatus=gatescan();
         if (scanstatus !=SCAN_ERROR) {
           for (x=0;x<sample;x++) 
	     *((unsigned long *) (dmabuf[buffer].bufp+4*x))=
             *((unsigned long *) (baseGC214+0x1000));
           if (channel>1) for (x=0;x<sample;x++) 
	     *((unsigned long *) (dmabuf[buffer].bufp+sample*4+4*x))=
             *((unsigned long *) (baseGC214+0x20000));
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

	   matched=0;
  	   /* filter corner frequency=mean of setcard args 2&3 */
           setcard(Fsamp,bandwidth,bandwidth,(float)bandwidth);
           setupGC214(baseGC214,0x000f4,0x202,sample+smpdlyAB,0);
           setupGC4016(baseGC214,&global,resampcoeffs);
           setupGC4016channel(baseGC214,0,&chana,cfircoeffsAB,pfircoeffsAB);
           setupGC4016channel(baseGC214,1,&chanb,cfircoeffsAB,pfircoeffsAB);
           setupGC4016channel(baseGC214,2,&chanc,cfircoeffsAB,pfircoeffsAB);
           setupGC4016channel(baseGC214,3,&chand,cfircoeffsAB,pfircoeffsAB);
           releaseGC4016(baseGC214);
           enableGC214(baseGC214);
         
           triggerGC214(baseGC214);
         
           scanstatus=burstscan();
         
           if (scanstatus !=SCAN_ERROR) {
	     for(x=0;x<smpdlyAB;x++) 
                tmp=*((unsigned long *)(baseGC214+0x1000));

             for (x=0;x<sample;x++) 
	     *((unsigned long *) (dmabuf[buffer].bufp+4*x))=
             *((unsigned long *) (baseGC214+0x1000));
	   }
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

