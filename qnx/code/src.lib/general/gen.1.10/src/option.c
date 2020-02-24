/* option.c
   ========
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

/*
 $Log: option.c,v $
 Revision 1.10  2001/06/27 20:38:16  barnes
 Added license tag

 Revision 1.9  2001/05/09 19:06:07  barnes
 Added code to deal with "quoted" options.

 Revision 1.8  2001/01/29 18:11:48  barnes
 Added Author Name

 Revision 1.7  1999/09/27 15:50:44  barnes
 Fixed bug in determining the correct return value when the last argument is
 a string.

 Revision 1.6  1999/09/27 13:54:50  barnes
 Changed the return value to be the first argument that does not start with
 a '-'.

 Revision 1.5  1999/09/15 01:56:27  barnes
 Added simple integer type and basic flag type.

 Revision 1.4  1999/09/09 23:50:28  barnes
 Fixed memory allocation problems.

 Revision 1.3  1999/09/09 23:11:50  barnes
 Fixed the way memory is released.

 Revision 1.2  1999/09/09 22:29:28  barnes
 Modifications for dynamic allocation of option tables.

 Revision 1.1  1998/06/03 13:01:36  barnes
 Initial revision

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "option.h"

char *scan_entry(FILE *file) {
  int c=0;
  int chr;
  int quote=0;

  char *parse=NULL;
  if ((parse=malloc(256)) ==NULL) return NULL;
  
  do {
    /* skip whitespace */
    while ( ((chr=fgetc(file)) != EOF) && 
          ( (chr==' ') || (chr == '\n') || (chr== '\t') ) );
 
   if (chr=='#') { /* skip this line its a comment */
      while ( ((chr=fgetc(file)) !=EOF) && (chr !='\n'));
      chr='#'; 
    }

  } while (chr=='#');

  if (chr=='"') quote=!quote;
  else {
    parse[0]=(char) chr;
    c++;
  }

  while ( (c<256) && ((chr=fgetc(file)) != EOF)) {

    if ((quote==0) && 
	((chr==' ') || (chr== '\n') || (chr== '\t'))) break;
    if (chr=='"') {
      quote=!quote;
      continue;
    }
    parse[c]=(char) chr;
   
    c++;
  }
  
  if ((chr==EOF) && (c==1)) {
    free(parse);
    return NULL;
  }
  parse[c]=0;
  parse=realloc(parse,c+1);
  return parse;
}

int opt_process(int argc,char *argv[], struct option *opt,
					void (*opterr)(char *)) {

  int i=1,k=1;
  while ((i<argc) && (argv[i][0]=='-')) {
    int j=0;
    while ( (opt[j].optname !=NULL) && 
            (strcmp(&argv[i][1],opt[j].optname) !=0) ) j++;

    if (opt[j].optname==NULL) {
      if (opterr !=NULL) (*opterr)(&argv[i][1]);
      i++;
      continue;
    }
    i++;
    opt[j].set=1;
    switch (opt[j].type) {
    case 'x'  : /* a flag */
        if (opt[j].ptr !=NULL) 
        *( (unsigned char *) opt[j].ptr)=(unsigned char) 1;
        break;
      case 'i' : /* vanilla integer */ 
        if (opt[j].ptr !=NULL)
        *( (int *) opt[j].ptr)=(int) atoi(argv[i]);
        i++;
        break;
      case 's' : /* short integer */
        if (opt[j].ptr !=NULL)
        *( (short int *) opt[j].ptr)=(short int) atoi(argv[i]);
        i++;
        break;
      case 'l' : /* long integer */
        if (opt[j].ptr !=NULL) 
        *( (long int *) opt[j].ptr)=(long int) atol(argv[i]);
        i++;
        break;
      case 'f' : /* float */
        if (opt[j].ptr !=NULL) 
        *( (float *) opt[j].ptr)=(float) atof(argv[i]);
        i++;
        break;
      case 'd' : /* double */
        if (opt[j].ptr !=NULL) 
        *( (double *) opt[j].ptr)=(double) atof(argv[i]);
        i++;
        break;
      case 't' : /* string */

        /* This is the only case were a null pointer in the 'ptr'
         * member of the structure is not ignored - instead
         * it is set to point to the first argv after the option
         * and the 'set' member, is set to the value of 'i'
         */

        if (opt[j].ptr !=NULL) *( (char **) opt[j].ptr)=argv[i];
        opt[j].set=i; /* will always be > 1 - so no problem */
        i++;
        break;
      default :
        break;
     } 
     /* skip to next option beginning with "-" */
     k=i;
     while ((i<argc) && (argv[i][0] !='-')) i++;
  }
  return k;
}

int option_process(int argc,char *argv[], struct optstr *opt,
					void (*opterr)(char *)) {
  return opt_process(argc,argv,opt->ptr,opterr);
}
 
void opt_file(FILE *fp,struct option *opt,void (*opterr)(char *)) {
  int fargc=1;
  char *fargv[256];  
  while ((fargc<256) && ((fargv[fargc]=scan_entry(fp)) !=NULL)) fargc++; 
  opt_process(fargc,fargv,opt,opterr);
} 

void option_file(FILE *fp,struct optstr *opt,void (*opterr)(char *)) {
  opt_file(fp,opt->ptr,opterr);
}



int option_add(struct optstr *opt,char *name,char type,void *data) {
  
 

  if (opt->ptr !=NULL) {
    opt->ptr=realloc(opt->ptr,sizeof(struct option)*(opt->num+2));
    if (opt->ptr==NULL) return -1;
  } else {
    opt->ptr=malloc(sizeof(struct option)*2);
    if (opt->ptr==NULL) return -1;
    opt->num=0; 
  }

  opt->ptr[opt->num].optname=malloc(strlen(name)+1);
  strcpy(opt->ptr[opt->num].optname,name);
  opt->ptr[opt->num].set=0;
  opt->ptr[opt->num].type=type;
  opt->ptr[opt->num].ptr=data;
  opt->ptr[opt->num+1].optname=NULL;
  opt->num++;
  return 0;
}
      
void option_free(struct optstr *opt) {
  if (opt->ptr !=NULL) {
    int i;
    for (i=0;(opt->ptr[i].optname !=NULL);i++) free(opt->ptr[i].optname);
    free(opt->ptr);
  }
  opt->ptr=NULL;
  opt->num=0;
}

struct option *option_read(struct optstr *opt,int num) {
  if (opt==NULL) return NULL;
  if (num>=opt->num) return NULL;
  return &opt->ptr[num];
}
  


