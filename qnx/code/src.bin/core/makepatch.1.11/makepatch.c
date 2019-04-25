/* makepatch.c
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

/*
 $Log: makepatch.c,v $
 Revision 1.11  2007/03/06 16:39:31  code
 Added code to correctly copy documentation files.

 Revision 1.10  2006/02/09 19:25:33  barnes
 Fixed bug in checking out library headers.

 Revision 1.9  2005/09/29 14:08:01  barnes
 Fixed bug in 2.6 kernels.

 Revision 1.8  2004/07/16 22:55:02  barnes
 headers are now placed under RCS.

 Revision 1.7  2004/07/09 21:20:19  barnes
 Added code for including documentation.

 Revision 1.6  2004/07/06 19:05:33  barnes
 Added missing header.

 Revision 1.5  2004/04/27 13:47:17  barnes
 Audit - Enforced warning set to "all".

 Revision 1.4  2002/02/26 19:26:24  barnes
 Fixed logging.

 Revision 1.3  2002/02/26 19:01:16  barnes
 Fixed bug in the test patch routine.

 Revision 1.2  2002/02/26 01:36:33  barnes
 Fixed bug in searching directory tree.

 Revision 1.1  2002/02/25 22:47:25  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifdef _QNX4
#include <process.h>
#endif

#include <dirent.h>
#include "hlpstr.h"
#include "errstr.h"

#define TMPFILE "/tmp/patch.tmp"

struct dirstr {
  char *name;
  char *version;
  int type;
};

int dnum=0;
struct dirstr dlst[8000];

char line[1024];
int vbflg=0;
char *postproc=NULL;

void log_info(char *text) {
  if (vbflg==1) fprintf(stderr,text);
  fprintf(stdout,text);
  fflush(stdout);
}

int filecmp(char *fileA,char *fileB) {
  FILE *fpA;
  FILE *fpB;
  int a,b;
  
  fpA=fopen(fileA,"r");
  if (fpA==NULL) return -1;
  fpB=fopen(fileB,"r");
  if (fpB==NULL) {
    fclose(fpA);
    return -1;
  }


  a=fgetc(fpA);
  b=fgetc(fpB);
  while ((a !=EOF) && (b !=EOF)) {
    if (a !=b) break;
    a=fgetc(fpA);
    b=fgetc(fpB);

  }
  fclose(fpA);
  fclose(fpB);
  if ((a==EOF) && (b==EOF)) return 0;
  return -1;
}



int do_command(char *command) {
  int status;
  char tmpname[256];
  time_t ltime;
  pid_t pid;
  
  pid=getpid();
  time(&ltime);  

  sprintf(tmpname,"log.%d.%d",pid,(int) ltime);
 
  pid=fork();
  if (pid==-1) return -1;
  if (pid==0) { /* execute the make command here */
    int s;

    char *argv[4];
    argv[0] = "sh";
    argv[1] = "-c";
    argv[2] = command;
    argv[3] = NULL;
    freopen(tmpname,"a+",stderr);
    freopen(tmpname,"a+",stdout);
    s=execvp("/bin/sh",argv); 
    exit(s);
  }
  do {
     if (waitpid(pid, &status, 0) == -1) {
       FILE *fp;
       /* now display log message and delete */
       fp=fopen(tmpname,"r");
       while(fgets(line,256,fp) !=NULL) {
         log_info(line);
       }
       fclose(fp);
       unlink(tmpname);
       return status;
     } 
  } while(1);
}



void free_dlst() {
  int i;
  for (i=0;i<dnum;i++) {
    if (dlst[i].name !=NULL) free(dlst[i].name);
    if (dlst[i].version !=NULL) free(dlst[i].version);
    dlst[i].name=NULL;
    dlst[i].version=NULL;
  }
  dnum=0;
}


