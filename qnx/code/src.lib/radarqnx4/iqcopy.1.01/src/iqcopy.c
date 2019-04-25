/* iqcopy.c
   ======== */

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
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"

/*
 $Log: iqcopy.c,v $
 Revision 1.1  2008/03/20 21:10:14  code
 Initial revision
 
*/

int IQCopy(int16 *inbuf,int16 *outbuf,int numsmp,int numchn,
           int roff,int ioff,int smpoff,int chnoff) {

  int c,s;
  int inoff,outoff;
  
  /* sort the IQ data into I&Q pairs for each channel. If the channels
   * are multiplexed, demultiplex the buffer so the channels appear
   * after each other.
   */

  if ((roff==0) && (ioff==1) && (smpoff==1) && (chnoff==(numsmp*2))) {
     memcpy( (char *) outbuf,(char *) inbuf,sizeof(int16)*numsmp*numchn*2);
     return 0;
  }
    
  for (c=0;c<numchn;c++) {
    for (s=0;s<numsmp;s++) {
      inoff=(s*smpoff)+chnoff*c;
      outoff=2*(c*numsmp+s);

      outbuf[outoff]=inbuf[inoff+roff];
      outbuf[outoff+1]=inbuf[inoff+ioff];
    }
  }

  return 0; 
}
  

    



 

