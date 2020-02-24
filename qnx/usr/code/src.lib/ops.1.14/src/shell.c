/* shell.c
   ======= */

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
 $Log: shell.c,v $
 Revision 1.4  2001/06/27 21:10:39  barnes
 Added license tag

 Revision 1.3  1999/09/10 00:19:29  barnes
 Moved the location of the shell structure.

 Revision 1.2  1999/09/10 00:08:57  barnes
 Changed the way the shell table is created.

 Revision 1.1  1999/03/31 19:58:35  barnes
 Initial revision

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "limit.h"

#include "global.h"
#include "radar_shell.h"

void init_shell() {
  radar_shell_add(&rstable,"intt",var_LONG,&intt);
  radar_shell_add(&rstable,"txpl",var_LONG,&txpl);
  radar_shell_add(&rstable,"mpinc",var_LONG,&mpinc);
  radar_shell_add(&rstable,"mppul",var_LONG,&mppul);
  radar_shell_add(&rstable,"mplgs",var_LONG,&mplgs);
  radar_shell_add(&rstable,"nrang",var_LONG,&nrang);
  radar_shell_add(&rstable,"frang",var_LONG,&frang);
  radar_shell_add(&rstable,"rsep",var_LONG,&rsep);
  radar_shell_add(&rstable,"bmnum",var_LONG,&bmnum);
  radar_shell_add(&rstable,"xcf",var_LONG,&xcf);
  radar_shell_add(&rstable,"tfreq",var_LONG,&tfreq);
  radar_shell_add(&rstable,"scan",var_LONG,&scan);
  radar_shell_add(&rstable,"mxpwr",var_LONG,&mxpwr);
  radar_shell_add(&rstable,"lvmax",var_LONG,&lvmax);
  radar_shell_add(&rstable,"rxrise",var_LONG,&rxrise);
  radar_shell_add(&rstable,"cp",var_LONG,&cp);
  radar_shell_add(&rstable,"combf",var_STRING,combf);
 };

 