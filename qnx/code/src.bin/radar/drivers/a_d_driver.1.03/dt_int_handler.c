/* dt_int_handler.c
   ================ */

/*
 $Log: dt_int_handler.c,v $
 Revision 1.5  1999/03/26 15:39:08  barnes
 Fixed bug that causes a kernel fault on newer versions of QNX.

 Revision 1.4  1999/03/25 21:13:44  barnes
 Removed dt.dec and dt.ext.

 Revision 1.3  1998/06/03 19:06:01  root
 Modification to the version control system.

 Revision 1.2  1998/05/27 09:48:05  root
 Modifications to allow the number of DMA buffers to be specified on the
 command line.

 Revision 1.1  1996/03/13 14:03:36  root
 Initial revision

*/

/* dt_int_handler contains the interrupt handler for the dt_int_adm task
*/

#include <stdio.h>
#include <sys/types.h>

#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"

#include "dt_ext.h"

#pragma off (check_stack)

pid_t __far dt_int_handler() {

	/* return proxy so that dt_int_adm can receive proxy */

	return(dt_proxy);	
}   