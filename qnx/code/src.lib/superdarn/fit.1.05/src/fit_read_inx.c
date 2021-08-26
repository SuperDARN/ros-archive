/* fit_read_inx.c
   ============== */

/* 
 $Log: fit_read_inx.c,v $
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

int fit_read_inx(struct fitfp *ptr,int32 *buffer,int recno) {
  int i,status;;
  if (ptr->inxfp !=-1) {
     if (recno !=-1)  /* jump to this record */
       lseek(ptr->inxfp,(recno+1)*ptr->inx_recl,SEEK_SET);
     for (i=0;(i<4) && (status==0);i++) 
     status=read_long(ptr->inxfp,&buffer[i]);
     return status;
  } else if (ptr->ibuf !=NULL) {
     if (recno !=-1)  ptr->iptr=(recno+1)*ptr->inx_recl;
     if ((ptr->istat.st_size-ptr->iptr)<(4*sizeof(int32))) return -1; 
     for (i=0;i<4;i++) 
        cnv_long(ptr->ibuf+ptr->iptr+sizeof(int32)*i,&buffer[i]);
     ptr->iptr+=sizeof(int32)*4;
     return 0;
  }
  return -1;
}


