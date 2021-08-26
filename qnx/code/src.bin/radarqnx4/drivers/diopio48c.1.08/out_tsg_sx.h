/* out_tsg_sx.h
   ============
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
 $Log: out_tsg_sx.h,v $
 Revision 1.1  2004/07/13 22:59:20  barnes
 Initial revision

*/

#define X_MAX 10000

int out_tsg_sx(unsigned char *tsg_xtd_A,unsigned char *tsg_xtd_B,
               unsigned char *tsg_code_A,unsigned char *tsg_code_B,
               unsigned char *tsg_rep_A,unsigned char *tsg_rep_B,
	       int tsg_length_A,int tsg_length_B,
               int tsg_delay_A,int tsg_delay_B,
               char flag_A,char flag_B,
               unsigned char *fptr_A,unsigned char *fptr_B,
               int TSG_safe_A,int TSG_safe_B,  
               int TSG_port_A,int TSG_port_B,
               int TSG_mask_A,int TSG_mask_B,
               int TSG_state_A,int TSG_state_B,
               int BEAM_port_A,int BEAM_mask_A,int BEAM_state_A,
               int BEAM_port_B,int BEAM_mask_B,int BEAM_state_B,
               int CLOCK_port,int CLOCK_mask,
	       int *fadr_A,int *fadr_B);
