/* sumproduct.c
   =============
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"

/*
 $Log: sumproduct.c,v $
 Revision 1.2  2004/05/04 18:47:28  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/05/03 21:30:33  barnes
 Initial revision

*/

int ACFSumProduct(int16 *buffer,float *avepower,
		   int numsamples,float *mxpwr,float *dco) {
  int sample;
  float sumpower;
  float power;
  float maxpower;
  float ltemp;

  float dcor1=0,dcoi1=0;

  sumpower = 0;
  maxpower = 0;

  if (dco !=NULL) {
    dcor1=dco[0];
    dcoi1=dco[1]; 
  }

  for (sample= 0;sample < numsamples; sample++) {
    ltemp = *buffer-dcor1;
    power = ltemp * ltemp;  /* compute the real */
    buffer++;
    ltemp = *buffer-dcoi1;
    power = power + ltemp * ltemp;    /* compute the imag */
    buffer++;

    /* compute max power */
    if (maxpower < power) maxpower = power;
    sumpower = sumpower + power;
  }

  *mxpwr = maxpower;
  *avepower = sumpower / numsamples;
  return 0;
}


