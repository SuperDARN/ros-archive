/* shell_s.c
   ========= */

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
 $Log: shell_s.c,v $
 Revision 1.3  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.2  2000/11/13 20:08:29  barnes
 Allowed for separate control program numbers for each channel.

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
  radar_shell_add(&rstable,"cp_A",var_LONG,&cp_A);

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
  radar_shell_add(&rstable,"cp_A",var_LONG,&cp_B);


 };

 
