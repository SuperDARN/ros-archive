/* out_tsg.c
   ========= */

/* 
 $Log: out_tsg_s.c,v $
 Revision 1.2  2000/02/03 12:53:23  barnes
 Attempt to fix problem with masking out unused bits in the
 timing sequence control word.

 Revision 1.1  1999/09/18 17:36:45  barnes
 Initial revision

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
#include "out_tsg_s.h"

int out_tsg_s(unsigned char *tsg_code_A,unsigned char *tsg_code_B,
            unsigned char *tsg_rep_A,unsigned char *tsg_rep_B,
  	        int tsg_length_A,int tsg_length_B,
            int tsg_delay_A,int tsg_delay_B,
            int TSG_safe_A,int TSG_safe_B, 
            int TSG_port_A,int TSG_port_B,
            int TSG_mask_A,int TSG_mask_B,
            int TSG_state_A,int TSG_state_B,
            int CLOCK_port,int CLOCK_mask) {

  register int port_A=TSG_port_A;
  register int port_B=TSG_port_B;
  register int pmask_A=TSG_mask_A;
  register int pmask_B=TSG_mask_B;
  register int pstate_A=TSG_state_A;
  register int pstate_B=TSG_state_B;

  register int clock=CLOCK_port;
  register int cmask=CLOCK_mask; 
  register unsigned int wrd;
 
  register int k_A=0;
  register int j_A=0;
  register int k_B=0;
  register int j_B=0;


  register int x=0;

  register int safe_A=TSG_safe_A;
  register int safe_B=TSG_safe_B;
 

  k_A=tsg_delay_A+1;
  k_B=tsg_delay_B+1;


  _disable();
  outp(port_A,(safe_A & pmask_A) | pstate_A); 
  outp(port_B,(safe_B & pmask_B) | pstate_B); 
 
  do {
    do { /* wait for the clock to go low before outputing pulse */
      wrd=inp(clock);
      x++;
    } while (((wrd & cmask) !=0) && (x<X_MAX));
    if (x==X_MAX) break;
    x=0;

    k_A--;
    if (k_A==0) {
      /* output next pulse */
      if (j_A !=tsg_length_A) {
         outp(port_A,(tsg_code_A[j_A] & pmask_A) | pstate_A);
         k_A=tsg_rep_A[j_A];
         j_A++;
      } else {
        k_A=65536; /* delay as long as possible */
        outp(port_A,(safe_A & pmask_A) | pstate_A);
      }
    }

    k_B--;
    if (k_B==0) {
      /* output next pulse */
      if (j_B !=tsg_length_B) {
         outp(port_B,(tsg_code_B[j_B] & pmask_B) | pstate_B);
         k_B=tsg_rep_B[j_B];
         j_B++;
      } else {
        k_B=65536; /* delay as long as possible */
        outp(port_B,(safe_B & pmask_B) | pstate_B);
      }
    }

    do { /* wait for the clock to go high */
      wrd=inp(clock);
      x++;
    } while (((wrd & cmask)==0) && (x<X_MAX));   
    if (x==X_MAX) break;
    x=0;

  } while ((j_A<tsg_length_A) || (j_B<tsg_length_B));
  _enable();
  if (x==X_MAX) return -1;
  return 0;
 
}



  
   

  
