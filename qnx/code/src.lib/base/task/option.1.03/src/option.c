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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "option.h"

/*
 $Log: option.c,v $
 Revision 1.2  2004/04/26 20:16:41  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/01/05 21:31:05  barnes
 Initial revision

*/


char *OptionScanFileArg(FILE *file) {

  int c=0;
  int chr;
  int stp=256;
  int max=256;
  int quote=0;
  char *txt=NULL;
  char *tmp=NULL;

  txt=malloc(max);
  if (txt==NULL) return NULL;
    
  do {
    while ( ((chr=fgetc(file)) != EOF) && 
          ( (chr==' ') || (chr == '\n') || (chr== '\t') ) );
 
   if (chr=='#') { /* skip this line its a comment */
      while ( ((chr=fgetc(file)) !=EOF) && (chr !='\n'));
      chr='#'; 
    }

  } while (chr=='#');

  if (chr=='"') quote=!quote;
  else {
    txt[0]=(char) chr;
    c++;
  }

  while ((chr=fgetc(file)) != EOF) {

    if ((quote==0) && 
	((chr==' ') || (chr== '\n') || (chr== '\t'))) break;

    if (chr=='"') {
      quote=!quote;
      continue;
    }

    txt[c]=(char) chr;
    c++;
    if (c>=max) {
      char *tmp;
      max+=stp;
      tmp=realloc(txt,max);
      if (tmp==NULL) {
        free(txt);
        return NULL;
      }
      txt=tmp;
    }
  }
  
  if ((chr==EOF) && (c==1)) {
    free(txt);
    return NULL;
  }
  txt[c]=0;

  tmp=realloc(txt,c+1);
  if (tmp==NULL) {
    free(txt);
    return NULL;
  }
  txt=tmp;
  return txt;
}

struct OptionFile *OptionProcessFile(FILE *fp) {
  char *txt=NULL;
  int n;
  struct OptionFile *ptr=NULL;

  ptr=malloc(sizeof(struct OptionFile));
  if (ptr==NULL) return NULL;
  ptr->argc=0;
  ptr->argv=NULL;

  while ((txt=OptionScanFileArg(fp)) !=NULL) {
    if (ptr->argv==NULL) {
      ptr->argv=malloc(sizeof(char **));
      if (ptr->argv==NULL) break;
    } else {
      char **tmp=NULL;
      tmp=realloc(ptr->argv,sizeof(char **)*(ptr->argc+1));
      if (tmp==NULL) break;
      ptr->argv=tmp;
    } 
    ptr->argv[ptr->argc]=txt;
    ptr->argc++;
   
  }
  if (txt !=NULL) {
    if (ptr->argv !=NULL) {
      for (n=0;n<ptr->argc;n++) if (ptr->argv[n] !=NULL) free(ptr->argv[n]);
      free(ptr->argv);
    } 
    free(ptr);
    return NULL;
  }
  return ptr;
}

void OptionFreeFile(struct OptionFile *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->argv !=NULL) {
    for (n=0;n<ptr->argc;n++) if (ptr->argv[n] !=NULL) free(ptr->argv[n]);
    free(ptr->argv);
  } 
  free(ptr);
  return;
}

void OptionFreeText(struct OptionText *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->txt !=NULL) {
    for (n=0;n<ptr->num;n++) if (ptr->txt[n] !=NULL) free(ptr->txt[n]);
    free(ptr->txt);
  } 
  free(ptr);
  return;
}

