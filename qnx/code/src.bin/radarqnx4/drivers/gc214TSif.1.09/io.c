/* io.c 
   ====
   Author: R.J.Barnes
*/


/*
 $Log: io.c,v $
 Revision 1.2  2008/03/12 14:17:47  code
 Blackstone code fixes from Todd.

 Revision 1.1  2007/10/30 13:58:08  code
 Initial revision
 
*/

#include <stdlib.h>
#include <stdio.h>

/*
 $Log: io.c,v $
 Revision 1.2  2008/03/12 14:17:47  code
 Blackstone code fixes from Todd.

 Revision 1.1  2007/10/30 13:58:08  code
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


