/* addterm.c
   ========
   Author R.J.Barnes
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
 $Log: addterm.c,v $
 Revision 1.3  2001/06/27 20:08:21  barnes
 Added missing return statement.

 Revision 1.2  2001/06/27 19:52:37  barnes
 Fixed minor bug.

 Revision 1.1  2001/06/27 18:07:53  barnes
 Initial revision
 
 */


#include <stdio.h>
#include <stdlib.h>

#include "hlpstr.h"
#include "errstr.h"

char buffer[4096];
char term_buffer[4096];

void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}


int instr(char *body,char *str) {

  int i=0;
  int j=0;
  while (body[i] !=0) {
    if (body[i]==str[j]) j++;
    else j=0;
    if (str[j]==0) break;
    i++;
  }
  if (body[i]==0) return -1;
  return i-j+1;
}

int main(int argc,char *argv[]) {

  char *prefix;

  FILE *fp;
  char *terms;
  int i,j;
  int vbflg=0;
  

  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } else break;
  }

  if (i==argc) {
     print_info(stderr,errstr);
    exit(1);
  }

 

  terms=argv[i];

  while(fgets(buffer,4096,stdin) !=NULL) {

    if (((i=instr(buffer,"License")) !=-1) && (i>0) && 
        (buffer[i-1]=='$') && (buffer[i+7]=='$')) { 
 
      /* weird construction above so that we don't confuse the
         program when it is run against itself. */
      i--;
      buffer[i]=0;
      prefix=buffer;

      if (instr(prefix,"/*") !=-1) {
        fprintf(stdout,"%s\n",prefix);
        for (j=0;prefix[j] !=0;j++) if (prefix[j]=='/') prefix[j]=' ';
      }
      if (instr(prefix,"(*") !=-1) {
        fprintf(stdout,"%s\n",prefix);
        for (j=0;prefix[j] !=0;j++) if (prefix[j]=='(') prefix[j]=' ';
      }

      fp=fopen(terms,"r");
      while(fgets(term_buffer,4096,fp) !=NULL) {           
        fprintf(stdout,"%s%s",prefix,term_buffer);
      }
      fclose(fp);
      fprintf(stdout,"%s%s",prefix,buffer+i+9);
      continue;
    }
    fprintf(stdout,"%s",buffer);
  }
  return 0;
}
