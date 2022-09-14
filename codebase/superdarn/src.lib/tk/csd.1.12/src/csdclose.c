/* csdclose.c
   ==========
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
#include "csddata.h"
#include "csdread.h"




void CSDClose(struct CSDfp *fptr) {
  if (fptr !=NULL) {
    if (fptr->fp !=0) close(fptr->fp);
    if (fptr->fbuf !=NULL) free(fptr->fbuf);
    free(fptr);
  }
}
