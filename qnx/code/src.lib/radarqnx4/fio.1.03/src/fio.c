/* fio.c
   =====
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
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>



/*
 $Log: fio.c,v $
 Revision 1.3  2004/06/04 18:19:31  barnes
 Changed the filename format.

 Revision 1.2  2004/05/03 17:56:29  barnes
 Replaced redundant macro.

 Revision 1.1  2003/10/31 19:04:56  barnes
 Initial revision

*/


char *FIOMakeFile(char *pathenv,int yr,int mo,int dy,int hr,int mt,int sc,
		  char *code,char *ext,int mode,int flag) {
 
  char pathname[128]="/";
  char *fullname=NULL;
  char name[32];
  char *path;
  int file;
  int openflag;
 
  fullname=malloc(256);
  if (fullname==NULL) return NULL;

  if (mode==0) 
    mode=S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  openflag=O_WRONLY | O_CREAT;
  if (flag==0) openflag|=O_EXCL;
  else if (flag==1) openflag|=O_TRUNC;    

 
  sprintf(name,"%04d%02d%02d.%02d%02d.%02d.%s",yr,mo,dy,hr,mt,sc,code);
  if (pathenv !=NULL) strncpy(pathname,pathenv,80);
  path = strtok(pathname, ":");	
  file = -1;
  while ((file == -1) && (path != NULL)) {
    sprintf(fullname,"%s/%s.%s",path,name,ext);      
    file = open(fullname,openflag,mode);
    path = strtok(NULL, ":");    
  }
   
  if (file != -1) {
    close(file);
    return fullname;
  }

  free(fullname);
  return NULL;
}
