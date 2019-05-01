/* io.c 
   ====
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

#include <stdlib.h>
#include <stdio.h>

/*
 $Log: io.c,v $
 Revision 1.1  2004/05/10 15:03:40  barnes
 Initial revision

*/

void write8(unsigned long reg1, signed char val){
	unsigned char *reg;
	reg=(unsigned char *) reg1;
	*(reg)=(char)val;
}

void write16(unsigned long reg1, signed short val){
	signed short *reg;
	reg=(signed short *) reg1;
	*(reg)=(short)val;
}

void write32(unsigned long reg1, signed long val){
	unsigned long *reg;
	reg=(unsigned long *) reg1;
	*(reg)=(long)val;
}

char read8(unsigned long reg1){
	unsigned long *reg;
	signed char val;
	reg=(unsigned long *) reg1;
	val=(char)(*reg);
	return val;
}

signed short read16(unsigned long reg1){
	unsigned long *reg;
	signed short val;
	reg=(unsigned long *) reg1;
	val=(short)*(reg);
	return val;
}

long read32(unsigned long reg1){
	unsigned long *reg;
	signed long val;
	reg=(unsigned long *) reg1;
	val=(long)*(reg);
	return val;
}


