/* dbl_cmp.c
   =========
   Author: R.J.Barnes & K. Baker
*/

/*
 (c) 2009 JHU/APL
 
*/




int dbl_cmp(const void *x,const void *y) {
  double *a,*b;
  a=(double *) x;
  b=(double *) y;
  if (*a > *b) return 1;
  else if (*a == *b) return 0;
  else return -1;
}
