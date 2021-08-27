/* file_io.c
   ========= */


/* 
 $Log: file_io.c,v $
 Revision 1.2  1999/03/31 00:13:24  barnes
 Changed to full year filenames.

 Revision 1.1  1998/06/19 10:44:53  root
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#include "file_io.h"
#include "cnv_time.h"

char *open_file(char *pathenv,double etime,char code,
              char *ext,int mode,char *sfx,int flag) {

  char pathname[128]="/";
  char *fullname=NULL;
  char name[32];
  char *path;
  int year,month,day,hour,minute,file;
  double second;
  int openflag;
 
  fullname=malloc(256);

  if (mode==0) 
    mode=S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  epoch_time(etime,&year,&month,&day,&hour,&minute,&second);

  month = month % 100;
  day = day % 100;
  hour = hour % 100;


  openflag=O_WRONLY | O_CREAT;
  if (flag==0) openflag|=O_EXCL;
  else if (flag==1) openflag|=O_TRUNC;    

  do {
    sprintf(name,"%04d%02d%02d%02d%c",year,month,day,hour,code);
    if (*sfx !=0) strncat(name,sfx,1);
    if (pathenv !=NULL) strncpy(pathname,pathenv,80);
    path = strtok(pathname, ":");	
    file = -1;
    while ((file == -1) && (path != NULL)) {
	  _makepath(fullname, NULL, path, name,ext);

      /* open the file */
	  file = open(fullname,openflag,mode);
	  path = strtok(NULL, ":");	/* get the next path element */
    }
    if (file==-1) {
      if (*sfx==0) *sfx='A';
      else if (*sfx =='Z') *sfx='a';
      else (*sfx)=(*sfx)+1;
    }
  } while ((file ==-1) && (*sfx < 'z'));

  if (file != -1) {
    close(file);
    return fullname;
  }
  free(fullname);
  return NULL;
}
 
