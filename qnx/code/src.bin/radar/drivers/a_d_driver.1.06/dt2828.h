/* dt2828.h
   ========
   Author: R.J.Barnes
*/

/* this file contains the function prototypes for dt2828.c*/


void dma_set_register(int port,struct dma_byte_str *data,int count);
void dma_read_register(int port,struct dma_byte_str *data,int count);
dt_set_register(int port,unsigned short int *data);
void dt_read_register(int port,unsigned short int *data);
