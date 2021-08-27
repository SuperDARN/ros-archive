/* default.c
   ========= */

/*
 $Log: default.c,v $
 Revision 1.6  2000/02/03 12:53:23  barnes
 Attempt to fix problem with masking out unused bits in the
 timing sequence control word.

 Revision 1.5  1999/09/18 17:36:32  barnes
 Modifications for stereo transmission.

 Revision 1.4  1999/04/13 14:08:34  barnes
 Fixed bug in setting the clock control words.

 Revision 1.3  1999/04/02 15:49:34  barnes
 Modifications to support Frequency switching.

 Revision 1.2  1998/11/08 19:11:06  barnes
 Modifications to support stereo operations.

 Revision 1.1  1998/11/07 00:19:16  barnes
 Initial revision

 */

#include <stdlib.h>
#include <stdio.h>

#include "port.h"

struct dio_hdw hdw;

void default_config() {
 
  strcpy(hdw.name,"Basic Configuration");
  hdw.BEAM_port[0]=0x04;
  hdw.Atten_port[0]=0x02;
  hdw.ANTENNA_port[0]=0x04;
  hdw.FILTER_port[0]=-1;
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

  hdw.TSG_mask[0]=0x7f;
  hdw.BEAM_mask[0]=0x0f;
  hdw.Atten_mask[0]=0x70;
  hdw.ANTENNA_mask[0]=0xf0;
  hdw.FILTER_mask[0]=0x80;
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
  hdw.FILTER_port[1]=-1;
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

  hdw.TSG_mask[1]=0x7f;
  hdw.BEAM_mask[1]=0;
  hdw.Atten_mask[1]=0;
  hdw.ANTENNA_mask[1]=0;
  hdw.FILTER_mask[1]=0;
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
  hdw.FREQ_inv=0;
  hdw.AGCstatus_inv=0;
  hdw.LOPWRstatus_inv=0;
  hdw.Local_Oscillator=40625;

}
