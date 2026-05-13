/* DIO.c
   =====
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
#include <stdio.h>
#include <i86.h>
#include <conio.h>

/*
 $Log: DIO.c,v $
 Revision 1.2  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:26:38  barnes
 Initial revision

*/


extern char debug;

int dio_adr[2]={0x510,0};
static unsigned char port_cont[24];


/* the internal port number decodes like this 
 *
 *   card|clock|connector|port
 *    1     1     1        2 
 *
 */

int DIO_adr(int dio_port) {
  int card;
  int port;
  card=(dio_port & 0x10)>>4;
  port=(dio_port & 0x0f);
  return dio_adr[card]+port;
}

int DIO_state(int dio_port) {
  return port_cont[dio_port];
}

int DIO_value(int dio_port,unsigned char dio_value,
		      unsigned char dio_mask) {
  return (port_cont[dio_port] & ~dio_mask)
		| (dio_value & dio_mask);
}
  
void PUT_DIO(int dio_port,unsigned char dio_value,
			  unsigned char dio_mask) {
  int card;
  int port;
  card=(dio_port & 0x10)>>4;
  port=(dio_port & 0x0f);
  port_cont[dio_port] = (port_cont[dio_port] & ~dio_mask)
		| (dio_value & dio_mask);

  if (debug !=0) return;
  if (dio_adr[card]==0) return;
  outp(dio_adr[card]+port,port_cont[dio_port]);
  return;
}

unsigned char GET_DIO(int dio_port) {
  unsigned char i;
  int card,port;
  card=(dio_port & 0x10)>>4;
  port=(dio_port & 0x0f);
  if (debug !=0) return 0;
  if (dio_adr[card]==0) return 0;
  i=inp(dio_adr[card]+port);
  return i;
}
