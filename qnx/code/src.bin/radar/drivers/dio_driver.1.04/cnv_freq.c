/* cnv_freq.c
   ========== */

#include "port.h"
#include "bcd.h"

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