/* dt2828.c
   ======== */

/*
 $Log: dt2828.c,v $
 Revision 1.2  1998/06/03 19:06:01  root
 Modification to the version control system.

 Revision 1.1  1996/03/13 14:03:36  root
 Initial revision

*/

/* this file contains utility for dt_2828 a/d converter card or DMA
*/

#include <conio.h>
#include <sys/types.h>

#include "dma.h"

/* dma_set_register sets the register of  DMA
   the input parameters 
      port = the port number to send the data
      data = the pointer to data
      count = the number of bytes to send

    the DMA register can only be set a byte at a time
*/

void dma_set_register(int port,struct dma_byte_str *data,int count) {

   outp(port,(int) data->low_byte);
   if(count > 1) 
      outp(port,(int) data->high_byte);
} 

/* dma_read_register reads the register of DMA
   the input parameters 
      port = the port number to read data from
      data = pointer to data
      count = the number of bytes to read

   DMA register operates in byte mode
*/

void dma_read_register(int port,struct dma_byte_str *data,int count) {
   unsigned int input;       /* input integer */

   input = inp(port);
   data->low_byte = input;
   /* if 2 bytes are requested */
   if(count > 1) {
      input = inp(port);
      data->high_byte = input;
   }
}

/* dt_set_register sets the register of  dt 2828
   input parameters
      port = port number to send
      data = pointer to data to send
*/

dt_set_register(int port,unsigned short int *data) {
   outpw(port,*data);
}

/* dt_read_register reads the register of dt 2828
   input paramter
      port = port number to read
      data = pointer to data to be read
*/

void dt_read_register(int port,unsigned short int *data) {
   *data = inpw(port);
} 
