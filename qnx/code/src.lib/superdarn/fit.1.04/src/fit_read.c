/* fit_read.c
   ========== */

/* 
 $Log: fit_read.c,v $
 Revision 1.2  1999/03/16 15:00:46  barnes
 Added check for null pointer.

 Revision 1.1  1999/03/16 14:36:30  barnes
 Initial revision

 */


/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "cnv_data.h"
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "fit_str.h"

int fit_read(struct fitfp *ptr,struct fitdata *fit_data) {
  if (ptr==NULL) return -1;
  return (ptr->fitread)(ptr,fit_data);
}







