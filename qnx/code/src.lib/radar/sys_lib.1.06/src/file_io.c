/* file_io.c
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
 $Log: file_io.c,v $
 Revision 1.4  2001/06/27 20:54:11  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:50  barnes
 Added Author Name

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
 
