/* default.c
   =========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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
#include <string.h>
#include "port.h"

/*
 $Log: default.c,v $
 Revision 1.5  2007/11/09 21:56:34  code
 Added extra wide filter.

 Revision 1.4  2005/06/16 18:37:02  barnes
 Added code to support inverting of the TX pulse.

 Revision 1.3  2004/11/12 15:13:13  barnes
 Modifications to handle digital phased radars.

 Revision 1.2  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

 */


struct dio_hdw hdw;

void default_config() {
 
  strcpy(hdw.name,"Basic Configuration");
  hdw.BEAM_port[0]=0x04;
  hdw.Atten_port[0]=0x02;
  hdw.ANTENNA_port[0]=0x04;
  hdw.PHASE_port[0]=0x04;
  hdw.FILTER_port[0]=-1;
  hdw.WIDEFILTER_port[0]=-1;
  hdw.TSG_port[0]=0x05;
  hdw.TSG_safe[0]=0x00;
  hdw.CLOCK_port[0]=0x06;
  hdw.TEST_port[0]=0x06;
  hdw.AGCstatus_port[0]=0x06;
  hdw.LOPWRstatus_port[0]=0x06;
  hdw.Fstatus_port[0]=0x06;	
  hdw.FMHz10_port[0]=0x02;
  hdw.FMKHz_port[0]=0x01;
  hdw.FKHz_port[0]=0x00;
  hdw.micro_clock[0]=0x08;
  hdw.micro_clock_cw[0]=0x0b;
  hdw.one_shot[0]=0x09;
  hdw.one_shot_cw[0]=0x0b;

  hdw.TSG_mask[0]=0xff;
  hdw.BEAM_mask[0]=0x0f;
  hdw.Atten_mask[0]=0x70;
  hdw.ANTENNA_mask[0]=0xf0;
  hdw.PHASE_mask[0]=0xff;
  hdw.FILTER_mask[0]=0x80;
  hdw.WIDEFILTER_mask[0]=0x80;
  hdw.CLOCK_mask[0]=0x01;
  hdw.TEST_mask[0]=0x10;
  hdw.AGCstatus_mask[0]=0x02;
  hdw.LOPWRstatus_mask[0]=0x04;
  hdw.Fstatus_bit[0]=0x08; 
  hdw.FMHz10_mask[0]=0x0f;
  hdw.FMKHz_mask[0]=0xff;
  hdw.FKHz_mask[0]=0xff;

  hdw.BEAM_port[1]=-1;
  hdw.Atten_port[1]=-1;
  hdw.ANTENNA_port[1]=-1;
  hdw.PHASE_port[1]=-1;
  hdw.FILTER_port[1]=-1;
  hdw.WIDEFILTER_port[0]=-1;
  hdw.TSG_port[1]=-1;
  hdw.TSG_safe[1]=-1;
  hdw.CLOCK_port[1]=-1;
  hdw.TEST_port[1]=-1;
  hdw.AGCstatus_port[1]=-1;
  hdw.LOPWRstatus_port[1]=-1;
  hdw.Fstatus_port[1]=-1;	
  hdw.FMHz10_port[1]=-1;
  hdw.FMKHz_port[1]=-1;
  hdw.FKHz_port[1]=-1;
  hdw.micro_clock[1]=-1;
  hdw.micro_clock_cw[1]=-1;
  hdw.one_shot[1]=-1;
  hdw.one_shot_cw[1]=-1;

  hdw.TSG_mask[1]=0xff;
  hdw.BEAM_mask[1]=0;
  hdw.Atten_mask[1]=0;
  hdw.ANTENNA_mask[1]=0;
  hdw.PHASE_mask[1]=0;
  hdw.FILTER_mask[1]=0;
  hdw.WIDEFILTER_mask[1]=0;
  hdw.CLOCK_mask[1]=0;
  hdw.TEST_mask[1]=0;
  hdw.AGCstatus_mask[1]=0;
  hdw.LOPWRstatus_mask[1]=0;
  hdw.Fstatus_bit[1]=0; 
  hdw.FMHz10_mask[1]=0;
  hdw.FMKHz_mask[1]=0;
  hdw.FKHz_mask[1]=0;

  hdw.control_word[0]=0x80;
  hdw.control_word[1]=0x81;
  hdw.control_word[2]=0x00;
  hdw.control_word[3]=0x00;

  hdw.Atten_max=3;
  hdw.BEAM_inv=0;
  hdw.TX_inv=0;
  hdw.FREQ_inv=0;
  hdw.FSTAT_inv=0;
  hdw.AGCstatus_inv=0;
  hdw.LOPWRstatus_inv=0;
  hdw.Local_Oscillator=40625;

}
