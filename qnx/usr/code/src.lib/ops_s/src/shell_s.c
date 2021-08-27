/* shell_s.c
   ========= */

/*
 $Log: shell_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "limit.h"

#include "global_s.h"
#include "radar_shell.h"

void init_shell_s() {
  radar_shell_add(&rstable,"intt",var_LONG,&intt);
  radar_shell_add(&rstable,"cp",var_LONG,&cp);

  radar_shell_add(&rstable,"txpl_A",var_LONG,&txpl_A);
  radar_shell_add(&rstable,"mpinc_A",var_LONG,&mpinc_A);
  radar_shell_add(&rstable,"mppul_A",var_LONG,&mppul_A);
  radar_shell_add(&rstable,"mplgs_A",var_LONG,&mplgs_A);
  radar_shell_add(&rstable,"nrang_A",var_LONG,&nrang_A);
  radar_shell_add(&rstable,"frang_A",var_LONG,&frang_A);
  radar_shell_add(&rstable,"rsep_A",var_LONG,&rsep_A);
  radar_shell_add(&rstable,"bmnum_A",var_LONG,&bmnum_A);
  radar_shell_add(&rstable,"xcf_A",var_LONG,&xcf_A);
  radar_shell_add(&rstable,"tfreq_A",var_LONG,&tfreq_A);
  radar_shell_add(&rstable,"scan_A",var_LONG,&scan_A);
  radar_shell_add(&rstable,"mxpwr_A",var_LONG,&mxpwr_A);
  radar_shell_add(&rstable,"lvmax_A",var_LONG,&lvmax_A);
  radar_shell_add(&rstable,"rxrise_A",var_LONG,&rxrise_A);
  radar_shell_add(&rstable,"combf_A",var_STRING,combf_A);

  radar_shell_add(&rstable,"txpl_B",var_LONG,&txpl_B);
  radar_shell_add(&rstable,"mpinc_B",var_LONG,&mpinc_B);
  radar_shell_add(&rstable,"mppul_B",var_LONG,&mppul_B);
  radar_shell_add(&rstable,"mplgs_B",var_LONG,&mplgs_B);
  radar_shell_add(&rstable,"nrang_B",var_LONG,&nrang_B);
  radar_shell_add(&rstable,"frang_B",var_LONG,&frang_B);
  radar_shell_add(&rstable,"rsep_B",var_LONG,&rsep_B);
  radar_shell_add(&rstable,"bmnum_B",var_LONG,&bmnum_B);
  radar_shell_add(&rstable,"xcf_B",var_LONG,&xcf_B);
  radar_shell_add(&rstable,"tfreq_B",var_LONG,&tfreq_B);
  radar_shell_add(&rstable,"scan_B",var_LONG,&scan_B);
  radar_shell_add(&rstable,"mxpwr_B",var_LONG,&mxpwr_B);
  radar_shell_add(&rstable,"lvmax_B",var_LONG,&lvmax_B);
  radar_shell_add(&rstable,"rxrise_B",var_LONG,&rxrise_B);
  radar_shell_add(&rstable,"combf_B",var_STRING,combf_B);


 };

 
