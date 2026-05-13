/* out_tsg_x.c
   ===========
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

#include <i86.h>
#include <stdlib.h>
#include <conio.h>
#include "out_tsg_x.h"

/* 
 $Log: out_tsg_x.c,v $
 Revision 1.2  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

 */

int out_tsg_x(unsigned char *tsg_xtd,
	      unsigned char *tsg_code,unsigned char *tsg_rep,
	      int tsg_length,unsigned char flag,unsigned char *fptr,
              int TSG_safe,int TSG_port,int TSG_mask,int TSG_state,
              int CLOCK_port,int CLOCK_mask,
	      int BEAM_port,int BEAM_mask,int BEAM_state,int *fadr) {

  register int f10Mhz=fadr[2];
  register int fMKhz=fadr[1];
  register int fKhz=fadr[0];
  register int beam=BEAM_port;
  register int beam_mask=BEAM_mask;
  register int beam_state=BEAM_state;

  register int port=TSG_port;
  register int pmask=TSG_mask;
  register int pstate=TSG_state;
  register int clock=CLOCK_port;
  register int cmask=CLOCK_mask; 
  register unsigned int wrd;
  register int ofrq=-1;

  register int k=0;
  register int j=0;
  register int x=0;

  register int safe=TSG_safe;
  int delay=0;

  k=delay+1;
  _disable();
  outp(port,(safe & pmask) | pstate); /* put the port into "safe" mode.  */
  
  if (flag & 0x02) {
    ofrq=tsg_xtd[0] & 0x10; 
    outp(f10Mhz,fptr[0+(ofrq !=0)*3]);
    outp(fMKhz,fptr[1+(ofrq !=0)*3]);
    outp(fKhz,fptr[2+(ofrq !=0)*3]);
  }

  do {
    do { /* wait for the clock to go low before outputing pulse */
      wrd=inp(clock);
      x++;
    } while (((wrd & cmask) !=0) && (x<X_MAX));
    if (x==X_MAX) break;
    x=0;

    k--;
    if (k==0) {
      /* output next pulse */
      if (j !=tsg_length) {

         if ((flag & 0x02) && ((tsg_xtd[j] & 0x10) != ofrq)) {
           ofrq=tsg_xtd[j] & 0x10; 
           outp(f10Mhz,fptr[0+(ofrq !=0)*3]);
           outp(fMKhz,fptr[1+(ofrq !=0)*3]);
           outp(fKhz,fptr[2+(ofrq !=0)*3]);
         }
         if (flag & 0x01) outp(beam,(tsg_xtd[j] & beam_mask) | beam_state);
         outp(port,(tsg_code[j] & pmask) | pstate);

         k=tsg_rep[j];
         j++;
      } else {
        k=65536; /* delay as long as possible */
        outp(port,(safe & pmask) | pstate);
      }
    }

    do { /* wait for the clock to go high */
      wrd=inp(clock);
      x++;
    } while (((wrd & cmask)==0) && (x<X_MAX));   
    if (x==X_MAX) break;
    x=0;

  } while (j<tsg_length);
  _enable();
  if (x==X_MAX) return -1;
  return 0;
 
}



  
   

  
