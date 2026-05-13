/* dt.h
   ====
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
 $Log: dt.h,v $
 Revision 1.1  2004/07/13 22:52:34  barnes
 Initial revision

*/


/* structure for i/o with dt register */
struct dt_byte_str {
   char low_byte;
   char high_byte;
};

/* dt adcsr register structure */
union dt_adcsr_str {
   struct {
      unsigned int chan :        4;
      unsigned int gs :          2;
      unsigned int ia_ddone :    1;
      unsigned int a_ddone :     1 ;
      unsigned int muxbusy :     1;
      unsigned int adclk :       1;
      unsigned int reserved :    5;
      unsigned int a_derr  :     1;
   } bit;
   struct dt_byte_str byte;
   unsigned short int word;
} ;

/* dt d chancsr register structure */
union dt_chancsr_str {
   struct {
      unsigned int numb :        4;
      unsigned int reserved_1 :  4;
      unsigned int presla :      4;
      unsigned int reserved_2 :  3;
      unsigned int lle :         1;
   } bit;
   struct dt_byte_str byte; 
   unsigned short int word;
};

/* dt dacsr register structure */
union dt_dacsr_str {
   struct {
      unsigned int lboe :        1;
      unsigned int hboe :        1;
      unsigned int res_1 :  3;
      unsigned int daclk :       1;
      unsigned int id_ardy :     1;
      unsigned int dacrdy :      1;
      unsigned int ssel :        1;
      unsigned int ysel :        1;
      unsigned int res_2 :  5;
      unsigned int d_aerr :      1;
   } bit;
   struct dt_byte_str byte;
   unsigned short int word;
};

/* dt supcsr register structure */
union dt_supcsr_str {
   struct {
      unsigned int bdinit :      1;
      unsigned int xclk  :       1;
      unsigned int xtrig :       1;
      unsigned int strig :       1;
      unsigned int prld :        1;
      unsigned int dacinit :     1;
      unsigned int adcinit :     1;
      unsigned int dacon :       1;
      unsigned int scdn :        1;
      unsigned int buffb :       1;
      unsigned int ds :          2;
      unsigned int ddma :        1;
      unsigned int clrdmadne :   1;
      unsigned int errinten :    1;
      unsigned int dmad :        1;
   } bit;
   struct dt_byte_str byte;
   unsigned short int word;
} ;

/* dt tmrctr register structure */
union dt_tmrctr_str {
   struct {
      unsigned int counter :     8;
      unsigned int prs     :     4;
      unsigned int res :    4;
   } bit;
   struct dt_byte_str byte;
   unsigned short int word;
} ;

#define BACKGRND_TSK       1          /* create background task */
#define CLOCK_FREQ         80000l    /* the dt internal clock freq */
#define ON                 1
#define OFF                0
#define NUM_RAM_M_1        3          /* number of RAM entries minus 1 */          
#define SU_NO_DMA            0        /* SUPCSR no DMA  */
#define SU_AD_CLOCKED_DMA    1        /* SUPCSR A/D clocked DMA */ 
#define SU_DA_CLOCKED_DMA    2        /* SUPCSR D/A clocked DMA */
#define SU_AD_TRIG_SCAN_DMA  3        /* SUPCSR A/D triggered scan DMA */


#define AD_IRQ         15         /* A/D interrupt handler */
#define AD_PORT 	   0x240

/* definitions for the dt 2828 ports */
#define ADCSR_PORT         0x0 
#define CHANCSR_PORT       0x2
#define ADDAT_PORT         0x4
#define DACSR_PORT         0x6
#define DADAT_PORT         0x8
#define DIODAT_PORT        0xA
#define SUPCSR_PORT        0xC
#define TMRCTR_PORT        0xE
