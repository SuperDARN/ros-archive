/* fit_close.c
   =========== */

/* 
 $Log: fit_close.c,v $
 Revision 1.1  1999/03/16 14:47:32  barnes
 Initial revision

 */


/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cnv_data.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "fit_str.h"

void fit_close(struct fitfp *ptr) {
  if (ptr==NULL) return;
  if (ptr->fitfp !=-1) close(ptr->fitfp);
  if (ptr->inxfp !=-1) close(ptr->inxfp);
  if (ptr->fbuf !=NULL) free(ptr->fbuf);
  if (ptr->ibuf !=NULL) free(ptr->ibuf);
  free(ptr);
}



