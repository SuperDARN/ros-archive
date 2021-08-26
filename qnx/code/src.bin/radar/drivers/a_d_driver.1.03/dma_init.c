/* dma_init.c
   ==========  */

/*
 $Log: dma_init.c,v $
 Revision 1.6  2000/03/02 16:35:51  barnes
 Fixed bug in setting up the DMA registers.

 Revision 1.5  2000/02/16 17:00:35  barnes
 Attempted fix to the DMA problem.

 Revision 1.4  1999/03/25 22:02:10  barnes
 Removed dma.dec

 Revision 1.3  1998/11/06 15:21:16  barnes
 Added the ability to specify the card address, IRQ, and DMA channel
 on the command line.

 Revision 1.2  1998/06/03 19:06:01  root
 Modification to the version control system.

 Revision 1.1  1996/03/13 14:03:36  root
 Initial revision

*/

/* dma_init initializes the dma
   the input parameter is
       buffer = physical address
       count  = the number of words to transfer
       mode_type = the type to transfer :
                       0 = DMA verify transfer
                       1 = DMA write transfer 
                       2 = DMA read transfer 
*/

#include <stdio.h>
#include <i86.h>
#include <sys/types.h>

#include "dma.h"
#include "pragma.h"
#include "dt2828.h"

union dma_mode_str dma_mode;                 /* dma_mode register */
union dma_single_mask_str dma_single_mask;   /* dma single mask register */
union dma_other_reg_str dma_other_reg;       /* dma other register */




void dma_init(paddr_t buffer,unsigned int channel,
		      unsigned int count,unsigned int mode_type) {

   char adr_high;            /* most significant bits of the physical addr */
   unsigned short int adr_low;     /* least significant bits of the physical adr */
   union p_a_union p_a;
   short int psw;

   p_a.paddr = buffer;

   /* disable interrupt for mutex */

   psw = disable();

   /* set pointer flip_flop */
   dma_other_reg.byte.low_byte = 0;   
   dma_set_register(DMA_RESET_FLIP_PORT,&dma_other_reg.byte,1);

   /* set mode */
   dma_mode.bit.chan = channel;
   dma_mode.bit.trans = mode_type;
   dma_mode.bit.auto_init = DMA_OFF;
   dma_mode.bit.addr_sel = DMA_OFF;
   dma_mode.bit.block = DMA_SINGLE_MODE;
   dma_set_register(DMA_MODE_PORT,&dma_mode.byte,1);

   /* set page select register */
   dma_other_reg.byte.low_byte = p_a.bytes[2];
   if (channel==1) 
     dma_set_register(DMA_PAGE_REG_PORT_1,&dma_other_reg.byte,1);
   if (channel==2) 
     dma_set_register(DMA_PAGE_REG_PORT_2,&dma_other_reg.byte,1);


   /* set base adress  */
   /* DMA wants the address / 2 */
   p_a.paddr = p_a.paddr >> 1;
   dma_other_reg.byte.low_byte = p_a.bytes[0];
	dma_other_reg.byte.high_byte = p_a.bytes[1];
   dma_set_register(DMA_BASE_ADDR_PORT+4*(channel-1),&dma_other_reg.byte,2);

   /* word count */
   dma_other_reg.word = count; 
   dma_set_register(DMA_WORD_COUNT_PORT+4*(channel-1),&dma_other_reg.byte,2);

   /* set mask register */
   dma_single_mask.bit.chan = channel;
   dma_single_mask.bit.mask = DMA_OFF;
   dma_set_register(DMA_SINGLE_MASK_PORT,&dma_single_mask.byte,1);


   /* enable interrupt */

   restore(psw);

} 
