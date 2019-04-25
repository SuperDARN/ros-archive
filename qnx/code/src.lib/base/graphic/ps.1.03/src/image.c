/* image.c
   ======= 
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
#include "rfbuffer.h"
#include "rps.h"

/*
 $Log: image.c,v $
 Revision 1.2  2004/04/26 19:53:27  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/10/21 13:55:49  barnes
 Initial revision

*/


int PostScriptImage(struct PostScript *ptr,
	            struct PostScriptMatrix *matrix,
                    struct FrameBuffer *img,
                    unsigned char mask,
                    float x,float y,
                    struct PostScriptClip *clip) {
  int s=0;
  int d=1;

  float ma=1.0,mb=0.0,mc=0.0,md=1.0;

  int i=0,j=0;
  char buf[4096];

  if (ptr==NULL) return -1;
  if (img==NULL) return -1;
  if (img->wdt==0) return 0;
  if (img->hgt==0) return 0;

  if (img->depth !=8) d=3; 

  if (ptr->cnt >0) {
    sprintf(buf,"d\n");
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    ptr->cnt=0;
  }
  if (s !=0) return s;

  if (clip !=ptr->clip) {
    s=PostScriptClip(ptr,clip);
    if (s !=0) return s;
    ptr->clip=clip;
  }

  if (matrix !=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  sprintf(buf,"s [%g %g %g %g %g %g] concat 0 0 m ",
          ma,mb,mc,md,ptr->x+x,ptr->y+ptr->hgt-y); 

  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  sprintf(buf,"%d %d translate %d %d scale\n",0,-img->hgt,
	  img->wdt,img->hgt);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;


  sprintf(buf,"/DataString %d string def\n",img->wdt*d);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  sprintf(buf,"%d %d 8 [%d 0 0 %d 0 %d]\n",img->wdt,img->hgt,
          img->wdt,-img->hgt,img->hgt);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  sprintf(buf,"{\ncurrentfile DataString readhexstring pop\n} ");
 
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  if (s !=0) return s;

  if (d==1) {
    unsigned char *v;
    sprintf(buf,"bind image\n");
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

    if (s !=0) return s;
    v=img->img;
    for (i=0;i<img->wdt*img->hgt;i++) {
      if (mask & 0x01) sprintf(buf,"%.2x",v[i]);
      else sprintf(buf,"%.2x",0);
      if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);
      if (s !=0) return s;
      j++;
      if ((j % 32)==0) {
	strcpy(buf,"\n");
        if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);
         if (s !=0) return s;
      }
    }
  } else {
    unsigned char *r,*g,*b,*a;
    int rv=0,gv=0,bv=0;
    float av=0;
 
    sprintf(buf,"bind false 3 colorimage\n");
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

    if (s !=0) return s;
    r=img->img;
    g=img->img+img->wdt*img->hgt;
    b=img->img+2*img->wdt*img->hgt;
    a=img->msk;
    for (i=0;i<img->wdt*img->hgt;i++) {
      rv=255;
      gv=255;
      bv=255;
      av=a[i]/255.0;
      if (mask & 0x04) rv=(1-av)*rv+av*r[i];
      if (mask & 0x02) gv=(1-av)*gv+av*g[i];
      if (mask & 0x01) bv=(1-av)*bv+av*b[i];
      sprintf(buf,"%.2x%.2x%.2x",rv,gv,bv);
      if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);
      if (s !=0) return s;
      j++;
      if ((j % 16)==0) {
	strcpy(buf,"\n");
        if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);
         if (s !=0) return s;
      }
    }
  }
  sprintf(buf,"\nr\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  return s;

}

