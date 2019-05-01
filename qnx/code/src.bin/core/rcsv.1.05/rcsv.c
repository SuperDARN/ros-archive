/* rcsv.c
   ======
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
 $Log: rcsv.c,v $
 Revision 1.10  2004/04/27 13:48:11  barnes
 Audit - Enforced warning set to "all".

 Revision 1.9  2003/03/13 04:20:07  barnes
 Added "-h" option to generate a version header.

 Revision 1.8  2001/06/27 18:09:34  barnes
 Added place holder for license.

 Revision 1.7  2001/01/29 18:28:24  barnes
 Tidied up revision logging.

 Revision 1.6  2001/01/29 18:12:11  barnes
 Added Author Name

 Revision 1.5  2000/08/22 14:40:13  barnes
 Added return to main.

 Revision 1.4  2000/08/16 22:17:54  barnes
 Changed return type of main().

 Revision 1.3  2000/08/07 14:06:30  barnes
 Added help message.

 Revision 1.2  2000/04/18 16:16:54  barnes
 Fixed bug that no errors were ever generated.

 Revision 1.1  1999/12/16 22:14:42  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hlpstr.h"

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

char *get_revision(FILE *fp) {
  
  char lne[256];
  char *txt=NULL;  
  int state=0;
  int rpos=0;
  while ((state !=2) && (fgets(lne,255,fp) !=NULL)) {
    if ((state==0) && (instr(lne,"$Log:")!=-1)) state=1;
    else if ((state==1) && ((rpos=instr(lne,"Revision"))!=-1)) state=2;
  }
  if (state==2) {
    txt=malloc(strlen(lne+rpos)+1);
    strcpy(txt,lne+rpos);
    return txt;
  }
  return NULL;
}

int decode_revision(char *txt,int *major,int *minor) {
  int i=9;
  int j; 
  if (strlen(txt)<9) return -1;
  for (j=i;(txt[j] !=0) && (txt[j] !='.');j++);
  if (txt[j]==0) return -1;
  txt[j]=0;
  *major=atoi(txt+i);
  for (i=j+1;(txt[i] !=0) && (txt[i] !=' ');i++);
  if (txt[i]==0) return -1;
  txt[i]=0;
  *minor=atoi(txt+j+1);
  return 0;
}

int test_revision(FILE *fp) {
  char lne[256];
  int i,j;
  int state=0;
  FILE *tfp;
  char *txt=NULL;
  int major,minor;
  int major_t,minor_t;
  char fname[256];
  while(fgets(lne,255,fp) !=NULL) {
    for (i=0;(lne[i] !=0) && (lne[i]==' ');i++);
    if (lne[i]=='#') continue;
    if (lne[i]==0) continue;
    for (j=i;(lne[j] !=0) && (lne[j] !=' ');j++); 
    strncpy(fname,lne+i,j-i);
    fname[j-i]=0;
    if (lne[j]==0) continue;
    for (i=j;(lne[i] !=0) && (lne[i]==' ');i++); /* determine major start */
    if (lne[i]==0) continue;
    for (j=i;(lne[j] !=0) && (lne[j]!='.');j++); /* determine major end */
    if (lne[j]==0) continue;
    lne[j]=0;
    major=atoi(lne+i);
    for (i=j+1;(lne[i] !=0) && (lne[j] !=' ');i++);
    minor=atoi(lne+j+1);
    fprintf(stderr,"Checking file:%s Version:%d.%.2d\n",fname,major,minor);
    tfp=fopen(fname,"r");
    if (tfp==NULL) {
      fprintf(stderr,"File not found.\n");
      state=-1;
      break;
    }
    txt=get_revision(tfp);
    fclose(tfp);
    if (txt==NULL) {
      fprintf(stderr,"Log message not found.\n");
      state=-1;
      break;
    }
    if (decode_revision(txt,&major_t,&minor_t) !=0) {
      fprintf(stderr,"Failed to decode revision.\n");
      free(txt);
      state=-1;
      break;
    }

    free(txt);
    

    if ((major_t !=major) || (minor_t !=minor)) {
      fprintf(stderr,"Versions disagree %d.%.2d != %d.%.2d.\n",
              major,minor,major_t,minor_t);
      state=-1;
      break;
    } else fprintf(stderr,"Versions agree.\n");
    
  }
  return state;
}

void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}



int main(int argc,char *argv[]) {
  char *txt;
  FILE *fp;
  int major,minor;
  int i;
  
  if (argc<2) { 
    fprintf(stderr,"At least one argument must be supplied.\n");
    exit(-1);
  }
  if (argv[1][0] !='-') {
    /* get the version info from a group of files */
    for (i=1;i<argc;i++) {
      fp=fopen(argv[i],"r");
      if (fp==NULL) continue;
      txt=get_revision(fp);
      if ((txt !=NULL) && (decode_revision(txt,&major,&minor)==0)) 
      fprintf(stdout,"%s %d.%.2d\n",argv[i],major,minor);
    
      fclose(fp);
    }
  } else {
    int state=0;
    if (strcmp(argv[1],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    }
    if (argc>2) {
      fp=fopen(argv[2],"r");
      if (fp==NULL) {
        fprintf(stdout,"could not locate version file.\n");
        exit(1);
      }
    } else fp=stdin;

     txt=get_revision(fp);
     if (txt==NULL) {
       fprintf(stderr,"failed to locate master version number.\n");
       exit(1);
     }

     if (decode_revision(txt,&major,&minor) !=0) {
       fprintf(stderr,"failed to decode master version number.\n"); 
       exit(1);
     }

     if (argv[1][1]=='t') { /* test for revision agreement */
        state=test_revision(fp);
        if (state !=0) exit(1);
     }
     if (argv[1][1]=='v') { /* print master version number */
        fprintf(stdout,"%d.%.2d",major,minor);
     }
     if (argv[1][1]=='h') { /* print version header */
       fprintf(stdout,"/* version.h\n");
       fprintf(stdout,"   ========= */\n\n");
       fprintf(stdout,"#define MAJOR_VERSION %d\n",major);
       fprintf(stdout,"#define MINOR_VERSION %d\n",minor);
     }



  }   
  return 0;
}



