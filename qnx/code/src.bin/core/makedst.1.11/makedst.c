/* makedst.c
   =========
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
 $Log: makedst.c,v $
 Revision 1.11  2001/07/06 21:22:58  barnes
 Modification to allow files to be included in the package list.

 Revision 1.10  2001/06/27 18:35:15  barnes
 Added the ability to post-process the source code.

 Revision 1.9  2001/01/30 21:46:52  barnes
 Added help messages.

 Revision 1.8  2001/01/29 18:12:11  barnes
 Added Author Name

 Revision 1.7  2001/01/24 22:53:00  barnes
 Modified the way the temporary filename is generated to make it
 platform independent.

 Revision 1.6  2001/01/24 20:30:42  barnes
 Changed the way directories are constructed.

 Revision 1.5  2001/01/24 18:19:48  barnes
 Added more explicit logging.

 Revision 1.4  2000/10/23 21:46:22  barnes
 Fixed bug in decoding the package list.

 Revision 1.3  2000/08/22 14:40:13  barnes
 Added return to main.

 Revision 1.2  2000/08/16 22:17:30  barnes
 Changed return type of main().

 Revision 1.1  2000/07/07 16:30:18  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "hlpstr.h"
#include "errstr.h"




/* makedst
 * =======
 *
 * Syntax:
 *         makedst [-vb] src dst pkg...
 *
 * Where:
 *         -vb    log to stderr as well as stdout
 *         src    source directory to search
 *         dst    destination directory to store distribution 
 *         pkg    list of packages  (sub-directories) to store. If pkg
 *                equals stdin then packages are read from standard in 
 *
 * The purpose of the makedst program is to make a distribution copy of 
 * a master archive. 
 *
 * The master archive is stored in the directory specified by src,
 * eg. "codebase". The program duplicates "packages" which are sub-directories
 * used to classify either data or software. A package groups together similar
 * code and often contains several levels of sub-directories.
 * A package can contain one of three directory types: 
 *
 * 1. plain  -    part of a larger tree or containing  data and shell scripts
 * 2. src    -    containing an "RCS" sub-directory indicating they contain
 *                source code under revision control.
 * 3. libsrc -    containing a "src" and "include" directory indication they
 *                contain the source code for a library.
 *
 * If a plain directory is found in a package it is duplicated in the 
 * destination directory.
 * If a src or libsrc directory is found then makedst
 * attempts to find a master revision number from the "version.info" file 
 * associated with the program or library. If a version number is found then
 * it is appended to the end of the directory name before it
 * is duplicated. In addition the most recent locked RCS revision of the 
 * source code is copied to the destination.  
 *                 
 */


struct dirstr {
  char *name;
  char *version;
  int type;
};

int dnum=0;
struct dirstr dlst[8000];
char *vfname;
char line[1024];
int vbflg=0;
char *postproc=NULL;

void log_info(char *text) {
  if (vbflg==1) fprintf(stderr,text);
  fprintf(stdout,text);
  fflush(stdout);
}


