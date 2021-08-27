/* print_info.c
   ============ */

/*
 $Log: print_info.c,v $
 Revision 1.1  1999/04/10 16:51:11  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>

void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}

