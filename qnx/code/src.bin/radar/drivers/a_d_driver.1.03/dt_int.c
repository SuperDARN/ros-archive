/* dt_int.c
   ======== */

/* 
 $Log: dt_int.c,v $
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

#include <stdio.h>
#include <sys/irqinfo.h>
#include <sys/name.h>
#include <sys/proxy.h>
#include <stdlib.h>
#include <sys/types.h>
#include <i86.h>

#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"

#include "dt_ext.h"

extern pid_t dt_int_handler(); /* interrupt handler */

int dt_int(int irq) {

  dt_proxy = qnx_proxy_attach(0,0,0,-1);

  if(dt_proxy == -1) return -1;
	
  dt_hint_id = qnx_hint_attach(irq,&dt_int_handler,
	               FP_SEG(&dt_proxy)); 
  
  if(dt_hint_id == -1) return -1;
  return 0;

}  
	
