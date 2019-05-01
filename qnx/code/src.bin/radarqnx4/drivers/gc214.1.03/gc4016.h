/* gc4016.h
   ======== 
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
 $Log: gc4016.h,v $
 Revision 1.1  2004/07/13 22:51:35  barnes
 Initial revision

*/

struct GC4016Global {
   char nchanout;       /* 0x27 */
   char nmult;          /* 0x46 */
   char filtslct;       /* 0x00 */
   char finalshft;      /* 0x16 */
   char chanmap;        /* 0xe4 */
   char addto;          /* 0x00 */
   char resampclkdvd;   /* 0x00 */
   char ratiomap;       /* 0xe4 */
   int ratio0;          /* 0x04000000 */
   int ratio1;          /* 0x04000000 */
   int ratio2;          /* 0x04000000 */   
   int ratio3;          /* 0x04000000 */  
   char cntbyte0;       /* 0x00 */
   char cntbyte1;       /* 0x00 */
   char tricntrl;       /* 0xff */
   char outformat;      /* 0x49 */
   char outmode;        /* 0x6c */
   char outframecntrl;  /* 0xc7 */
   char outwdsize;      /* 0xef */
   char outclkcntrl;    /* 0x01 */ 
   char sermuxcntrl;    /* 0xe4 */
   char outtaga;        /* 0x10 */
   char outtagb;        /* 0x32 */
   char outtagc;        /* 0x54 */
   char outtagd;        /* 0x76 */
};

struct GC4016Channel { 
  int chreset;          /* 0x0c*/
  int phase;            /* 0x0000 */
  int freq;             /* 0x2f430313 */
  int decratio;         /* 0x703f */
  int cicscale;         /* 0x64 */
  int splitiq;          /* 0x00 */
  int cfir;             /* 0x00 */
  int pfir;             /* 0x00 */
  int input;            /* 0x00 */
  int peakcntrl;        /* 0x00 */
  int peakcount;        /* 0x00 */
  int finegain;         /* 0x00 */
            
};


void setupGC4016(long baseGC214,struct GC4016Global *ptr);
void releaseGC4016(long baseGC124,int *resampcoeff);
void setupGC4016channel(long baseGC214,int channel,
                        struct GC4016Channel *ptr,int *cfircoeff,
                        int *pfircoef);
