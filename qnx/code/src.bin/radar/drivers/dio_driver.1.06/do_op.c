/* do_op.c
   ======= */

/* 
 $Log: do_op.c,v $
 Revision 1.22  2000/11/14 16:02:05  barnes
 Numerouse bug fixes to ensure the the local/remote condition of the
 synthesizer is correctly reported.

 Revision 1.21  1999/08/09 01:52:18  barnes
 Fixed bug in the low power and agc status flags.

 Revision 1.20  1999/04/02 16:17:20  barnes
 Changed logic for local/remote frequency detect.

 Revision 1.19  1998/11/08 19:11:06  barnes
 Modifications to support stereo operations.

 Revision 1.18  1998/11/07 00:19:16  barnes
 Extensive changes for stereo operations.
 Moved all of the hard coded configuration information into a configuration
 file.

 Revision 1.17  1998/11/06 01:19:57  barnes
 Implemented the majority of the Stereo Radar code. The remaining sections
 involve selecting the correct PIO port based on the channel number and the
 implementation of the channel switching timing sequence.

 Revision 1.16  1998/11/04 15:54:25  barnes
 ttFixed bug in decoding result of forbidden frequency test.

 Revision 1.15  1998/10/23 17:39:58  barnes
 Modifications to support the extended timing sequence.

 Revision 1.14  1998/09/11 17:33:25  root
 Modifications to remove short integer types.

 Revision 1.13  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.12  1997/08/19 15:33:13  root
 Julian Thornhill's fixes to the filter code.

 Revision 1.11  1997/08/12 18:10:12  root
 Modified code for halley as they now use the leicester Rx design.

 Revision 1.10  1997/08/12 17:54:08  root
 Added #if to mask out filter switching on sites that don't have it.

 Revision 1.9  1997/08/07 16:12:48  root
 Added code for filter switch

 Revision 1.8  1997/06/10 20:17:13  root
 Moved sd_st_id.h header.

 Revision 1.7  1997/02/19 17:07:40  root
 Fixed bug in Sanae code.

 * Revision 1.6  1997/02/19  17:05:45  root
 * Added code for Sanae
 *
 * Revision 1.5  1997/01/28  20:48:32  root
 * Added code for forbidden frequency checks.
 *
 * Revision 1.4  1996/09/23  14:58:04  root
 * Added code for syowa radars.
 *
 * Revision 1.3  1996/03/26  19:47:30  root
 * Added code for fclr
 *
 * Revision 1.2  1996/03/18  11:57:57  root
 * Removed #ifdef LEICESTER directive and replaced with SD_st_id
 * form.
 *
 * Revision 1.1  1996/03/13  15:13:31  root
 * Initial revision
 *
*/

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>


#include "freq_lib.h"
#include "DIO.h"
#include "port.h"
#include "main.h"
#include "cnv_freq.h"

#include "dio_msg.h"

extern struct dio_hdw hdw;
extern struct dio_param param;
extern struct freq_table *fptr;
extern int debug;

int put_filter(int channel,unsigned int filter) { /* set the filter bit */

  if (hdw.FILTER_port[channel]<0) return 0;
  if (filter !=0) PUT_DIO(hdw.FILTER_port[channel],
			              hdw.FILTER_mask[channel],hdw.FILTER_mask[channel]); 
  else PUT_DIO(hdw.FILTER_port[channel],0,hdw.FILTER_mask[channel]);
  return 0; 
}

int put_beam(int channel,unsigned char bmnum) {
  if (bmnum>15) return -1;
  if (hdw.BEAM_port[channel]<0) return 0;
  if (hdw.BEAM_inv !=0) 
    PUT_DIO(hdw.BEAM_port[channel], bmnum^0x0f, hdw.BEAM_mask[channel]);  
  else
    PUT_DIO(hdw.BEAM_port[channel], bmnum, hdw.BEAM_mask[channel]);	
  return 0;
}

int put_antenna(int channel,unsigned char ant) {

  if (hdw.ANTENNA_port[channel]<0) return 0;
  PUT_DIO(hdw.ANTENNA_port[channel], ant<<4, hdw.ANTENNA_mask[channel]);
  return 0;
}

int put_Hz(int channel,int freq) {
  int i;
  unsigned char btable[3];
  if ((hdw.FMHz10_port[channel]<0) ||
     (hdw.FMKHz_port[channel]<0) || 
     (hdw.FKHz_port[channel]<0)) return 0;
  if ((freq < 8000) || (freq > 20000)) return -1;	/* check frequency */
    
  if ((fptr !=0) && (test_freq(fptr,freq))!=0) return -1;  
  cnv_freq(freq,btable);
  PUT_DIO(hdw.FMHz10_port[channel],btable[0],hdw.FMHz10_mask[channel]);
  PUT_DIO(hdw.FMKHz_port[channel],btable[1],hdw.FMKHz_mask[channel]);
  PUT_DIO(hdw.FKHz_port[channel],btable[2],hdw.FKHz_mask[channel]);

  return get_freq_mode(channel);
}

int get_freq_mode(int channel) {
 int i;
  if (hdw.Fstatus_port[channel]<0) return 0;
 
  if (debug==1) return 0;
  if (hdw.FSTAT_inv==0) {
    if ((GET_DIO(hdw.Fstatus_port[channel]) & hdw.Fstatus_bit[channel])) 
      return 0; /* LOCAL_FREQ warning */
    return LOCAL_FREQ;
  } else {
    if ((GET_DIO(hdw.Fstatus_port[channel]) & hdw.Fstatus_bit[channel])) 
      return LOCAL_FREQ; /* LOCAL_FREQ warning */
    return 0;
  }
  return 0;

}


int put_gain(int channel,unsigned char atten) {
  unsigned char atten_byte;
  int i;
  if (hdw.Atten_port[channel]<0) return 0;
  if (hdw.Atten_max > 3) {
    if (atten > hdw.Atten_max) return -1;
    atten_byte = atten;
    atten_byte = atten_byte << 4;	/* shift the bits to the upper 4 bits */
    PUT_DIO(hdw.Atten_port[channel], atten_byte, hdw.Atten_mask[channel]);
  } else {  
    if (atten > hdw.Atten_max) return -1;
    atten_byte = 0;
    for (i=1; i<= atten; i++) atten_byte = (atten_byte << 1) | 1; 
    atten_byte = atten_byte << 4;	/* shift the bits to the upper 4 bits */
    PUT_DIO(hdw.Atten_port[channel], atten_byte, hdw.Atten_mask[channel]);
  }  
  return 0;
}

int put_test_mode(int channel,unsigned char mode) {

  if (hdw.TEST_port[channel]<0) return 0;
  if (mode !=0) PUT_DIO(hdw.TEST_port[channel], hdw.TEST_mask[channel],
						hdw.TEST_mask[channel]);
  else PUT_DIO(hdw.TEST_port[channel], 0, hdw.TEST_mask[channel]);
  return 0;
}

int read_agcstatus(int channel) {

  int i, agcstat;

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

  