void test_patch(char *src,char *cmp) {
  int i;
  char dpath[1024];
  char spath[1024]; 
  char fpath[1024];
  char sys[1024];
  char *tmpfile=TMPFILE;
  int s;
  struct dirent *direntp=NULL;
  DIR *dirp;
  struct stat dstat;
 
 
  /* first exclude from our list any library or binary
   * directories that have not been updated.
   */
 
  for (i=0;i<dnum;i++) {
    if ((dlst[i].type==0) || (dlst[i].version==NULL)) continue;
    sprintf(dpath,"%s/%s.%s",cmp,dlst[i].name,dlst[i].version);
    dirp=opendir(dpath);
    if (dirp !=NULL) dlst[i].type=-1;
    if (dirp !=NULL) closedir(dirp);
  }
  
  /* now compare the contents of other directories - based on the
   * copy directory routine.
   */

  for (i=0;i<dnum;i++) {
    if (dlst[i].type !=0) continue;
    sprintf(dpath,"%s/%s",src,dlst[i].name);
    dirp=opendir(dpath);
  
    while ((direntp=readdir(dirp)) !=NULL) {
      if (direntp->d_name[0]=='.') continue;
      /* this is a fudge to exclude temporary files and backups */

      s=strlen(direntp->d_name);
      if (direntp->d_name[s-1]=='~') continue;
      if ((direntp->d_name[0]=='#') && (direntp->d_name[s-1]=='#')) continue;
      sprintf(spath,"%s/%s",dpath,direntp->d_name);
      s=stat(spath,&dstat);
      if ((s==0) && (S_ISDIR(dstat.st_mode))) continue;
      
      /* okay check to see if the file exists in the old version */
      sprintf(fpath,"%s/%s/%s",cmp,dlst[i].name,direntp->d_name);
      s=stat(fpath,&dstat);
      fprintf(stderr,"%s\n",fpath);      
      if (s==-1) break; /* no point going on - we're missing this file */

      /* okay forces to do a byte wise comparison.
       * If we have a post-processor - we have to run that on 
       * the archive to get a good match. 
       */

     if (postproc==NULL) 
       sprintf(sys,"cat %s > %s",spath,tmpfile);
      else
       sprintf(sys,"cat %s | %s > %s",spath,postproc,tmpfile);
      s=do_command(sys);
      if (s !=0) break; /* failed so assume we've got to do the copy */

      /* now compare the two files */
      if (filecmp(spath,fpath) !=0) break;   
    }
    if (direntp==NULL) dlst[i].type=-1;
    closedir(dirp);
  }
}


int dirtype(char *path) {  
  int s=0;
  int type=0;
  struct dirent *direntp=NULL;
  DIR *dirp; 

  dirp=opendir(path);
  if (dirp==NULL) return 0;
 
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    if (strcmp(direntp->d_name,"RCS")==0) s+=2;
    if (strcmp(direntp->d_name,"doc")==0) s+=1;
  }
  if (s>1) {
    type=(s==2) ? 1 : 2;
    closedir(dirp);
    return type;
  }

  rewinddir(dirp);

  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    if (strcmp(direntp->d_name,"include")==0) s+=2;
    if (strcmp(direntp->d_name,"src")==0) s+=4;
    if (strcmp(direntp->d_name,"doc")==0) s+=1;
    
  }
  closedir(dirp);
  if (s>=6) type=(s==6) ? 3: 4;
  
  return type;
}


void dirfind(char *root,char *path) {
  int type=0;
  char fpath[1024]; 
  struct dirent *direntp=NULL;
  DIR *dirp;

  struct stat dstat;
  sprintf(fpath,"%s/%s",root,path);
  dirp=opendir(fpath);
  if (dirp==NULL) return;
  
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    sprintf(fpath,"%s/%s/%s",root,path,direntp->d_name);
    stat(fpath,&dstat);
    if (S_ISDIR(dstat.st_mode)) {

       /* determine what kind of directory source,library source, regular */

       type=dirtype(fpath);

       sprintf(fpath,"%s/%s",path,direntp->d_name);

       dlst[dnum].name=malloc(strlen(fpath)+1);
       
       dlst[dnum].type=type;
       strcpy(dlst[dnum].name,fpath);
       dnum++;

       if (type==0) dirfind(root,fpath);
    }
  }
  closedir(dirp);
}   