int do_command(char *command) {
  int status;
  char tmpname[256];
  time_t ltime;
  pid_t pid;
  
  pid=getpid();
  time(&ltime);  

  sprintf(tmpname,"log.%.d.%d",pid,ltime);
 
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


int dirtype(char *path) {  
  int s=0;
  int type=0;
  struct dirent *direntp=NULL;
  DIR *dirp; 

  dirp=opendir(path);
  if (dirp==NULL) return 0;
 
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
    if (strcmp(direntp->d_name,"RCS")==0) {
      type=1;
      break;
    }
    if (strcmp(direntp->d_name,"include")==0) s++;
    if (strcmp(direntp->d_name,"src")==0) s++;
    if (s==2) break;
  }
  if (s==2) type=2;
  closedir(dirp);
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
  char sys[1024];
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
    if (dlst[i].type !=2) {
      /* make a regular directory */

      if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/",root,dlst[i].name);
      else sprintf(fpath,"%s/%s.%s/",root,dlst[i].name,dlst[i].version); 
      if (make_branch(fpath) !=0) return -1;
    } else {

      /* make the src and include subdirectories of a library */

      if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/src/",root,dlst[i].name);
      else  sprintf(fpath,"%s/%s.%s/src/",root,dlst[i].name,dlst[i].version);
      make_branch(fpath);
      if (dlst[i].version==NULL) sprintf(fpath,"%s/%s/include/",
                                         root,dlst[i].name);
      else  sprintf(fpath,"%s/%s.%s/include/",
                    root,dlst[i].name,dlst[i].version);

      if (make_branch(fpath) !=0) return -1;
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
  char fpath[1024];
  char sys[1024];
  char tmp[16];
  for (i=0;i<dnum;i++) {
    if (dlst[i].type==0) continue; /* not a source directory */
    if (dlst[i].type==1) sprintf(fpath,"%s/%s/RCS",root,dlst[i].name);
    if (dlst[i].type==2) sprintf(fpath,"%s/%s/src/RCS",root,dlst[i].name);
    
    dirp=opendir(fpath);
    if (dirp==NULL) continue;
   
    while ((direntp=readdir(dirp)) !=NULL) {
      if (direntp->d_name[0]=='.') continue;
      if (strncmp(direntp->d_name,"version.info",12)==0) break;
    }
    closedir(dirp);
    if (direntp==NULL) continue;

    if (postproc==NULL)     
      sprintf(sys,"co -p %s/%s > %s",fpath,direntp->d_name,vfname);
    else 
      sprintf(sys,"co -p %s/%s | %s > %s",
              fpath,direntp->d_name,postproc,vfname);

    s=do_command(sys);
    if (s !=0) return -1;
    fp=fopen(vfname,"r");
    if (fp==NULL) return -1;

    vtxt=get_vtxt(fp);
    fclose(fp);
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



int copy_dir(char *src,char *dst,char *ptn) {
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
      if (postproc==NULL) 
        sprintf(sys,"cat %s > %s",spath,dpath);
      else
       sprintf(sys,"cat %s | %s > %s",spath,postproc,dpath);
      s=do_command(sys);
      if (s !=0) {
         closedir(dirp);
         return -1;
      }
    }
  }
  return 0; 
}


int copy_all(char *src,char *dst) {
  int i;
  char spath[1024];
  char dpath[1024];
  char tspath[1024];
  char tdpath[1024];
  char logtxt[1024];

  for (i=0;i<dnum;i++) {
    sprintf(spath,"%s/%s",src,dlst[i].name);
 
    if (dlst[i].version==NULL) sprintf(dpath,"%s/%s",dst,dlst[i].name);
    else sprintf(dpath,"%s/%s.%s",dst,dlst[i].name,dlst[i].version);

    log_info("Copying Directory:\n");
    sprintf(logtxt,"From:%s\n",spath);
    log_info(logtxt);
    sprintf(logtxt,"To:%s\n",dpath); 
    log_info(logtxt);

    if ((dlst[i].type==0) && (copy_dir(spath,dpath,NULL) !=0)) return -1;
    if (dlst[i].type==1) { /* copy source code */
       if (copy_dir(spath,dpath,"makefile") !=0) return -1; 
       if (copy_dir(spath,dpath,".h") !=0) return -1;
       if (co_dir(spath,dpath) !=0) return -1;
    } 
    if (dlst[i].type==2) { /* copy library source code */
      log_info("include:\n");
      sprintf(tspath,"%s/include",spath);
      sprintf(tdpath,"%s/include",dpath);
      copy_dir(tspath,tdpath,NULL);
      log_info("src:\n");
      sprintf(tspath,"%s/src",spath);
      sprintf(tdpath,"%s/src",dpath);
      if (copy_dir(tspath,tdpath,"makefile") !=0) return -1; 
      if (copy_dir(tspath,tdpath,".h") !=0) return -1;
      if (co_dir(tspath,tdpath) !=0) return -1;

    }    
    log_info("\n");
  }
  return 0;
}     
 

void makedst(char *src,char *dst,char *pkg) {
  char fpath[1024];
  char dpath[1024];
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
    if (type==0) dirfind(src,pkg);

    if (make_version(src) !=0) {
       log_info("Error generating version information.\n");
       exit(1);
    }
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
  char *pkg;
  char line[1024];
  char logtxt[256];

  vfname=tmpnam(NULL);

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
 

  if (argc<(i+3)) {
     print_info(stderr,errstr);
     exit(1);
  }

  log_info("makedst\n");

  if (postproc !=NULL) {
    sprintf(logtxt,"Post Processor:%s\n",postproc);
    log_info(logtxt);
  }
 

  src=argv[i];
  dst=argv[i+1];
  if (strcmp(argv[i+2],"stdin")==0) {
    /*read from stdin */
    while (fgets(line,1024,stdin) !=NULL) {
      for (i=0;(line[i] !=0) && (line[i]==' ');i++); 
      if (line[i] ==0) continue;
      memmove(line,line+i,strlen(line)+1-i);
      for (i=0;(line[i] !=0) && (line[i] !='\n') && (line[i] !=' ');i++); 
      line[i]=0; 
      if (strlen(line)==0) continue;
      makedst(src,dst,line);
    }
  } else {
    for (j=i+2;j<argc;j++) {
      makedst(src,dst,argv[j]);
    }
  }
  return 0;

 

}
  




