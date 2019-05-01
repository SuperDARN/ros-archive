/* dt2828.c
   ========
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

#include <conio.h>
#include <sys/types.h>

#include "dma.h"

/*
 $Log: dt2828.c,v $
 Revision 1.1  2004/05/10 14:32:20  barnes
 Initial revision

*/

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
