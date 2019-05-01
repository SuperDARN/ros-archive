/* dma_init.c
   ==========
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

/*
 $Log: dma_init.c,v $
 Revision 1.2  2004/06/22 12:33:00  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:32:20  barnes
 Initial revision

*/

union dma_mode_str dma_mode;                 /* dma_mode register */
union dma_single_mask_str dma_single_mask;   /* dma single mask register */
union dma_other_reg_str dma_other_reg;       /* dma other register */




void dma_init(paddr_t buffer,unsigned int channel,
		      unsigned int count,unsigned int mode_type) {

   
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
