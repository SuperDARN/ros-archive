/* add_grid.c
   ==========
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
 $Log: add_grid.c,v $
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

void add_grid(struct griddata *a,struct griddata *b,int recnum) {
  int i;
  int j;

  if (b==NULL)  { /* tidy up */
    if (recnum>1) {
      for (i=0;i<a->npnt[0];i++) {
        a->sdata[i].freq0=a->sdata[i].freq0/recnum;
        a->sdata[i].noise.mean=a->sdata[i].noise.mean/recnum;
        a->sdata[i].noise.sd=a->sdata[i].noise.sd/recnum;
      }
    }
    return;
  }
 
  if (b->npnt[0] !=0) {
    if (a->npnt[0]==0) {
      a->npnt[0]=b->npnt[0];
      if (a->sdata==NULL) a->sdata=malloc(sizeof(struct svec)*b->npnt[0]);
      else a->sdata=realloc(a->sdata,sizeof(struct svec)*b->npnt[0]);
      memcpy(a->sdata,b->sdata,sizeof(struct svec)*b->npnt[0]);
    } else {
      int st_id;
      /* merge the header information */
      for (i=0;i<b->npnt[0];i++) {
         st_id=b->sdata[i].st_id;
         for (j=0;(j<a->npnt[0]) && (a->sdata[j].st_id !=st_id);j++);
         if (j<a->npnt[0]) { /* merge record */
            a->sdata[j].npnt+=b->sdata[i].npnt;
            a->sdata[j].freq0+=b->sdata[i].freq0;
            if (b->sdata[i].gsct !=0) a->sdata[i].gsct=b->sdata[i].gsct !=0;
            a->sdata[j].noise.mean+=b->sdata[i].noise.mean;
            a->sdata[j].noise.sd+=b->sdata[i].noise.sd;
            if (b->sdata[i].vel.min<a->sdata[j].vel.min)
                a->sdata[j].vel.min=b->sdata[i].vel.min;
            if (b->sdata[i].pwr.min<a->sdata[j].pwr.min)
                a->sdata[j].pwr.min=b->sdata[i].pwr.min;
            if (b->sdata[i].wdt.min<a->sdata[j].wdt.min)
                a->sdata[j].wdt.min=b->sdata[i].wdt.min;
            if (b->sdata[i].vel.max>a->sdata[j].vel.max)
                a->sdata[j].vel.max=b->sdata[i].vel.max;
            if (b->sdata[i].pwr.max>a->sdata[j].pwr.max)
                a->sdata[j].pwr.max=b->sdata[i].pwr.max;
            if (b->sdata[i].wdt.max>a->sdata[j].wdt.max)
                a->sdata[j].wdt.max=b->sdata[i].wdt.max;
	 } else {
           a->sdata=realloc(a->sdata,sizeof(struct svec)*(j+1));
           memcpy(&a->sdata[j],&b->sdata[i],sizeof(struct svec));
           a->npnt[0]++;
	 }
      }
    }
  }

  i=a->npnt[1]; 
  j=a->npnt[1]+b->npnt[1];

  if (b->npnt[1] !=0) {  
    if (a->data==NULL) a->data=malloc(sizeof(struct gvec)*j);
    else a->data=realloc(a->data,sizeof(struct gvec)*j);
    memcpy(&a->data[i],b->data,
          sizeof(struct gvec)*b->npnt[1]);
    a->npnt[1]=j;
  }

}
