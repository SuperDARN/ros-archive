/* bcd.c
   ===== */

/*
 $Log: bcd.c,v $
 Revision 1.3  1998/10/23 17:39:58  barnes
 Modifications to support the extended timing sequence.

 Revision 1.2  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.1  1996/03/13 15:13:31  root
 Initial revision

*/

/* This routine coverts an 8-bit value into a single 4-bit BCD digit
	The 8-bits are ANDED with a mask and then shifted into position */

unsigned char get_bcd(unsigned char x, unsigned char m,int s) {

  unsigned char temp;
  temp = x & m;
  temp = s < 0 ? temp << -s : temp >> s;
  return temp;
}


void binary_bcd(unsigned int a,char digits[]) {
  digits[0] = a/10000;
  a=a-10000*digits[0];
  digits[1] = a/1000;
  a=a-1000*digits[1];
  digits[2] = a/100;
  a=a-100*digits[2];
  digits[3] = a/10;
  digits[4] = a % 10;
  return;
}
