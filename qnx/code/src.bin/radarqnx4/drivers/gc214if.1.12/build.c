/* build.c 
   ======= */

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
#include "dma-alloc.h"
#include "registers.h"
#include "io.h"
#include "gc214card.h"
#include "gc4016.h"

/*
 $Log: build.c,v $
 Revision 1.2  2005/06/16 15:50:14  barnes
 Changed header name.

 Revision 1.1  2004/05/10 15:02:58  barnes
 Initial revision

*/

void buildGC4016Global(struct GC4016Global *ptr) {
  ptr->nchanout=0x23;      /* 0x27 */
  ptr->nmult=0x07;         /* 0x46 */
  ptr->filtslct=0x00; 
  ptr->finalshft=0x14;     /* 0x16 */
  ptr->chanmap=0xe4; 
  ptr->addto=0x70; 
  ptr->resampclkdvd=0x00;
  ptr->ratiomap=0xe4;
  ptr->ratio0=0x04000000;
  ptr->ratio1=0x04000000;
  ptr->ratio2=0x04000000;
  ptr->ratio3=0x04000000;
  ptr->cntbyte0=0x00;
  ptr->cntbyte1=0x00;
  ptr->tricntrl=0xff;
  ptr->outformat=0x49;
  ptr->outmode=0x6c;
  ptr->outframecntrl=0xc7;
  ptr->outwdsize=0xef;
  ptr->outclkcntrl=0x01;
  ptr->sermuxcntrl=0xe4;
  ptr->outtaga=0x10;
  ptr->outtagb=0x32;
  ptr->outtagc=0x54;
  ptr->outtagd=0x76;
}

void buildGC4016Channel(struct GC4016Channel *ptr) {
   ptr->chreset=0x0c;
   ptr->phase=0x0000;
   ptr->freq=3446578694;  /* freq */
   ptr->decratio=0x703f;  /* 0x7000 | decimation -1 */
   ptr->cicscale=0x64;    /* cicscale */
   ptr->splitiq=0x00;
   ptr->cfir=0x00;        /* 0x00 | coarse<<4 */
   ptr->pfir=0x00;  
   ptr->input=0x00;       /* 0x01 */
   ptr->peakcntrl=0x00;
   ptr->peakcount=0x00;
   ptr->finegain=0x042e;  /* finegain*/

}


