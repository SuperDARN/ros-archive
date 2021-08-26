/* sort_grid.c
   =========== */


/* 
 $Log: sort_grid.c,v $
 Revision 1.1  2000/06/15 18:18:09  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cnv_time.h"
#include "file_lib.h"
#include "griddata.h"

int gsort(const void *a,const void *b) {
  struct gvec *ga,*gb;
  ga=(struct gvec *) a;
  gb=(struct gvec *) b;
  if (ga->st_id < gb->st_id) return -1;
  if (ga->st_id > gb->st_id) return 1;
  if (ga->index < gb->index) return -1;
  if (ga->index > gb->index) return 1;
  return 0;
}

void sort_grid(struct griddata *ptr) {
            
  /* sort the stations into order */

   qsort(ptr->data,ptr->npnt[1],sizeof(struct gvec),gsort);
}
