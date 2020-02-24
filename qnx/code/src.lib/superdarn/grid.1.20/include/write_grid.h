/* write_grid.h
   ============
   Author: R.J.Barnes
*/

#ifndef _GRID_WRITE_GRID_H
#define _GRID_WRITE_GRID_H
#ifndef _GRIDDDAT_H
#include "griddata.h"
#endif

int encode_grid_one(FILE *fp,struct griddata *ptr);
int encode_grid_two(FILE *fp,struct griddata *ptr);

int write_grid(FILE *fp,struct griddata *ptr,int flag);

#endif



