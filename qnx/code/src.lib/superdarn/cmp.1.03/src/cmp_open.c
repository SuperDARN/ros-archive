/* cmp_open.c
   ========== */

/*
 $Log: cmp_open.c,v $
 Revision 1.4  1999/07/25 00:42:19  barnes
 Initialize the major and minor revision numbers.

 Revision 1.3  1999/07/23 19:28:23  barnes
 Modified the memory allocation system so that it is much more efficient.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:24:26  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "limit.h"
#include "cmpdata.h"
#include "cmp_read.h"
#include "cnv_data.h"
#include "cnv_time.h"

struct cmpfp *cmp_open(char *name) {
  int sze;
  int size,magic;
  struct stat cstat;
  struct cmpfp *fptr=NULL;
  fptr=malloc(sizeof(struct cmpfp));
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
  
  fptr->hdr.major_rev=0;
  fptr->hdr.minor_rev=0;
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



