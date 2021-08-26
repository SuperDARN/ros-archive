/* insthdr.c
   =========
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
 $Log: insthdr.c,v $
 Revision 1.8  2004/04/27 13:26:32  barnes
 Audit - Enforced warning set to "all".

 Revision 1.7  2001/06/27 18:08:38  barnes
 Added place holder for license.

 Revision 1.6  2001/02/02 20:15:15  barnes
 Fixed missing return.

 Revision 1.5  2001/01/30 20:55:12  barnes
 Added help message.

 Revision 1.4  2001/01/29 18:12:11  barnes
 Added Author Name

 Revision 1.3  2001/01/26 17:51:44  barnes
 Bug fix to make sure the root package name is an exact match.

 Revision 1.2  2001/01/25 23:37:45  barnes
 Major modification to support a list of package directories on the command line.

 Revision 1.1  2001/01/24 20:21:06  barnes
 Initial revision
 

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "hlpstr.h"
#include "errstr.h"


int vflg=1; /* compile only most recent version of code */
int num=0;
int lnum=0;
char *inclist[4096];
char *incname[4096];
char *incroot[4096];
int incvmajor[4096];
int incvminor[4096];

char *src=NULL;
char *dst=NULL;


char line[256];
int vbflg=0;


void log_info(char *text) {
  if (vbflg==1) fprintf(stderr,text);
  fprintf(stdout,text);
  fflush(stdout);
}




void add_include(char *name) {

  int i,j,k;
  int major=0,minor=0;
 
  i=strlen(src)+1;  

  for (j=strlen(name)-1;(name[j] !='.') && (j>0);j--);
  if (j>0) {
    minor=atoi(name+j);
    for (k=j-1;(name[k] !='.') && (k>0);k--);
    if (k>0) {
      name[j]=0;
      major=atoi(name+k);
      name[j]='.';
      j=k;
    } else {
      minor=0;
      j=strlen(name);
    }
  } else j=strlen(name);


  if (vflg==1) {
    for (k=0;k<num;k++) {
      if ((strlen(incroot[k])==(j-i)) && 
          (strncmp(name+i,incroot[k],j-i)==0)) break;
    }  
    if (k<num) {
      
      /* now compare version numbers */
      if (major<incvmajor[k]) return;
      if (minor<incvminor[k]) return;
  
      incvmajor[k]=major;
      incvminor[k]=minor;
      incname[k]=realloc(incname[k],strlen(name)+1);
      strcpy(incname[k],name);
      return;
    }
  }
  incname[num]=malloc(strlen(name)+1);
  strcpy(incname[num],name);
  name[j]=0;
  incroot[num]=malloc(strlen(name+i)+1);
  strcpy(incroot[num],name+i);
 
  num++;

}


int make_link(char *src,char *dst) {
  char x[1024],y[1024];
  char logstr[1024];
  struct dirent *direntp=NULL;
  DIR *dirp;
  int status=0;
  dirp=opendir(src);
  if (dirp==NULL) return 0;


  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    sprintf(x,"%s/%s",src,direntp->d_name);
    sprintf(y,"%s/%s",dst,direntp->d_name);
    sprintf(logstr,"%s -> %s\n",x,y);
    log_info(logstr);
    unlink(y);
    status=symlink(x,y);
    if (status !=0) break;
  }
  closedir(dirp);
  return status;
}

int find_include(char *path) {
  struct dirent *direntp=NULL;
  DIR *dirp;
  int s; 
  struct stat dstat;
  char spath[4096];


  dirp=opendir(path);
  if (dirp==NULL) return 0;
 
  while ((direntp=readdir(dirp)) !=NULL) {
     if (direntp->d_name[0]=='.') continue;
     if (strcmp(direntp->d_name,"include")==0) add_include(path);
     sprintf(spath,"%s/%s",path,direntp->d_name);
     s=stat(spath,&dstat); 
    
     if ((s==0) && (S_ISDIR(dstat.st_mode))) {
       /* recurse here */
       find_include(spath);
     }
  }
  closedir(dirp);
  return 0;
}


