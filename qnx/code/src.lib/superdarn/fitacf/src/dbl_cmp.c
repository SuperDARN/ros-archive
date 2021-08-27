/* dbl_cmp.c
   ========= */
/*
 $Log: dbl_cmp.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */



int dbl_cmp(double *x,double *y) {
  if (*x > *y) return 1;
  else if (*x == *y) return 0;
  else return -1;
}
