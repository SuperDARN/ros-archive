/* cfitread.c
   =========
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"

/*
 $Log: cfitread.c,v $
 Revision 1.5  2004/06/22 17:36:33  barnes
 Changed references of Long to Int.

 Revision 1.4  2004/05/04 18:18:23  barnes
  Renamed the type header to avoid confusion.

 Revision 1.3  2004/04/26 21:37:01  barnes
 Audit - Enforced warning set to "all".

 Revision 1.2  2003/09/13 22:36:31  barnes
 Modifcation to use the new data structures.

 Revision 1.1  2003/04/09 23:41:09  barnes
 Initial revision

*/

int CFitRead(struct CFitfp *fptr,struct CFitdata *ptr) {
  float val;
  int i;
  
  if (fptr->fp !=-1) {
    if (ConvertReadDouble(fptr->fp,&ptr->time) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->stid) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->scan) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->cp) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->bmnum) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->channel) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->intt.sc) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->intt.us) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->frang) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->rsep) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->rxrise) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->tfreq) !=0) return -1;
    if (ConvertReadInt(fptr->fp,&ptr->noise) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->atten) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->nave) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->nrang) !=0) return -1;
    
  } else {
    if ((fptr->fptr+sizeof(double)+7*sizeof(int16)+
         sizeof(int32))>fptr->fsze) return -1;
    ConvertToDouble(&fptr->fbuf[fptr->fptr],&ptr->time);
    fptr->fptr+=sizeof(double);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->stid);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->scan);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->cp);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->bmnum);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->channel);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->intt.sc);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->intt.us);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->frang);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->rsep);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->rxrise);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->tfreq);
    fptr->fptr+=sizeof(int16);
    ConvertToInt(fptr->fbuf+fptr->fptr,&ptr->noise);
    fptr->fptr+=sizeof(int32);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->atten);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->nave);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->nrang);
    fptr->fptr+=sizeof(int16);
  }
  fptr->ctime=ptr->time;

  if (fptr->fp !=-1) {
    if (read(fptr->fp,&ptr->num,1) !=1) return -1;
    if (read(fptr->fp,ptr->rng,ptr->num) !=ptr->num) return -1;
  } else {
    if (fptr->fptr>=fptr->fsze) return -1;
    ptr->num=fptr->fbuf[fptr->fptr];
    fptr->fptr+=1;
    if ((fptr->fptr+ptr->num)>=fptr->fsze) return -1;
    memcpy(ptr->rng,&fptr->fbuf[fptr->fptr],ptr->num);
    fptr->fptr+=ptr->num;
  }

  for (i=0;i<ptr->num;i++) {
    if (fptr->fp !=-1) {
      if (read(fptr->fp,&ptr->data[i].gsct,1) !=1) return -1;
      if (ptr->data[i].gsct==EOF) return -1; 
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].p_0=val;  
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].p_0_e=val;      
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].v=val;
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].p_l=val;
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].w_l=val;
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].v_e=val;
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].p_l_e=val;
      if (ConvertReadFloat(fptr->fp,&val) !=0) return -1;
      ptr->data[i].w_l_e=val;
    } else {
      if ((fptr->fptr+1+sizeof(float)*6)>=fptr->fsze) return -1;
      ptr->data[i].gsct=fptr->fbuf[fptr->fptr];
      fptr->fptr+=1;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].p_0=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].p_0_e=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].v=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].p_l=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].w_l=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].v_e=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].p_l_e=val;
      ConvertToFloat(fptr->fbuf+fptr->fptr,&val);
      fptr->fptr+=sizeof(float);
      ptr->data[i].w_l_e=val;
    }
  }
  
  return 0;
}









