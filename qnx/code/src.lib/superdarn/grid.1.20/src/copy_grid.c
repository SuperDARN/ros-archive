/* copy_grid.c
   ===========
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
 $Log: copy_grid.c,v $
 Revision 1.3  2001/06/27 20:49:26  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  2000/06/15 18:18:09  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cnv_time.h"
#include "file_lib.h"
#include "griddata.h"

void copy_grid(struct griddata *a,struct griddata *b) {

  int i;
  a->st_time=b->st_time;
  a->ed_time=b->ed_time;

  a->nprm[0]=b->nprm[0];
  a->nprm[1]=b->nprm[1];
  a->npnt[0]=b->npnt[0];
  a->npnt[1]=b->npnt[1];

  for (i=0;i<2;i++) {
     if (a->name[i]==NULL) a->name[i]=malloc(strlen(b->name[i])+1);
     else a->name[i]=realloc(a->name[i],strlen(b->name[i])+1);
     if (a->type[i]==NULL) a->type[i]=malloc(strlen(b->type[i])+1);
     else a->type[i]=realloc(a->type[i],strlen(b->type[i])+1);
     if (a->unit[i]==NULL) a->unit[i]=malloc(strlen(b->unit[i])+1);
     else a->unit[i]=realloc(a->unit[i],strlen(b->unit[i])+1);
     strcpy(a->name[i],b->name[i]);
     strcpy(a->type[i],b->type[i]);
     strcpy(a->unit[i],b->unit[i]);
  }
   

  if (b->npnt[0]>0) {
    if (a->sdata==NULL) a->sdata=malloc(sizeof(struct svec)*b->npnt[0]);
    else a->sdata=realloc(a->sdata,sizeof(struct svec)*b->npnt[0]);
  } else if (a->sdata !=NULL) {
    free(a->sdata);
    a->sdata=NULL;
  }

  if (b->npnt[1]>0) {
  if (a->data==NULL) a->data=malloc(sizeof(struct gvec)*b->npnt[1]);
  else a->data=realloc(a->data,sizeof(struct gvec)*b->npnt[1]);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->npnt[0] !=0) memcpy(a->sdata,b->sdata,sizeof(struct svec)*b->npnt[0]);
  if (b->npnt[1] !=0) memcpy(a->data,b->data,sizeof(struct gvec)*b->npnt[1]);
}
