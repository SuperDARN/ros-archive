/* addtext.c
   =========
   Author R.J.Barnes
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
 "Radar Software Toolkit - Core Software" (hereinafter "the Program").
 
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
 $Log: addtext.c,v $
 Revision 1.6  2004/07/16 22:50:20  barnes
 removed redundant header.

 Revision 1.5  2004/07/06 18:37:14  barnes
 Added missing header.

 Revision 1.4  2004/04/27 13:24:56  barnes
 Audit - Enforced warning set to "all".

 Revision 1.3  2002/02/26 16:30:09  barnes
 Modification to make the routing more general purpose.

 Revision 1.2  2001/10/24 12:41:38  barnes
 Fixed problem with NULL pointer.

 Revision 1.1  2001/07/06 14:41:03  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hlpstr.h"

char buffer[4096];
char text_buffer[4096];
char *dstr={"License"};
char *rtext={"text"};

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
  int tlen=0;
  char *tstr=NULL;
  char *rstr=NULL;
  char *prefix;

  FILE *fp;
  char *texts=NULL;
  int i,j;

  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } if (strcmp(argv[i],"-t")==0) {
      tstr=argv[i+1];
      i++;
    } else if (strcmp(argv[i],"-r")==0) {
      rstr=argv[i+1];
      i++;
    } else break;
  }

  if (tstr==NULL) tstr=dstr;
  if (rstr==NULL) rstr=rtext;
  if (i<argc) texts=argv[i];
  tlen=strlen(tstr);  


  while(fgets(buffer,4096,stdin) !=NULL) {

    if (((i=instr(buffer,tstr)) !=-1) && (i>0) && 
        (buffer[i-1]=='$') && (buffer[i+tlen]=='$')) { 
 
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

      if (texts !=NULL) {
        fp=fopen(texts,"r");
        while(fgets(text_buffer,4096,fp) !=NULL) {           
          fprintf(stdout,"%s%s",prefix,text_buffer);
        }
        fclose(fp);
        fprintf(stdout,"%s%s",prefix,buffer+i+tlen+2);
      } else {
        fprintf(stdout,"%s%s%s",prefix,rstr,buffer+i+tlen+2);
      }
      continue;
    }
    fprintf(stdout,"%s",buffer);
  }
  return 0;
}