int OptionProcess(int offset,int argc,char *argv[], struct OptionData *opt,
					int (*opterr)(char *)) {

  int i=offset,k=offset;
  while ((i<argc) && (argv[i][0]=='-')) {
    int j=0;
    while ((j<opt->num) && 
           (strcmp(&argv[i][1],opt->ptr[j].name) !=0) ) j++;

    if (j==opt->num) {
      int s=0;
      if (opterr !=NULL) s=(*opterr)(&argv[i][1]);
      i++;
      if (s==0) continue;
      else return -1;
    }
    i++;
    opt->ptr[j].set=1;
    switch (opt->ptr[j].type) {
    case 'x'  : /* a flag */
        if (opt->ptr[j].ptr !=NULL) 
        *( (unsigned char *) opt->ptr[j].ptr)=(unsigned char) 1;
        break;
      case 'i' : /* vanilla integer */ 
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL)
        *( (int *) opt->ptr[j].ptr)=(int) atoi(argv[i]);
        i++;
        break;
      case 's' : /* short integer */
        if (i==argc) return -1;    
        if (opt->ptr[j].ptr !=NULL)
        *( (short int *) opt->ptr[j].ptr)=(short int) atoi(argv[i]);
        i++;
        break;
      case 'l' : /* long integer */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) 
        *( (long int *) opt->ptr[j].ptr)=(long int) atol(argv[i]);
        i++;
        break;
      case 'f' : /* float */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) 
        *( (float *) opt->ptr[j].ptr)=(float) atof(argv[i]);
        i++;
        break;
      case 'd' : /* double */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) 
        *( (double *) opt->ptr[j].ptr)=(double) atof(argv[i]);
        i++;
        break;
      case 't' : /* string */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) {
	  char **tptr;
          tptr=(char **) opt->ptr[j].ptr;
          if (*tptr !=NULL) free(*tptr);
          *tptr=malloc(strlen(argv[i])+1);
          if (*tptr==NULL) return -1;
   	  strcpy(*tptr,argv[i]);
	}
        opt->ptr[j].set=i; 
        i++;
        break;
        case 'a' : /* array of strings */
        if (i==argc) return -1;
        if (opt->ptr[j].ptr !=NULL) {
          struct OptionText *ptr;
          char **tmp;
          ptr=*(struct OptionText **) opt->ptr[j].ptr;
          if (ptr==NULL) {
	    ptr=malloc(sizeof(struct OptionText));
            if (ptr==NULL) return -1;
            ptr->num=0;
            ptr->txt=NULL;
            *(struct OptionText **) opt->ptr[j].ptr=ptr;
          }
          if (ptr->txt==NULL) ptr->txt=malloc(sizeof(char *));
          else {
            tmp=realloc(ptr->txt,sizeof(char *)*ptr->num+1);
	    if (tmp==NULL) return -1;
            ptr->txt=tmp;
	  }
          if (ptr->txt==NULL) return -1;
          ptr->txt[ptr->num]=malloc(strlen(argv[i])+1);
	  if (ptr->txt[ptr->num]==NULL) return -1;
          strcpy(ptr->txt[ptr->num],argv[i]);
          ptr->num++;
	}
        opt->ptr[j].set=i; 
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







int OptionAdd(struct OptionData *opt,char *name,char type,void *data) {
  
  struct OptionEntry *tmp; 
  char *buf;

  if (opt==NULL) return -1;

  buf=malloc(strlen(name)+1);
  if (buf==NULL) return -1;
  strcpy(buf,name);

 
  if (opt->ptr !=NULL) {
    tmp=realloc(opt->ptr,sizeof(struct OptionEntry)*(opt->num+1));
    if (tmp==NULL) {
      free(buf);
      return -1;
    }
    opt->ptr=tmp;
  } else {
    opt->ptr=malloc(sizeof(struct OptionEntry));
    if (opt->ptr==NULL) {
      free(buf);
      return -1;
    }
    opt->num=0; 
  }

  opt->ptr[opt->num].name=buf;
  opt->ptr[opt->num].set=0;
  opt->ptr[opt->num].type=type;
  opt->ptr[opt->num].ptr=data;
  opt->num++;
  return 0;
}
      
void OptionFree(struct OptionData *opt) {
  if (opt->ptr !=NULL) {
    int i;
    for (i=0;i<opt->num;i++) free(opt->ptr[i].name);
    for (i=0;i<opt->num;i++) if (opt->ptr[i].type=='t') {
      char **tptr;
      tptr=(char **) opt->ptr[i].ptr;
      if (*tptr !=NULL) free(*tptr);
    }
    free(opt->ptr);
  }
  

  opt->ptr=NULL;
}

 


