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
 Revision 1.3  2008/03/12 18:35:13  code
 Todd modifications for matched filtering.

 Revision 1.2  2005/06/16 15:50:14  barnes
 Changed header name.

 Revision 1.1  2004/05/10 15:03:40  barnes
 Initial revision

*/
	
void setupGC214(long base_GC214,int cfg,int clkdly,int bcnt,int dlycnt) {
       
  write32(base_GC214+offset_GC214+reg_GC214_RSTCMD,0x00000001);
  delay(1);
  write32(base_GC214+offset_GC214+reg_GC214_CLKDLY,clkdly); 
  delay(1);
  write32(base_GC214+offset_GC214+reg_GC214_TCD,0x00000000);  

  write32(base_GC214+offset_GC214+reg_GC214_BCNT,bcnt);
  write32(base_GC214+offset_GC214+reg_GC214_DLYCNT,dlycnt);
  write32(base_GC214+offset_GC214+reg_GC214_CFG,cfg);
}


void enableGC214(long base_GC214) {
  write32(base_GC214+offset_GC214+reg_GC214_FRST,0x0000000f);
  read32(base_GC214+offset_GC214+reg_GC214_STAT);
  read32(base_GC214+offset_GC214+reg_GC214_ISR);
}

void triggerGC214(long base_GC214) {
  delay(5);
  write32(base_GC214+offset_GC214+reg_GC214_GTCR,0x0000010);
}






int pollGC214(long base_GC214) {
  int r;
  r=read32(base_GC214+offset_GC214+reg_GC214_STAT);
  return r;
}

int  pollGC214FIFO(long base_GC214) {
  int r;
  r=read32(base_GC214+offset_GC214+reg_GC214_FSTAT);
  return r;
}

