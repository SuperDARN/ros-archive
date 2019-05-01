/* maketree.c
   ==========
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>

/*
 $Log: maketree.c,v $
 Revision 1.1  2004/05/11 20:34:28  barnes
 Initial revision

*/

int maketree(char *path,char *patn) {
  struct dirent *direntp=NULL;
  DIR *dirp;
  int s; 
  int flg=0;
  struct stat dstat;
  char spath[4096];

  fprintf(stdout,"%s\n",path);

  dirp=opendir(path);
  if (dirp==NULL) return 0;
  
  while ((direntp=readdir(dirp)) !=NULL) {
     if (direntp->d_name[0]=='.') continue;
     /* do pattern matching here */
     flg=(fnmatch(patn,direntp->d_name,0)==0);
     if (flg==1) break;
  }
  if (flg==1) {
    closedir(dirp);
    return 0;
  }

  rewinddir(dirp);
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
     sprintf(spath,"%s/%s",path,direntp->d_name);
     s=stat(spath,&dstat); 
    
     if ((s==0) && (S_ISDIR(dstat.st_mode))) {
       /* recurse here */
       maketree(spath,patn);
     }
  }
  closedir(dirp);
  return 0;
}

int main(int argc,char *argv[]) {
  maketree(argv[1],argv[2]);
  return 0;
}
 


  




