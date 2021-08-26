/* dt_etrig.c
   ========== */

/*
 $Log: dt_etrig.c,v $
 Revision 1.5  1999/03/25 21:13:44  barnes
 Removed dt.dec and dt.ext.

 Revision 1.4  1998/11/06 15:21:16  barnes
 Added the ability to specify the card address, IRQ, and DMA channel
 on the command line.

 Revision 1.3  1998/06/03 19:06:01  root
 Modification to the version control system.

 Revision 1.2  1998/05/27 09:48:05  root
 Modifications to allow the number of DMA buffers to be specified on the
 command line.

 Revision 1.1  1996/03/13 14:03:36  root
 Initial revision

*/

/* dt_etrig_scan  initializes the dt2828 a/d converter card to do triggered 
   scan dma using internal clock
*/

#include <stdio.h>
#include <sys/types.h>

#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"
#include "dt_ext.h"
#include "dt2828.h"

extern int ad_port;

dt_etrig_scan(unsigned short int num_channel) {

   unsigned short int chan;            /* channel number */

   dt_supcsr.word = 0;
   dt_supcsr.bit.clrdmadne = ON;
   dt_supcsr.bit.buffb = ON;
   dt_supcsr.bit.adcinit = ON;
   dt_set_register(ad_port+SUPCSR_PORT,&dt_supcsr);
      
   dt_chancsr.word = 0;
   dt_chancsr.bit.lle =  ON;
   dt_chancsr.bit.numb = num_channel - 1;
   dt_set_register(ad_port+CHANCSR_PORT,&dt_chancsr);

   dt_adcsr.word = 0;
   for(chan = 0; chan < num_channel; ++chan) {
      dt_adcsr.bit.chan = chan;
      dt_set_register(ad_port+ADCSR_PORT,&dt_adcsr);
   }

   dt_chancsr.bit.lle =  OFF;
   dt_set_register(ad_port+CHANCSR_PORT,&dt_chancsr);

   dt_adcsr.bit.adclk = ON;
   dt_adcsr.bit.ia_ddone = ON;
   dt_set_register(ad_port+ADCSR_PORT,&dt_adcsr);

   dt_supcsr.bit.bdinit = OFF;   
   dt_supcsr.bit.xclk = OFF;
   dt_supcsr.bit.xtrig = OFF;
   dt_supcsr.bit.strig = OFF;
   dt_supcsr.bit.prld = ON;
   dt_supcsr.bit.dacinit = OFF;
   dt_supcsr.bit.adcinit = OFF;
   dt_supcsr.bit.dacon = OFF;
   dt_supcsr.bit.buffb = OFF;
   dt_supcsr.bit.ds = SU_AD_TRIG_SCAN_DMA;
   dt_supcsr.bit.ddma = OFF;
   dt_supcsr.bit.clrdmadne =  OFF; 
   dt_supcsr.bit.errinten = ON;
   dt_set_register(ad_port+SUPCSR_PORT,&dt_supcsr);

   dt_adcsr.bit.muxbusy = ON;
   while(dt_adcsr.bit.muxbusy) 
      dt_read_register(ad_port+ADCSR_PORT,&dt_adcsr);
   
   dt_supcsr.bit.bdinit = OFF;   
   dt_supcsr.bit.xclk = OFF;
   dt_supcsr.bit.xtrig = ON;
   dt_supcsr.bit.strig = OFF;
   dt_supcsr.bit.prld = OFF;
   dt_supcsr.bit.dacinit = OFF;
   dt_supcsr.bit.adcinit = OFF;
   dt_supcsr.bit.dacon = OFF;
   dt_supcsr.bit.buffb = OFF;
   dt_supcsr.bit.ds = SU_AD_TRIG_SCAN_DMA;
   dt_supcsr.bit.errinten = ON;
   dt_set_register(ad_port+SUPCSR_PORT,&dt_supcsr);

}

