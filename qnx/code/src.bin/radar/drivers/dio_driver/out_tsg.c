/* out_tsg.c
   ========= */

/* 
 $Log: out_tsg.c,v $
 Revision 1.7  2000/02/03 12:53:23  barnes
 Attempt to fix problem with masking out unused bits in the
 timing sequence control word.

 Revision 1.6  1999/09/18 17:36:32  barnes
 Modifications for stereo transmission.

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
#include "out_tsg.h"

int out_tsg(unsigned char *tsg_code,unsigned char *tsg_rep,
	    int tsg_length,int TSG_safe,
            int TSG_port,int TSG_mask,int TSG_state,
            int CLOCK_port,int CLOCK_mask) {

  register int port=TSG_port;
  register int pmask=TSG_mask;
  register int pstate=TSG_state;
  register int clock=CLOCK_port;
  register int cmask=CLOCK_mask; 
  register unsigned int wrd;
 
  register int k=0;
  register int j=0;
  register int x=0;

  register int safe=TSG_safe;
  int delay=0;

  k=delay+1;
  _disable();
  outp(port,(safe & pmask) | pstate); 

  /* put the port into "safe" mode.  */
  
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



  
   

  