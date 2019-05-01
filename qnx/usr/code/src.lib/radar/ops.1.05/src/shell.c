/* shell.c
   ======= 
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "rmsg.h"
#include "radarshell.h"
#include "global.h"

/*
 $Log: shell.c,v $
 Revision 1.1  2004/05/06 20:10:19  barnes
 Initial revision

*/

void OpsSetupShell() {
  RadarShellAdd(&rstable,"intsc",var_LONG,&intsc);
  RadarShellAdd(&rstable,"intus",var_LONG,&intus);
  RadarShellAdd(&rstable,"txpl",var_LONG,&txpl);
  RadarShellAdd(&rstable,"mpinc",var_LONG,&mpinc);
  RadarShellAdd(&rstable,"mppul",var_LONG,&mppul);
  RadarShellAdd(&rstable,"mplgs",var_LONG,&mplgs);
  RadarShellAdd(&rstable,"nrang",var_LONG,&nrang);
  RadarShellAdd(&rstable,"frang",var_LONG,&frang);
  RadarShellAdd(&rstable,"rsep",var_LONG,&rsep);
  RadarShellAdd(&rstable,"bmnum",var_LONG,&bmnum);
  RadarShellAdd(&rstable,"xcf",var_LONG,&xcf);
  RadarShellAdd(&rstable,"tfreq",var_LONG,&tfreq);
  RadarShellAdd(&rstable,"scan",var_LONG,&scan);
  RadarShellAdd(&rstable,"mxpwr",var_LONG,&mxpwr);
  RadarShellAdd(&rstable,"lvmax",var_LONG,&lvmax);
  RadarShellAdd(&rstable,"rxrise",var_LONG,&rxrise);
  RadarShellAdd(&rstable,"cp",var_LONG,&cp);
  RadarShellAdd(&rstable,"combf",var_STRING,combf);
 };

 
