/* port.h
   ======
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

/*
 $Log: port.h,v $
 Revision 1.4  2007/11/13 17:52:07  code
 Added super wide filter option.

 Revision 1.3  2005/06/17 18:57:44  barnes
 Added the inv_TX flag.

 Revision 1.2  2004/11/12 14:25:30  barnes
 Added phase port.

 Revision 1.1  2004/07/13 22:59:19  barnes
 Initial revision

*/


struct dio_hdw {
  char name[64];
  int Fstatus_port[2];	
  int FKHz_port[2];
  int FMKHz_port[2];
  int FMHz10_port[2];
  int Atten_port[2];
  int BEAM_port[2];
  int ANTENNA_port[2];
  int PHASE_port[2];
  int FILTER_port[2];
  int WIDEFILTER_port[2];
  int TSG_port[2];
  int TSG_safe[2];
  int CLOCK_port[2];
  int TEST_port[2];
  int AGCstatus_port[2];
  int LOPWRstatus_port[2];
  int micro_clock[2];
  int micro_clock_cw[2];

  int one_shot[2];
  int one_shot_cw[2];

  int FKHz_mask[2];
  int FMKHz_mask[2];
  int FMHz10_mask[2];
  int Atten_mask[2];
  int BEAM_mask[2];
  int ANTENNA_mask[2];
  int PHASE_mask[2];
  int FILTER_mask[2];
  int WIDEFILTER_mask[2];
  int CLOCK_mask[2];
  int TEST_mask[2];
  int TSG_mask[2];
  int AGCstatus_mask[2];
  int LOPWRstatus_mask[2];
  int Fstatus_bit[2];

  int control_word[4];

  int Atten_max;
  int BEAM_inv;
  int TX_inv;
  int FREQ_inv;
  int FSTAT_inv;
  int AGCstatus_inv;
  int LOPWRstatus_inv;
  int Local_Oscillator;
}; 
