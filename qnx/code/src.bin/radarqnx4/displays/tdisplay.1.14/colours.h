/* colours.h
   =========
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
 $Log: colours.h,v $
 Revision 1.1  2004/07/13 22:59:24  barnes
 Initial revision

*/


#define PWR_COL_MAX 39
#define VEL_COL_MAX 39

  struct col_map {
    char chr;
    unsigned attr; 
  } pwr_key[]={
    
    {'\xb2',TERM_BLUE},
	{'\xdb',TERM_BLUE},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xdb',TERM_BLUE | TERM_HILIGHT},

	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN},

	{'\xb0',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb2',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN | TERM_HILIGHT},

	{'\xb2',TERM_CYAN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_CYAN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb0',TERM_CYAN | TERM_HILIGHT | TERM_GREEN_BG},
    {'\xb0',TERM_CYAN | TERM_GREEN_BG},
	{'\xdb',TERM_GREEN},

	{'\xb0',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_GREEN | TERM_HILIGHT},

	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_YELLOW | TERM_HILIGHT},

	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_YELLOW},

	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_RED | TERM_HILIGHT},

	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xdb',TERM_RED},
    {'\xb2',TERM_RED},
    {0,0}
};

struct col_map vel_key[]={

	{'\xb0',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_GREEN | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_GREEN | TERM_HILIGHT},

	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_GREEN_BG},
	{'\xdb',TERM_YELLOW | TERM_HILIGHT},

	{'\xb2',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb0',TERM_YELLOW | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_YELLOW},

	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_YELLOW_BG},
	{'\xdb',TERM_RED | TERM_HILIGHT},

	{'\xb2',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb1',TERM_RED | TERM_HILIGHT | TERM_RED_BG},
	{'\xb0',TERM_RED | TERM_HILIGHT | TERM_RED_BG},

	{'\xdb',TERM_RED}, 
    {'\xb2',TERM_BLUE}, 

	{'\xdb',TERM_BLUE},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_BLUE_BG},
	{'\xdb',TERM_BLUE | TERM_HILIGHT},

	{'\xb2',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb0',TERM_BLUE | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN},

	{'\xb0',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb1',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xb2',TERM_CYAN | TERM_HILIGHT | TERM_CYAN_BG},
	{'\xdb',TERM_CYAN | TERM_HILIGHT},
    
    {'\xb0',TERM_WHITE | TERM_HILIGHT | TERM_CYAN_BG},
    {'\xb1',TERM_WHITE | TERM_HILIGHT | TERM_CYAN_BG},

    {'\xb0',TERM_WHITE | TERM_HILIGHT | TERM_WHITE_BG},
    {'\xb1',TERM_WHITE | TERM_HILIGHT | TERM_WHITE_BG},
    {'\xb2',TERM_WHITE | TERM_HILIGHT | TERM_WHITE_BG},
    {'\xdb',TERM_WHITE | TERM_HILIGHT},
        
    {0,0}
};

char gsct_key='\xb0';
