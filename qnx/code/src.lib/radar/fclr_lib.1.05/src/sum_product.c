/* sum_product.c
   =============
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

/* 
 $Log: sum_product.c,v $
 Revision 1.4  2001/10/15 15:50:25  barnes
 Changed internals to floating point.

 Revision 1.3  2001/06/27 20:53:22  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:49  barnes
 Added Author Name

 Revision 1.1  1999/03/25 21:04:05  barnes
 Initial revision

 Revision 1.1  1998/10/28 22:12:32  barnes
 Initial revision

 */

#include <stdio.h>
#include "types.h"

int sum_product(int16 *buffer,float *ave_power,
				int num_samples,float *mxpwr) {
  int sample;
  float sum_power;
  float power;
  float max_power;
  float l_temp;

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
