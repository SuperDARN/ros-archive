/* dt_reset.c
   ========== */

/* 
 $Log: dt_reset.c,v $
 Revision 1.5  1999/03/25 21:13:44  barnes
 Removed dt.dec and dt.ext.

 Revision 1.4  1998/11/06 15:21:16  barnes
 Added the ability to specify the card address, IRQ, and DMA channel
 on the command line.

 Revision 1.3  1998/06/03 19:06:01  root
 Modification to the version control system.

 Revision 1.2  1998/05/27 09:48:05  root
 Modifications to allow the number of DMA buffers to be specified on the
 command line.

 Revision 1.1  1996/03/13 14:03:36  root
 Initial revision

*/

#include <sys/types.h>

#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"
#include "dt_ext.h"
#include "dt2828.h"

extern int ad_port;

void dt_reset() {
   dt_supcsr.word = 0;
   dt_supcsr.bit.bdinit = ON;
   dt_set_register(ad_port+SUPCSR_PORT,&dt_supcsr);

} 

