/* printinfo.c
   ===========
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 $Log: printinfo.c,v $
 Revision 1.2  2004/11/16 23:47:35  barnes
 Added a more sophisticated string interpreter that formats the output.

 Revision 1.1  2003/02/22 00:23:06  barnes
 Initial revision

*/

void OptionPrintInfo(FILE *fp,char *str[]) {
  int i,j,k,l,m,n;
  int maxtab=0;
  char buffer[4096];
  /* first go through looking for tabs */

  for (i=0;str[i] !=NULL;i++) {
    for (j=0;str[i][j] !=0;j++) if (str[i][j]=='\t') break;
    if ((str[i][j] !=0) && (j>maxtab)) maxtab=j;
  } 
  maxtab++;

  /* now output the lines */

  for (i=0;str[i] !=NULL;i++) {
    for (j=0;str[i][j] !=0;j++) if (str[i][j]=='\t') break;
    if (str[i][j]==0) {
      fprintf(fp,str[i]);
      continue;
    }
    strncpy(buffer,str[i],4095);
    buffer[j]=0;
    fprintf(fp,buffer);
    for (k=0;k<(maxtab-j);k++) buffer[k]=' ';
    buffer[k]=0;
    fprintf(fp,buffer);
    j++;
    /* chop the rest of the buffer up into 80 character long lines */
    m=0;
    l=maxtab;
    for (k=j;str[i][k] !=0;k++) {
      buffer[m]=str[i][k];
      m++;
      if (buffer[m-1]==' ') {
        if ((l+m-1)>80) {
	  /* new line */
          fprintf(fp,"\n");
          for (n=0;n<maxtab;n++) buffer[m+n]=' ';
	  buffer[m+n]=0;
          fprintf(fp,buffer+m);
          l=maxtab;
        }
        buffer[m]=0; 
        fprintf(fp,buffer);
        l+=m-1;
        m=0;
      }     
    }
    if ((l+m-1)>80) {
      /* new line */
      fprintf(fp,"\n");
      l=maxtab;
    }
    buffer[m]=0;
    fprintf(fp,buffer);
  }
}


