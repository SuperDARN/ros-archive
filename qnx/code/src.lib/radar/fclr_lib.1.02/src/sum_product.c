/* sum_product.c
   ============= */

/* 
 $Log: sum_product.c,v $
 Revision 1.1  1999/03/25 21:04:05  barnes
 Initial revision

 Revision 1.1  1998/10/28 22:12:32  barnes
 Initial revision

 */

#include <stdio.h>
#include "types.h"

int sum_product(int16 *buffer,int *ave_power,
				int num_samples,int *mxpwr) {
  int sample;
  int sum_power;
  int power;
  int max_power;
  int l_temp;

  sum_power = 0L;
  max_power = 0L;

 
  for (sample= 0;sample < num_samples; sample++) {
    l_temp = *buffer;
    power = l_temp * l_temp;  /* compute the real */
    buffer++;
	l_temp = *buffer;
    power = power + l_temp * l_temp;    /* compute the imag */
    buffer++;

    /* compute max power */
    if (max_power < power) max_power = power;
    sum_power = sum_power + power;
  }

  *mxpwr = max_power;
  *ave_power = sum_power / num_samples;
  return 0;
}
