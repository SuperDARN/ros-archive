/* cnv_freq.c
   ==========
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

#include "port.h"
#include "bcd.h"

/*
 $Log: cnv_freq.c,v $
 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

*/

extern struct dio_hdw hdw;

void cnv_freq(int freq,unsigned char *bptr) {
  int i;  
  unsigned char f[5];
  int FMHz, FKHz;
  int true_freq;

  if (hdw.FREQ_inv !=0) true_freq= (hdw.Local_Oscillator-freq)/2L;
  else true_freq = freq + hdw.Local_Oscillator; 
  

  FMHz = true_freq/1000;		/* get the MHz part */
  FKHz = true_freq % 1000;	/* get the kilohertz part */

  binary_bcd(FMHz,f);			/* convert MHz to bcd */
  for (i=0; i<5; i++)  f[i] = f[i] ^ 0x0f;	/* complement for negative logic */

  bptr[0]= f[3] & 0x0f;	/* output the 10 MHz digit */
  bptr[1]= (f[4] << 4) &  0xf0; /* output the 1 MHz digit */

  binary_bcd(FKHz,f);			/* convert KHz to bcd */
  for (i=0; i<5; i++)  f[i] = f[i] ^ 0x0f;	/* complement for negative logic */

  bptr[1] |=f[2] & 0x0f;	/* output the 100 KHz digit */
  bptr[2] = (f[3] << 4) | f[4];	    /* shift the 10s digit left 4 bits
									and add the 1's digit */
} 