int make_branch(char *path) {
  int i=0,s;
  struct stat dstat;

  char logtxt[256];

  /* walk a pathname creating a directories as necessary */

  while (path[i] !=0) { 

    if ((path[i]=='/') && (i>0)) { 
       path[i]=0;

       s=stat(path,&dstat); 
       if (s !=0) {
         sprintf(logtxt,"Making directory:%s\n",path);  
         log_info(logtxt); 
         mkdir(path,
         S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
         s=stat(path,&dstat); 
         if (s !=0) return -1;
       }     
       path[i]='/';
    }
    i++;
  }     
  return 0;
}

int make_tree(char *root) {
  int i;
  char fpath[1024];
  for (i=0;i<dnum;i++) {
    if (dlst[i].type==-1) continue;
    if (dlst[i].type <3) {
      /* make a regular directory */

      if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/",root,dlst[i].name);
      else sprintf(fpath,"%s/%s.%s/",root,dlst[i].name,dlst[i].version); 
      if (make_branch(fpath) !=0) return -1;
      if (dlst[i].type>1) {
        if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/doc/",
					   root,dlst[i].name);
        else  sprintf(fpath,"%s/%s.%s/doc/",
		      root,dlst[i].name,dlst[i].version);
        if (make_branch(fpath) !=0) return -1;
      }


    } else {

      /* make the src and include subdirectories of a library */

      if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/src/",root,dlst[i].name);
      else  sprintf(fpath,"%s/%s.%s/src/",root,dlst[i].name,dlst[i].version);
      if (make_branch(fpath) !=0) return -1;
      if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/include/",
                                         root,dlst[i].name);
      else  sprintf(fpath,"%s/%s.%s/include/",
                    root,dlst[i].name,dlst[i].version);

      if (make_branch(fpath) !=0) return -1;
      if (dlst[i].type>3) {
        if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/doc/",
                                         root,dlst[i].name);
        else  sprintf(fpath,"%s/%s.%s/doc/",
                    root,dlst[i].name,dlst[i].version);
        if (make_branch(fpath) !=0) return -1;
      }

    }
  }
  return 0;
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

char *get_vtxt(FILE *fp) {
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

int decode_vtxt(char *txt,int *major,int *minor) {
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


int make_version(char *root) {
  int i,s;
  int major,minor;
  struct dirent *direntp=NULL;
  DIR *dirp;
  FILE *fp;
  char *vtxt=NULL;
  char vname[1024]="";
  char vfname[1024];
  char fpath[1024];
  char sys[1024];
  char tmp[16]; 
 
  time_t ltime;
  pid_t pid;
 


  for (i=0;i<dnum;i++) {
    if (dlst[i].type==-1) continue; /* excluded directory */
    if (dlst[i].type==0) continue; /* not a source directory */
    if (dlst[i].type<3) sprintf(fpath,"%s/%s/RCS",root,dlst[i].name);
    if (dlst[i].type>2) sprintf(fpath,"%s/%s/src/RCS",root,dlst[i].name);
 
      
    dirp=opendir(fpath);
    if (dirp==NULL) continue;
   
    while ((direntp=readdir(dirp)) !=NULL) {
      if (direntp->d_name[0]=='.') continue;
      if (strncmp(direntp->d_name,"version.info",12)==0) break;
    }
    if (direntp !=NULL) strcpy(vname,direntp->d_name);
    closedir(dirp);
    if (direntp==NULL) continue;

    pid=getpid();
    time(&ltime);  

    sprintf(vfname,"/tmp/version.%d.%d",pid,(int) ltime);

    if (postproc==NULL)     
      sprintf(sys,"co -p %s/%s > %s",fpath,vname,vfname);
    else 
      sprintf(sys,"co -p %s/%s | %s > %s",
              fpath,vname,postproc,vfname);

    s=do_command(sys);
    if (s !=0) return -1;
    fp=fopen(vfname,"r");
    if (fp==NULL) return -1;

    vtxt=get_vtxt(fp);
    fclose(fp);
    unlink(vfname);
    if (vtxt !=NULL) {
      decode_vtxt(vtxt,&major,&minor);
      sprintf(tmp,"%d.%.2d",major,minor);
      dlst[i].version=malloc(strlen(tmp)+1);
      strcpy(dlst[i].version,tmp);
      free(vtxt);
    }
  
  }
  return 0;

}

int co_dir(char *src,char *dst) {
  char *sfx;
  char spath[1024];
  char dpath[1024];
  char sys[1024];

  struct dirent *direntp=NULL;
  DIR *dirp;
   
  int s;
  sprintf(spath,"%s/RCS",src); 
  dirp=opendir(spath);
  if (dirp==NULL) return 0;
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    sprintf(spath,"%s/RCS/%s",src,direntp->d_name);

    sfx=strstr(direntp->d_name,",v");
    if (sfx !=NULL) sfx[0]=0;
    sprintf(dpath,"%s/%s",dst,direntp->d_name);
    if (postproc==NULL)
      sprintf(sys,"co -p %s > %s",spath,dpath);
    else
      sprintf(sys,"co -p %s | %s > %s",spath,postproc,dpath);

    s=do_command(sys);
    if (s !=0) {
      closedir(dirp);
      return -1;
    }  

  }
  closedir(dirp);
  return 0;
}



int copy_dir(char *src,char *dst,char *ptn,int pflg) {
  char spath[1024];
  char dpath[1024];
  char sys[1024];
  int s;
  struct dirent *direntp=NULL;
  DIR *dirp;
  struct stat dstat;

  dirp=opendir(src);
  if (dirp==NULL) return 0;
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;

    /* this is a fudge to exclude temporary files and backups */

    s=strlen(direntp->d_name);
    if (direntp->d_name[s-1]=='~') continue;
    if ((direntp->d_name[0]=='#') && (direntp->d_name[s-1]=='#')) continue;

    if ((ptn !=NULL) && (strstr(direntp->d_name,ptn)==NULL)) continue; 
    sprintf(spath,"%s/%s",src,direntp->d_name);
    s=stat(spath,&dstat); 
    if ((s==0) && (! S_ISDIR(dstat.st_mode))) {
      sprintf(dpath,"%s/%s",dst,direntp->d_name);
      if ((postproc !=NULL) && (pflg==1))
        sprintf(sys,"cat %s | %s > %s",spath,postproc,dpath);
      else 
        sprintf(sys,"cp %s %s",spath,dpath);
      s=do_command(sys);
      if (s !=0) {
         closedir(dirp);
         return -1;
      }
    }
  }
  return 0; 
}


