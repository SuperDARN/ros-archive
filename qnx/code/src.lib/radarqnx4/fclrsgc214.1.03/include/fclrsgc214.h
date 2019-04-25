/* fclrgc214.h
   ===========
   Author: R.Todd Parris & R.J.Barnes
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
 $Log: fclrsgc214.h,v $
 Revision 1.1  2008/03/12 19:33:30  code
 Initial revision

*/

#ifndef _FCLRSGC214TS_H
#define _FCLRSGC214TS_H

#define AB_FREQ_DIFF 15

int FCLRSGC214(ipcid_t dioid,ipcid_t *drid,struct FreqTable *fptr,
              int startA,int endA,int rsepA,
              int startB,int endB,int rsepB,
              int Fclk,int roff,int ioff,
	          void **bufadr,int bufoff,
		      int *tfreqA,float *noiseA,float *statisticA,
              int *flgA,void (*userA)(int start,int end,int rsep,
                                    int nn,float *pwr,void *usrdata),
              void *usrdataA,
              int *tfreqB,float *noiseB,float *statisticB,
              int *flgB,void (*userB)(int start,int end,int rsep,
                                    int nn,float *pwr,void *usrdata),
              void *usrdataB);



#endif
