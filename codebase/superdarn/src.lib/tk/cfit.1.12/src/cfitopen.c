/* cfitopen.c
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"





struct CFitfp *CFitOpen(char *name) {
  struct stat cstat;
  struct CFitfp *fptr=NULL;

  fptr=malloc(sizeof(struct CFitfp));
  fptr->fp=open(name,O_RDONLY);
  if (fptr->fp==-1) {
    free(fptr);
    return NULL;
  }
 
  fstat(fptr->fp,&cstat);
  fptr->fsze=cstat.st_size;
  
  fptr->ctime=-1;
  fptr->blen=0;
  fptr->fptr=0;
  fptr->fbuf=NULL;

  fptr->fbuf=malloc(fptr->fsze);
  
  if (fptr->fbuf !=NULL) {
    if (read(fptr->fp,fptr->fbuf,fptr->fsze) !=fptr->fsze) {
      close(fptr->fp);  
      free(fptr->fbuf);
      free(fptr);
      return NULL;
    }
    close(fptr->fp);
    fptr->fp=-1;
  }

  return fptr;
} 



