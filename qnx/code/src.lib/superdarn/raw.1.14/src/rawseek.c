/* rawseek.c
   =========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "dmap.h"
#include "rawindex.h"

/*
 $Log: rawseek.c,v $
 Revision 1.6  2007/03/15 21:13:08  code
 Fixed typo.

 Revision 1.5  2007/03/09 14:28:23  code
 Corrected seek.

 Revision 1.4  2004/06/16 21:49:14  barnes
 Added missing header.

 Revision 1.3  2004/05/04 18:37:48  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:30:56  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/16 16:14:13  barnes
 Initial revision

*/


double RawGetTime(struct DataMap *ptr) {
  struct DataMapScalar *s;
  int c;
  int yr=0,mo=0,dy=0,hr=0,mt=0,sc=0,us=0;  
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    if ((strcmp(s->name,"time.yr")==0) && (s->type==DATASHORT)) 
      yr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mo")==0) && (s->type==DATASHORT))
      mo=*(s->data.sptr);
    if ((strcmp(s->name,"time.dy")==0) && (s->type==DATASHORT))
      dy=*(s->data.sptr);
    if ((strcmp(s->name,"time.hr")==0) && (s->type==DATASHORT))
      hr=*(s->data.sptr);
    if ((strcmp(s->name,"time.mt")==0) && (s->type==DATASHORT))
      mt=*(s->data.sptr);
    if ((strcmp(s->name,"time.sc")==0) && (s->type==DATASHORT))
      sc=*(s->data.sptr);
    if ((strcmp(s->name,"time.us")==0) && (s->type==DATASHORT))
      us=*(s->data.sptr);
   }
   if (yr==0) return -1;
   return TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+us/1.0e6); 
}

int RawSeek(int fid,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
            struct RawIndex *inx) {

  int fptr=0;
  struct DataMap *ptr;
  double tfile=0,tval;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (inx !=NULL) {
    int rec=0,prec=-1;
    int srec;
    int erec;

    /* search using index records */

    double stime,etime;
    stime=inx->tme[0];
    etime=inx->tme[inx->num-1];

  
    srec=0;
    erec=inx->num;
    if (tval<stime) { /* before start of file */
      if (atme !=NULL) *atme=stime;
      lseek(fid,inx->inx[srec],SEEK_SET);
      return 0;
    } else if (tval>etime) { /* after end of file */
      if (atme !=NULL) *atme=stime;
      lseek(fid,inx->inx[erec-1],SEEK_SET);
      return -1;
    }
    do {
      prec=rec;
      rec=srec+(int) ((tval-stime)*(erec-srec))/(etime-stime);
      if (inx->tme[rec]==tval) break;
      if (inx->tme[rec]<tval) {
         srec=rec;
         stime=inx->tme[rec];
      } else {
         erec=rec;
         etime=inx->tme[rec];
      }
    } while (prec != rec);
    if (atme !=NULL) *atme=inx->tme[rec]; 
    lseek(fid,inx->inx[rec],SEEK_SET);
    return 0;
  } else {
    fptr=lseek(fid,0,SEEK_CUR);
    ptr=DataMapRead(fid);
    if (ptr !=NULL) {
      tfile=RawGetTime(ptr);
      DataMapFree(ptr);
      if (tfile>tval) fptr=lseek(fid,0,SEEK_SET);
    } else fptr=lseek(fid,0,SEEK_SET);
    if (atme!=NULL) *atme=tfile;
    while (tval>tfile) {
      ptr=DataMapRead(fid);
      if (ptr==NULL) break;
      tfile=RawGetTime(ptr);
      DataMapFree(ptr);
      if (tval>tfile) fptr=lseek(fid,0,SEEK_CUR);
      if (atme !=NULL) *atme=tfile;
    } 
    if (tval>tfile) return -1;
    lseek(fid,fptr,SEEK_SET);
  }
  return 0;
}

int RawFseek(FILE *fp,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
            struct RawIndex *inx) {
  return RawSeek(fileno(fp),yr,mo,dy,hr,mt,sc,atme,inx);
}
