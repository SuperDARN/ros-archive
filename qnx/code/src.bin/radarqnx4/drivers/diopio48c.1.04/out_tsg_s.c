/* out_tsg_s.c
   ===========
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

#include <i86.h>
#include <stdlib.h>
#include <conio.h>
#include "out_tsg_s.h"

/* 
 $Log: out_tsg_s.c,v $
 Revision 1.2  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

 */

int out_tsg_s(unsigned char *tsg_code_A,unsigned char *tsg_code_B,
            unsigned char *tsg_rep_A,unsigned char *tsg_rep_B,
  	        int tsg_length_A,int tsg_length_B,
            int tsg_delay_A,int tsg_delay_B,
            int TSG_safe_A,int TSG_safe_B, 
            int TSG_port_A,int TSG_port_B,
            int TSG_mask_A,int TSG_mask_B,
            int TSG_state_A,int TSG_state_B,
            int CLOCK_port,int CLOCK_mask) {

  register int port_A=TSG_port_A;
  register int port_B=TSG_port_B;
  register int pmask_A=TSG_mask_A;
  register int pmask_B=TSG_mask_B;
  register int pstate_A=TSG_state_A;
  register int pstate_B=TSG_state_B;

  register int clock=CLOCK_port;
  register int cmask=CLOCK_mask; 
  register unsigned int wrd;
 
  register int k_A=0;
  register int j_A=0;
  register int k_B=0;
  register int j_B=0;


  register int x=0;

  register int safe_A=TSG_safe_A;
  register int safe_B=TSG_safe_B;
 

  k_A=tsg_delay_A+1;
  k_B=tsg_delay_B+1;


  _disable();
  outp(port_A,(safe_A & pmask_A) | pstate_A); 
  outp(port_B,(safe_B & pmask_B) | pstate_B); 
 
  do {
    do { /* wait for the clock to go low before outputing pulse */
      wrd=inp(clock);
      x++;
    } while (((wrd & cmask) !=0) && (x<X_MAX));
    if (x==X_MAX) break;
    x=0;

    k_A--;
    if (k_A==0) {
      /* output next pulse */
      if (j_A !=tsg_length_A) {
         outp(port_A,(tsg_code_A[j_A] & pmask_A) | pstate_A);
         k_A=tsg_rep_A[j_A];
         j_A++;
      } else {
        k_A=65536; /* delay as long as possible */
        outp(port_A,(safe_A & pmask_A) | pstate_A);
      }
    }

    k_B--;
    if (k_B==0) {
      /* output next pulse */
      if (j_B !=tsg_length_B) {
         outp(port_B,(tsg_code_B[j_B] & pmask_B) | pstate_B);
         k_B=tsg_rep_B[j_B];
         j_B++;
      } else {
        k_B=65536; /* delay as long as possible */
        outp(port_B,(safe_B & pmask_B) | pstate_B);
      }
    }

    do { /* wait for the clock to go high */
      wrd=inp(clock);
      x++;
    } while (((wrd & cmask)==0) && (x<X_MAX));   
    if (x==X_MAX) break;
    x=0;

  } while ((j_A<tsg_length_A) || (j_B<tsg_length_B));
  _enable();
  if (x==X_MAX) return -1;
  return 0;
 
}



  
   

  
