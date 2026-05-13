/* gc4016.c 
   ======== 
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
#include "gc4016.h"


/*
 $Log: gc4016.c,v $
 Revision 1.1  2005/07/25 15:12:45  barnes
 Initial revision

*/

void setupGC4016(long baseGC214,struct GC4016Global *ptr) {
  
  write8(baseGC214+offset_GC4016+reg_GC4016_GENSYNC,0x00);
  write8(baseGC214+offset_GC4016+reg_GC4016_GLOBALRST,0xF8);
  write8(baseGC214+offset_GC4016+reg_GC4016_NCHANOUT,ptr->nchanout);
  write8(baseGC214+offset_GC4016+reg_GC4016_MISC,0x02);
  write8(baseGC214+offset_GC4016+reg_GC4016_NMULT,ptr->nmult);
  write8(baseGC214+offset_GC4016+reg_GC4016_FILTSLCT,ptr->filtslct);
  write8(baseGC214+offset_GC4016+reg_GC4016_FINALSHFT,ptr->finalshft);
  write8(baseGC214+offset_GC4016+reg_GC4016_CHANMAP,ptr->chanmap);
  write8(baseGC214+offset_GC4016+reg_GC4016_ADDTO,ptr->addto);
  write8(baseGC214+offset_GC4016+reg_GC4016_RESAMPCLKDVD,ptr->resampclkdvd);
  write8(baseGC214+offset_GC4016+reg_GC4016_RATIOMAP,ptr->ratiomap);
  write32(baseGC214+offset_GC4016+reg_GC4016_RATIO0,ptr->ratio0);
  write32(baseGC214+offset_GC4016+reg_GC4016_RATIO1,ptr->ratio1);
  write32(baseGC214+offset_GC4016+reg_GC4016_RATIO2,ptr->ratio2);
  write32(baseGC214+offset_GC4016+reg_GC4016_RATIO3,ptr->ratio3);
  write8(baseGC214+offset_GC4016+reg_GC4016_GENSYNC,0x00);
  write8(baseGC214+offset_GC4016+reg_GC4016_CNTSYNC,0x00);
  write8(baseGC214+offset_GC4016+reg_GC4016_CNTBYTE0,ptr->cntbyte0);
  write8(baseGC214+offset_GC4016+reg_GC4016_CNTBYTE1,ptr->cntbyte1);
  write8(baseGC214+offset_GC4016+reg_GC4016_TRICNTRL,ptr->tricntrl);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTFORMAT,ptr->outformat);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTMODE,ptr->outmode);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTFRAMECNTRL,ptr->outframecntrl);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTWDSIZE,ptr->outwdsize);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTCLKCNTRL,ptr->outclkcntrl);
  write8(baseGC214+offset_GC4016+reg_GC4016_SERMUXCNTRL,ptr->sermuxcntrl);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTTAGA,ptr->outtaga);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTTAGB,ptr->outtagb);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTTAGC,ptr->outtagc);
  write8(baseGC214+offset_GC4016+reg_GC4016_OUTTAGD,ptr->outtagd);
}

void releaseGC4016(long baseGC214,int *resampcoeffs) {
  int i;
  write8(baseGC214+offset_GC4016+reg_GC4016_GLOBALRST,0x08);
  for(i=0;i<256;i++){
    write16(baseGC214+offset_GC4016+reg_GC4016_RESAMPCOEFFS+2*i,
            resampcoeffs[i]);
  }
}

void setupGC4016channel(long baseGC214,int channel,
                        struct GC4016Channel *ptr,int *cfircoeffs,
                        int *pfircoeffs) {
  int i;
  int offset=0;
  
  if (channel==0) offset=offset_GC4016_A;
  if (channel==1) offset=offset_GC4016_B;
  if (channel==2) offset=offset_GC4016_C;
  if (channel==3) offset=offset_GC4016_D;
  write16(baseGC214+offset_GC4016+offset+reg_GC4016_PHASE,ptr->phase);
  write32(baseGC214+offset_GC4016+offset+reg_GC4016_FREQ,ptr->freq);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_CHRESET,ptr->chreset);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_FREQSYNC,0x77);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_NCOSYNC,0x22);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_ZEROPAD,0x00);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_DECSYNC,0x22);
  write16(baseGC214+offset_GC4016+offset+reg_GC4016_DECRATIO,ptr->decratio);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_CICSCALE,ptr->cicscale);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_SPLITIQ,ptr->splitiq);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_CFIR,ptr->cfir);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_PFIR,ptr->pfir);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_INPUT,ptr->input);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_PEAKCNTRL,ptr->peakcntrl);
  write8(baseGC214+offset_GC4016+offset+reg_GC4016_PEAKCOUNT,ptr->peakcount);
  write16(baseGC214+offset_GC4016+offset+reg_GC4016_FINEGAIN,ptr->finegain);

  for(i=0;i<11;i++){
    write16(baseGC214+offset_GC4016+offset+reg_GC4016_CFIRCOEFFS+2*i,
            cfircoeffs[i]);
  }
  for(i=0;i<32;i++){
    write16(baseGC214+offset_GC4016+offset+reg_GC4016_PFIRCOEFFS+2*i,
            pfircoeffs[i]);
  }	
}








