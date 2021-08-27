/* cmp_close.c
   ========== */

/*
 $Log: cmp_close.c,v $
 Revision 1.1  1999/03/16 19:24:26  barnes
 Initial revision

 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "limit.h"
#include "cmpdata.h"
#include "cmp_read.h"
#include "cnv_data.h"
#include "cnv_time.h"

void cmp_close(struct cmpfp *fptr) {
  if (fptr !=NULL) {
    if (fptr->fp !=0) close(fptr->fp);
    if (fptr->fbuf !=NULL) free(fptr->fbuf);
    free(fptr);
  }
}






