/* gc214.c 
   ======= 
   Author:R.Todd Parris, R.J.Barnes
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
#include "registers.h"
#include "io.h"
#include "gc214card.h"

/*
 $Log: gc214.c,v $
 Revision 1.1  2007/05/17 19:03:21  code
 Initial revision

*/
	
void setupGC214(long base_GC214,int cfg,int clkdly,int bcnt,
                int metcnt,int dlycnt) {

       
  write32(base_GC214+offset_GC214+reg_GC214_RSTCMD,0x00000003);
  write32(base_GC214+offset_GC214+reg_GC214_CLKDLY,0x000001ff); 
  write32(base_GC214+offset_GC214+reg_GC214_CH1ID,0x00010000);  
  write32(base_GC214+offset_GC214+reg_GC214_CH2ID,0x00020000);  
  write32(base_GC214+offset_GC214+reg_GC214_CH3ID,0x00030000);  
  write32(base_GC214+offset_GC214+reg_GC214_CH4ID,0x00040000);  
  write32(base_GC214+offset_GC214+reg_GC214_CH1FS,bcnt);  
  write32(base_GC214+offset_GC214+reg_GC214_CH2FS,bcnt);  
  write32(base_GC214+offset_GC214+reg_GC214_CH3FS,metcnt);  
  write32(base_GC214+offset_GC214+reg_GC214_CH4FS,metcnt);  
  write32(base_GC214+offset_GC214+reg_GC214_CFG,0x00000057);
  write32(base_GC214+offset_GC214+reg_GC214_SMSK,0x00000001);
  write32(base_GC214+offset_GC214+reg_GC214_SCMD,0x00000001);
}


void enableGC214(long base_GC214) {
  write32(base_GC214+offset_GC214+reg_GC214_SCMD,0x00000000);
  read32(base_GC214+offset_GC214+reg_GC214_STAT);
  read32(base_GC214+offset_GC214+reg_GC214_ISR);
  write32(base_GC214+offset_GC214+reg_GC214_ICR,0x00000000);
  write32(base_GC214+offset_GC214+reg_GC214_CHCTRL,0x00000000);
  write32(base_GC214+offset_GC214+reg_GC214_SMSK,0x000000fc);
  write32(base_GC214+offset_GC214+reg_GC214_SCMD,0x00000010);
  write32(base_GC214+offset_GC214+reg_GC214_RSTCMD,0x00000002);
  write32(base_GC214+offset_GC214+reg_GC214_CHCTRL,0x0000000f);
  write32(base_GC214+offset_GC214+reg_GC214_SMSK,0x00000fff);
}

void triggerGC214(long base_GC214) {
  write32(base_GC214+offset_GC214+reg_GC214_SCMD,0x0000010);
}

int pollGC214(long base_GC214) {
  int r;
  int temp;
  temp=read32(base_GC214+offset_GC214+reg_GC214_CH1ID);
  r=(int)(temp&0x0000ffff);
  return r;
}

int  pollGC214FIFO(long base_GC214) {
  int r;
  r=read32(base_GC214+offset_GC214+reg_GC214_STAT);
  return r;
}

void stopGC214(long base_GC214){

  write32(base_GC214+offset_GC214+reg_GC214_CHCTRL,0x00000000);  
  write32(base_GC214+offset_GC214+reg_GC214_SMSK,0x00000004);  
  write32(base_GC214+offset_GC214+reg_GC214_SCMD,0x00000010);

}