int copy_example(char *src,char *dst) {
  char spath[1024];
  char dpath[1024];
  int s;
  struct dirent *direntp=NULL;
  DIR *dirp;
  struct stat dstat;
 
  dirp=opendir(src);
  if (dirp==NULL) return 0;
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    sprintf(spath,"%s/%s",src,direntp->d_name);
    s=stat(spath,&dstat); 
    if ((s==0) && (S_ISDIR(dstat.st_mode))) {
      sprintf(dpath,"%s/%s",dst,direntp->d_name);
      mkdir(dpath,
          S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
       s=stat(dpath,&dstat); 
       if (s !=0) return -1;
       if (copy_dir(spath,dpath,"makefile",1) !=0) return -1; 
       if (copy_dir(spath,dpath,".h",1) !=0) return -1;
       if (co_dir(spath,dpath) !=0) return -1;
    }
  }
  closedir(dirp);
  return 0;

}

int copy_all(char *src,char *dst) {
  int i,s;
  char spath[1024];
  char dpath[1024];
  char tspath[1024];
  char tdpath[1024];
  char logtxt[1024];

  for (i=0;i<dnum;i++) {
    if (dlst[i].type==-1) continue;
    sprintf(spath,"%s/%s",src,dlst[i].name);
 
    if (dlst[i].version==NULL) sprintf(dpath,"%s/%s",dst,dlst[i].name);
    else sprintf(dpath,"%s/%s.%s",dst,dlst[i].name,dlst[i].version);

    log_info("Copying Directory:\n");
    sprintf(logtxt,"From:%s\n",spath);
    log_info(logtxt);
    sprintf(logtxt,"To:%s\n",dpath); 
    log_info(logtxt);

    if ((dlst[i].type==0) && (copy_dir(spath,dpath,NULL,1) !=0)) return -1;
    if ((dlst[i].type>0) && (dlst[i].type<3)) { /* copy source code */

       if (copy_dir(spath,dpath,"makefile",1) !=0) return -1; 
       /*
         if (copy_dir(spath,dpath,".h") !=0) return -1;
       */
       if (co_dir(spath,dpath) !=0) return -1;

       if (dlst[i].type>1) {
         struct stat dstat;
         log_info("doc:\n");
	 sprintf(tdpath,"%s/doc",dpath);
	
         sprintf(tspath,"%s/doc/rplot",spath);
         s=stat(tspath,&dstat); 
    
         if ((s==0) && (S_ISDIR(dstat.st_mode))) {
         
           sprintf(tdpath,"%s/doc/rplot",dpath);
           mkdir(tdpath,
           S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
           s=stat(tdpath,&dstat); 
           if (s !=0) return -1;     
           if (copy_dir(tspath,tdpath,NULL,0) !=0) return -1;
         }

	 sprintf(tspath,"%s/doc/postscript",spath);
	 s=stat(tspath,&dstat);

	 if ((s==0) && (S_ISDIR(dstat.st_mode))) {
	 
	   sprintf(tdpath,"%s/doc/postscript",dpath);
	   mkdir(tdpath,
		 S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	   s=stat(tdpath,&dstat);
	   if (s !=0) return -1;
	   if (copy_dir(tspath,tdpath,NULL,0) !=0) return -1;
	 }

	 sprintf(tspath,"%s/doc/ppm",spath);
	 s=stat(tspath,&dstat);

	 if ((s==0) && (S_ISDIR(dstat.st_mode))) {
	 
	   sprintf(tdpath,"%s/doc/ppm",dpath);
	   mkdir(tdpath,
		 S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	   s=stat(tdpath,&dstat);
	   if (s !=0) return -1;
	   if (copy_dir(tspath,tdpath,NULL,0) !=0) return -1;
	 }


       }

    } 
    if (dlst[i].type>2) { /* copy library source code */
      log_info("include:\n");
      sprintf(tspath,"%s/include",spath);
      sprintf(tdpath,"%s/include",dpath);
      if (co_dir(tspath,tdpath) !=0) return -1;
      /*
        if (copy_dir(tspath,tdpath,NULL) !=0) return -1;
      */
      log_info("src:\n");
      sprintf(tspath,"%s/src",spath);
      sprintf(tdpath,"%s/src",dpath);
      if (copy_dir(tspath,tdpath,"makefile",1) !=0) return -1;
      /* 
        if (copy_dir(tspath,tdpath,".h") !=0) return -1;
      */
      if (co_dir(tspath,tdpath) !=0) return -1;

      if (dlst[i].type>3) {
        struct stat dstat;

        log_info("doc:\n");
        sprintf(tspath,"%s/doc",spath);
        sprintf(tdpath,"%s/doc",dpath);
        if (co_dir(tspath,tdpath) !=0) return -1;
        sprintf(tspath,"%s/doc/src",spath);
        s=stat(tspath,&dstat); 
       
        if ((s==0) && (S_ISDIR(dstat.st_mode))) {
           
          sprintf(tdpath,"%s/doc/src",dpath);
          mkdir(tdpath,
          S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
          s=stat(tdpath,&dstat); 
          if (s !=0) return -1;     
          if (copy_example(tspath,tdpath) !=0) return -1; 
	}

        sprintf(tspath,"%s/doc/rplot",spath);
        s=stat(tspath,&dstat); 
       
        if ((s==0) && (S_ISDIR(dstat.st_mode))) {
        
         sprintf(tdpath,"%s/doc/rplot",dpath);
          mkdir(tdpath,
          S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
          s=stat(tdpath,&dstat); 
          if (s !=0) return -1;     
          if (copy_dir(tspath,tdpath,NULL,0) !=0) return -1;
	}

        sprintf(tspath,"%s/doc/postscript",spath);
        s=stat(tspath,&dstat); 
       
        if ((s==0) && (S_ISDIR(dstat.st_mode))) {
        
         sprintf(tdpath,"%s/doc/postscript",dpath);
          mkdir(tdpath,
          S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
          s=stat(tdpath,&dstat); 
          if (s !=0) return -1;     
          if (copy_dir(tspath,tdpath,NULL,0) !=0) return -1;
	}
 
        sprintf(tspath,"%s/doc/ppm",spath);
        s=stat(tspath,&dstat); 
       
        if ((s==0) && (S_ISDIR(dstat.st_mode))) {
        
         sprintf(tdpath,"%s/doc/ppm",dpath);
          mkdir(tdpath,
          S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
          s=stat(tdpath,&dstat); 
          if (s !=0) return -1;     
          if (copy_dir(tspath,tdpath,NULL,0) !=0) return -1;
	}

      }
    }    
    log_info("\n");
  }
  return 0;
}     
 

void makedst(char *src,char *dst,char *cmp,char *pkg) {
  char fpath[1024];
  char cpath[1024];
  char dpath[1024];
  char *tmpfile=TMPFILE;
  char logtxt[1024];
  char sys[1024];

  struct stat dstat;


  int type,s;
  
  dnum=0;

  sprintf(fpath,"%s/%s",src,pkg);

  s=stat(fpath,&dstat); 
  if ((s==0) && (S_ISDIR(dstat.st_mode))) {
    sprintf(logtxt,"Package:%s\n",pkg);
    log_info(logtxt);

    type=dirtype(fpath);

    dlst[dnum].name=malloc(strlen(pkg)+1);     
    dlst[dnum].type=type;
    strcpy(dlst[dnum].name,pkg);
    dnum++;
    if (type<=0) dirfind(src,pkg);

    if (make_version(src) !=0) {
       log_info("Error generating version information.\n");
       exit(1);
    }
    
    test_patch(src,cmp);
    
    if (make_tree(dst) !=0) {
      log_info("Error making directory structure.\n");
      exit(1);
    }
    
    if (copy_all(src,dst) !=0) { 
       log_info("Error copying files.\n");
       exit(1);
    }
    free_dlst();
   
  } else {
     sprintf(logtxt,"File:%s\n",pkg);
     log_info(logtxt);
      
     if (postproc==NULL)       
       sprintf(sys,"cat %s > %s",fpath,tmpfile);
      else
	sprintf(sys,"cat %s | %s > %s",fpath,postproc,tmpfile);
     s=do_command(sys);

     s=filecmp(cpath,tmpfile);
     if (s !=0) return;

     sprintf(dpath,"%s/%s",dst,pkg);

     log_info("Copying File:\n");
     sprintf(logtxt,"From:%s\n",fpath);
     log_info(logtxt);
     sprintf(logtxt,"To:%s\n",dpath); 
     log_info(logtxt);
     if (postproc==NULL) 
        sprintf(sys,"cat %s > %s",fpath,dpath);
      else
       sprintf(sys,"cat %s | %s > %s",fpath,postproc,dpath);
      s=do_command(sys);
     
  }

}

void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}



int main(int argc,char *argv[]) {
  int i,j;
  char *src;
  char *dst;
  char *cmp;
  char line[1024];
  char logtxt[256];

  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-vb")==0) vbflg=1;
    else if (strcmp(argv[i],"-p")==0) {
      postproc=argv[i+1];
      i++;

    } else if (strcmp(argv[i],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } else break;
  }
 

  if (argc<(i+4)) {
     print_info(stderr,errstr);
     exit(1);
  }

  log_info("makepatch\n");

  if (postproc !=NULL) {
    sprintf(logtxt,"Post Processor:%s\n",postproc);
    log_info(logtxt);
  }
 

  src=argv[i];
  dst=argv[i+1];
  cmp=argv[i+2];
  if (strcmp(argv[i+3],"stdin")==0) {
    /*read from stdin */
    while (fgets(line,1024,stdin) !=NULL) {
      for (i=0;(line[i] !=0) && (line[i]==' ');i++); 
      if (line[i] ==0) continue;
      memmove(line,line+i,strlen(line)+1-i);
      for (i=0;(line[i] !=0) && (line[i] !='\n') && (line[i] !=' ');i++); 
      line[i]=0; 
      if (strlen(line)==0) continue;
      makedst(src,dst,cmp,line);
    }
  } else {
    for (j=i+3;j<argc;j++) {
      makedst(src,dst,cmp,argv[j]);
    }
  }
  return 0;

 

}
  




