/* shell.c
   ======= */

/*
 $Log: shell.c,v $
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

 