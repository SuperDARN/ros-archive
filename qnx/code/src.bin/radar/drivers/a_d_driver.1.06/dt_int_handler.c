/* dt_int_handler.c
   ================
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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

/*
 $Log: dt_int_handler.c,v $
 Revision 1.7  2001/06/27 20:55:33  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:12:05  barnes
 Added Author Name

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