void load_list(FILE *fp) {
  int i;
  char line[1024];
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i]==' ') && (line[i] !=0);i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

    memmove(line,line+i,strlen(line)+1-i);
    for (i=0;(line[i] !=0) && (line[i] !='\n') && (line[i] !=' ');i++); 
    line[i]=0; 
    if (strlen(line)==0) continue;
    inclist[lnum]=malloc(strlen(line)+1);
    strcpy(inclist[lnum],line);
    lnum++;

  } 
}


void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}



int main(int argc,char *argv[]) {
  
  char path[4096];
  char logstr[1024];

  char sep[256];
  int i,j,status,s;
  int aflg=1;
  
  struct stat dstat;


  for (i=0;i<80;i++) sep[i]='=';
  sep[i]='\n';
  sep[i+1]=0;



  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-a")==0) vflg=0;
    else if (strcmp(argv[i],"-q")==0) aflg=0;
    else if (strcmp(argv[i],"-vb")==0) vbflg=1;
    else if (strcmp(argv[i],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } else break;
  }

  if (argc<(2+i)) {
     print_info(stderr,errstr);
     exit(1);
  }

  src=argv[i];
  dst=argv[i+1];
  

 if (argc>(i+2)) {
    if (strcmp(argv[i+2],"stdin")==0) load_list(stdin);
    else {
      for (j=0;j<(argc-i-2);j++) inclist[j]=argv[j+i+2];
      lnum=j;      
    }
  }

  

  find_include(src);
  log_info("insthdr\n");

  sprintf(logstr,"Package Source Directory:%s\n",src);
  log_info(logstr);
  sprintf(logstr,"Destination Directory:%s\n",dst);
  log_info(logstr);

  if (aflg !=0) sprintf(logstr,"Abort on failure:yes\n");
  else sprintf(logstr,"Abort on failure:no\n");

  log_info(logstr);

 
  if (vflg !=0) sprintf(logstr,
                "Link most recent version of headers:yes\n");
  else sprintf(logstr,"Link most recent version of headers:no\n");

  log_info(logstr);
  log_info(sep);


  if (lnum !=0) {
    sprintf(logstr,"Installation Order List:\n");
    log_info(logstr);
    for  (i=0;i<lnum;i++) {
      sprintf(logstr,"%d:%s\n",i+1,inclist[i]);
      log_info(logstr);
    }
  }
 
  if (lnum==0) {
    lnum=num;
    for (i=0;i<num;i++) inclist[i]=incroot[i];
  }

  log_info("Located Libraries:\n");
 
  for (i=0;i<lnum;i++) {
    for (j=0;j<num;j++) if (strcmp(inclist[i],incroot[j])==0) break;
    if (j==num) continue;
    sprintf(logstr,"%d.%s\n",i+1,incname[j]);
    log_info(logstr);
  }
  log_info(sep);
 
  

  s=stat(dst,&dstat); 
  if (s !=0) {
    log_info("Creating destination directory.\n");
    mkdir(dst,
       S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    s=stat(dst,&dstat); 
  }

  
  if (s !=0) {
    log_info("Could not create destination directory.\n");
    exit(-1);
  }

  if ((s==0) && !(S_ISDIR(dstat.st_mode))) {
    log_info("Destination is not a directory.\n");
    exit(-2);
  }
    


  for (i=0;i<lnum;i++) {

    for (j=0;j<num;j++) if (strcmp(inclist[i],incroot[j])==0) break;
    if (j==num) continue;

    sprintf(path,"%s/include",incname[j]);
    status=make_link(path,dst);
   

    if ((aflg==1) && (status !=0)) break;
  } 


 
  if (i<lnum) log_info("Linking Aborted.\n");
 
   


  return 0;
}
 


  




