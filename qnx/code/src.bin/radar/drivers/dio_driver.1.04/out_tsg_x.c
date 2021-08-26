/* out_tsg.c
   ========= */

/* 
 $Log: out_tsg_x.c,v $
 Revision 1.8  2000/02/03 12:53:23  barnes
 Attempt to fix problem with masking out unused bits in the
 timing sequence control word.

 Revision 1.7  1999/09/18 18:59:13  barnes
 Fixed bug in the frequency switching: if masks out the frequency was
 still set at the start of the sequence.

 Revision 1.6  1999/09/18 17:36:45  barnes
 Modifications for stereo.

 Revision 1.5  1999/04/14 14:58:50  barnes
 Modifications to prevent deadlock if we lose the clock pulse.

 Revision 1.4  1999/04/02 15:49:34  barnes
 Modifications to support Frequency switching.

 Revision 1.3  1999/02/24 16:47:24  barnes
 Fixed bug in outputing the timing sequence code.

 Revision 1.2  1998/11/07 00:19:16  barnes
 Extensive changes for stereo operations.
 Moved all of the hard coded configuration information into a configuration
 file.

 Revision 1.1  1998/10/23 17:41:28  barnes
 Initial revision

 */

#include <i86.h>
#include <stdlib.h>
#include "out_tsg_x.h"

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



  
   

  
