/* do_op.c
   =======
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
#include <errno.h>
#include <stdio.h>
#include <conio.h>

#include "freq.h"
#include "DIO.h"
#include "port.h"
#include "main.h"
#include "cnv_freq.h"

#include "dio_msg.h"

/* 
 $Log: do_op.c,v $
 Revision 1.5  2007/11/09 21:56:34  code
 Added extra wide filter.

 Revision 1.4  2004/11/12 15:13:13  barnes
 Modifications to handle digital phased radars.

 Revision 1.3  2004/06/22 14:56:40  barnes
 Added the majority of Sessai's modifications.

 Revision 1.2  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

*/

extern struct dio_hdw hdw;
extern struct dio_param param;
extern struct FreqTable *fptr;
extern unsigned char debug;

int put_filter(int channel,unsigned int filter) { /* set the filter bit */
  int hi,lo;
  if (debug==1) return 0;
  if (hdw.FILTER_port[channel]<0) return 0;

  lo=((filter & 0x01) ? hdw.FILTER_mask[channel] : 0);
  
  PUT_DIO(hdw.FILTER_port[channel],lo,hdw.FILTER_mask[channel]);

  if (hdw.WIDEFILTER_port[channel] !=-1) {

    hi=((filter & 0x02) ? hdw.WIDEFILTER_mask[channel] : 0);
    PUT_DIO(hdw.WIDEFILTER_port[channel],hi,hdw.WIDEFILTER_mask[channel]);
  }
  
  return 0; 
}

int put_beam(int channel,unsigned char bmnum) {
  if (debug==1) return 0;
  if (bmnum>15) return -1;
  if (hdw.BEAM_port[channel]<0) return 0;
  if (hdw.BEAM_inv !=0) 
    PUT_DIO(hdw.BEAM_port[channel], bmnum^0x0f, hdw.BEAM_mask[channel]);  
  else
    PUT_DIO(hdw.BEAM_port[channel], bmnum, hdw.BEAM_mask[channel]);	
  return 0;
}

int put_antenna(int channel,unsigned char ant) {
  if (debug==1) return 0;
  if (hdw.ANTENNA_port[channel]<0) return 0;
  PUT_DIO(hdw.ANTENNA_port[channel], ant<<4, hdw.ANTENNA_mask[channel]);
  return 0;
}

int put_phase(int channel,unsigned char phase) {
  if (debug==1) return 0;
  if (hdw.PHASE_port[channel]<0) return 0;
  PUT_DIO(hdw.PHASE_port[channel], phase, hdw.PHASE_mask[channel]);
  return 0;
}

int put_Hz(int channel,int freq) {
 
  unsigned char btable[3];
  if (debug==1) return 0;
  if ((hdw.FMHz10_port[channel]<0) ||
     (hdw.FMKHz_port[channel]<0) || 
     (hdw.FKHz_port[channel]<0)) return 0;
  if ((freq < 8000) || (freq > 20000)) return -1; /* check frequency */
  if ((fptr !=0) && (FreqTest(fptr,freq)!=0)) return -1;
  cnv_freq(freq,btable);
  PUT_DIO(hdw.FMHz10_port[channel],btable[0],hdw.FMHz10_mask[channel]);
  PUT_DIO(hdw.FMKHz_port[channel],btable[1],hdw.FMKHz_mask[channel]);
  PUT_DIO(hdw.FKHz_port[channel],btable[2],hdw.FKHz_mask[channel]);

  return get_freq_mode(channel);
}

int get_freq_mode(int channel) {
 
 if (debug==1) return 0;
  if (hdw.Fstatus_port[channel]<0) return 0;
  if (hdw.FSTAT_inv==0) {
    if ((GET_DIO(hdw.Fstatus_port[channel]) & hdw.Fstatus_bit[channel])) 
      return 0; /* LOCAL_FREQ warning */
    return LOCAL_FREQ;
  } else {
    if ((GET_DIO(hdw.Fstatus_port[channel]) & hdw.Fstatus_bit[channel])) 
      return LOCAL_FREQ; /* LOCAL_FREQ warning */
    return 0;
  }
}


int put_gain(int channel,unsigned char atten) {
  unsigned char atten_byte;
  int i;
  if (debug==1) return 0;
  if (hdw.Atten_port[channel]<0) return 0;
  if (hdw.Atten_max > 3) {
    if (atten > hdw.Atten_max) return -1;
    atten_byte = atten;
    atten_byte = atten_byte << 4; /* shift the bits to the upper 4 bits */
    PUT_DIO(hdw.Atten_port[channel], atten_byte, hdw.Atten_mask[channel]);
  } else {  
    if (atten > hdw.Atten_max) return -1;
    atten_byte = 0;
    for (i=1; i<= atten; i++) atten_byte = (atten_byte << 1) | 1; 
    atten_byte = atten_byte << 4; /* shift the bits to the upper 4 bits */
    PUT_DIO(hdw.Atten_port[channel], atten_byte, hdw.Atten_mask[channel]);
  }  
  return 0;
}

int put_test_mode(int channel,unsigned char mode) {
  if (debug==1) return 0;
  if (hdw.TEST_port[channel]<0) return 0;
  if (mode !=0) PUT_DIO(hdw.TEST_port[channel], hdw.TEST_mask[channel],
						hdw.TEST_mask[channel]);
  else PUT_DIO(hdw.TEST_port[channel], 0, hdw.TEST_mask[channel]);
  return 0;
}

int read_agcstatus(int channel) {

  int i, agcstat;
  if (debug==1) return 0;
  if (hdw.AGCstatus_port[channel]<0) agcstat =0;
  agcstat = GET_DIO(hdw.AGCstatus_port[channel]) & 
			        hdw.AGCstatus_mask[channel];

  i = hdw.AGCstatus_mask[channel];
  while (i > 1) {
    i = i >> 1;		/* shift the mask and the status bit right until */
    agcstat = agcstat >> 1;  /* you get to bit 0 */
  }
  if (hdw.AGCstatus_inv !=0) agcstat=agcstat ^ 0x01;
  return agcstat;
}

int read_lopwrstatus(int channel) {
  int i, lopwr;
  if (debug==1) return 0;
  if (hdw.LOPWRstatus_port[channel]<0) lopwr =0;
  lopwr = GET_DIO(hdw.LOPWRstatus_port[channel]) & 
		          hdw.LOPWRstatus_mask[channel];

  i = hdw.LOPWRstatus_mask[channel];
  while (i > 1) {
    i = i >> 1;		/* shift the mask and the status bit right until */
	lopwr = lopwr >> 1;  /* you get to bit 0 */
  }
  if (hdw.LOPWRstatus_inv !=0) lopwr=lopwr ^ 0x01;
  return lopwr;
}

  



