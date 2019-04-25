/* dma.c 
   ===== 
   Author: R.Todd Parris, R.J.Barnes
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
#include <i86.h>
#include <conio.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <sys/pci.h>
#include <time.h>
#include <signal.h>
#include "registers.h"

/*
 $Log: dma.c,v $
 Revision 1.2  2008/03/12 18:35:13  code
 Todd modifications for matched filtering.

 Revision 1.1  2004/05/10 15:03:28  barnes
 Initial revision

*/


void DMAxfer(long addr,long dmapadr,long dmaladr,long dmasiz,long pcicntrl){
  long temp_out,temp_in;
  temp_out=outpd(addr+reg_PCI9080_BIGEND,0x00);
  temp_in=inpd(addr+reg_PCI9080_BIGEND);
  temp_out=outpd(addr+reg_PCI9080_CNTRL,pcicntrl);
  temp_in=inpd(addr+reg_PCI9080_CNTRL);
  temp_in=inpd(addr+reg_PCI9080_INTCSR);
  temp_out=outpd(addr+reg_PCI9080_INTCSR,temp_in | 0x00080000);
  temp_out=outpd(addr+reg_PCI9080_DMACSR1,0x8);
  temp_out=outpd(addr+reg_PCI9080_DMACSR1,0x0);
  temp_out=outpd(addr+reg_PCI9080_DMAMODE1,0x00Dc3);
  temp_out=outpd(addr+reg_PCI9080_DMAPADR1,dmapadr);
  temp_out=outpd(addr+reg_PCI9080_DMALADR1,dmaladr);
  temp_out=outpd(addr+reg_PCI9080_DMASIZ1,dmasiz);
  temp_out=outpd(addr+reg_PCI9080_DMADPR1,0x8);
  temp_out=outpd(addr+reg_PCI9080_DMACSR1,0x01);
  temp_out=outpd(addr+reg_PCI9080_DMACSR1,0x3);
}


void DMAabort(long addr){
  long temp_out,temp_in;

  temp_out=outpd(addr+reg_PCI9080_DMACSR1,0x0);
  temp_out=outpd(addr+reg_PCI9080_CNTRL,0x10000);
  temp_in=inpd(addr+reg_PCI9080_CNTRL);
  temp_out=outpd(addr+reg_PCI9080_DMACSR1,0x4);
}


int DMApoll(long addr) {
  struct timespec interval;
  struct timespec remaining;
  struct timespec start;
  struct timespec stop;
  
  interval.tv_sec=0;
  interval.tv_nsec=400000;
  clock_gettime(CLOCK_REALTIME,&start);
  while ((inpd(addr+reg_PCI9080_DMACSR1) & 0x00000010) !=0x10) {
    clock_gettime(CLOCK_REALTIME,&stop);
    if ((stop.tv_sec-start.tv_sec)>1) {
      DMAabort(addr);
      return -1;
    }
    nanosleep(&interval,&remaining);
  }
  return 0;
}
