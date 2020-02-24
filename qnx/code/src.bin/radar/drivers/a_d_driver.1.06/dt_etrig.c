/* dt_etrig.c
   ==========
   Author: R.J.Barnes
*/

/*
 Copyright � 2001 The Johns Hopkins University/Applied Physics Laboratory.
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
 $Log: dt_etrig.c,v $
 Revision 1.7  2001/06/27 20:55:33  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:12:05  barnes
 Added Author Name

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

