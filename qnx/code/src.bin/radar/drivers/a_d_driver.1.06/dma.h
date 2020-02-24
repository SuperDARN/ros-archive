/* dma.h
   =====
   Author: R.J.Barnes
*/

/* this file contains the definitions for the dma registers 
*/

/* define the port numbers for  the registers in DMA    */
#define DMA_MASTER_RESET_PORT   0x0DA
#define DMA_RESET_FLIP_PORT     0x0D8
#define DMA_MODE_PORT           0x0D6
#define DMA_SINGLE_MASK_PORT    0x0D4

#define DMA_PAGE_REG_PORT_1       0x08B
#define DMA_PAGE_REG_PORT_2	      0x089

#define DMA_BASE_ADDR_PORT      0x0c4
#define DMA_WORD_COUNT_PORT     0x0c6
#define DMA_STATUS_PORT         0x0d0
#define DMA_COMMAND_PORT        0x0d0

/* dma channel 5 is 1 for controller 2 */

/* DMA mode given to the DMA mode register */
#define DMA_VERIFY_TRANSFER     0
#define DMA_WRITE_TRANSFER      1
#define DMA_READ_TRANSFER       2
#define DMA_SINGLE_MODE         1

/* general purpose definitions to turn bit on or off */
#define DMA_OFF                 0
#define DMA_ON                  1

/* useful structure to break up word into bytes */
struct dma_byte_str {
   unsigned char low_byte;
   unsigned char high_byte;
} ;

/* dma mode register structure */
union dma_mode_str {
   struct {
      unsigned int chan        : 2;
      unsigned int trans       : 2;
      unsigned int auto_init   : 1;
      unsigned int addr_sel    : 1;
      unsigned int block       : 2;
   } bit;
   struct dma_byte_str byte;
} ;

/* dma single mask register structure */
union dma_single_mask_str {
   struct {   
      unsigned int chan        : 2;
      unsigned int mask        : 1;
      unsigned int unused      : 5;
   } bit;
   struct dma_byte_str byte;
} ;

/* dma other register structure; other here means none of the two above */
union dma_other_reg_str {
   unsigned short int word;
   struct dma_byte_str byte;
} ;

/* convenient type for mapping logical to physical addresses */

union p_a_union {
   paddr_t paddr;
	unsigned char bytes[3];
} ;

/* dma_init initializes the dma
   the input parameter is
       ds     = segment where the buffer is located
       buffer = 16 bits offset from the segment; the starting address of 
                the buffer
       count  = the number of words to transfer
       mode_type = the type to transfer :
                       0 = DMA verify transfer
                       1 = DMA write transfer 
                       2 = DMA read transfer 
   notice the funny way the way DMA wants the address 

*/

void dma_init(paddr_t buffer,unsigned int channel,
			  unsigned int count,unsigned int mode_type);
