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
#include <math.h>
#include "rfbuffer.h"

/*
 $Log: image.c,v $
 Revision 1.3  2005/03/29 22:59:38  barnes
 Fixed bug in rendering 8-bit images.

 Revision 1.2  2004/04/26 19:48:22  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/13 20:19:56  barnes
 Initial revision

 */

int FrameBufferImage(struct FrameBuffer *ptr,
			 struct FrameBufferMatrix *matrix,
                         struct FrameBuffer *img,
		         unsigned char m,
                         int x,int y,int sflg,
                         struct FrameBufferClip *clip) {
  unsigned char r,g,b,a;
  unsigned char rv,gv,bv,av;
  float alpha;

  int s=0;
  int iw,ih,isze;
  int i;
  float ma=1,mb=0,mc=0,md=1;
  float determ;
  float ia,ib,ic,id;
  int tr;

  int btm=0,lft=0,rgt=0,top=0;
  int ix[4]={0,0,0,0},iy[4]={0,0,0,0};
  int tix=0,tiy=0;
  int tx,ty;
  int sze;
  int flg;
 
  if (ptr==NULL) return -1;
  if (img==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;
  if (ptr->img==0) return -1;
 
  if (img->wdt==0) return -1;
  if (img->hgt==0) return -1;
  if (img->img==0) return -1;

  sze=ptr->wdt*ptr->hgt;

  if (matrix!=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  /* find the inverse matrix to map from render space to
   * the image space. */

  determ=1/(ma*md-mb*mc);
  ia=md*determ;
  ib=-mb*determ;
  ic=-mc*determ;
  id=ma*determ;

  /* work out the limits of the plot in image space */

  ix[1]=img->wdt;
  ix[2]=img->wdt;
  iy[2]=img->hgt;
  iy[3]=img->hgt;

  lft=ptr->wdt;
  btm=ptr->hgt;

  for (tr=0;tr<4;tr++) {
    tx=x+ix[tr]*ma+iy[tr]*mb;
    ty=y+ix[tr]*mc+iy[tr]*md;
    if (tx<lft) lft=tx;
    if (ty<btm) btm=ty;
    if (tx>rgt) rgt=tx;
    if (ty>top) top=ty;
  } 

  if (lft<0) lft=0;
  if (btm<0) btm=0;
  if (rgt>ptr->wdt) rgt=ptr->wdt;
  if (top>ptr->hgt) top=ptr->hgt;

  iw=img->wdt;
  ih=img->hgt;
  isze=iw*ih;
 

  ty=btm;
  while (ty !=top) {
    tx=lft;
    while (tx !=rgt) {
      /* find image space coordinates */
      flg=1;
      
   
      ix[0]=(tx-x)*ia+(ty-y)*ib;
      iy[0]=(tx-x)*ic+(ty-y)*id;
          
      if (ix[0]<0) flg=0;
      if (iy[0]<0) flg=0;
      if (ix[0]>=img->wdt) flg=0;
      if (iy[0]>=img->hgt) flg=0;
 
      if (flg) {
        if (sflg) {
          int cr=0,cg=0,cb=0,ca=0,c=0;
          int ilft=0,ibtm=0,irgt=0,itop=0;
          ix[1]=(tx+1-x)*ia+(ty-y)*ib;
          iy[1]=iy[0];
          ix[2]=ix[1];
          iy[2]=(tx-x)*ic+(ty+1-y)*id;
          ix[3]=ix[0];
          iy[3]=iy[2];
          ilft=img->wdt;
          ibtm=img->hgt;
          for (i=0;i<4;i++) {
            if (ix[i]<0) ix[i]=0;
            if (iy[i]<0) iy[i]=0;
            if (ix[i]>img->wdt) ix[i]=img->wdt;
            if (iy[i]>img->hgt) iy[i]=img->hgt;
            if (ix[i]<ilft) ilft=ix[i];
            if (iy[i]<ibtm) ibtm=iy[i];
            if (ix[i]>irgt) irgt=ix[i];
            if (iy[i]>itop) itop=iy[i];
	  }
          if ((ilft==irgt) || (itop==ibtm)) {
	    if (ilft==irgt) {
	      tix=ilft;
              tiy=(ibtm+itop)/2;

	    }
            if (ibtm==itop) {
	      tix=(ilft+irgt)/2;
              tiy=ibtm;
	    }
            cr=img->img[tiy*iw+tix];
            ca=img->msk[tiy*iw+tix];
            if (img->depth !=8) {
              cg=img->img[tiy*iw+tix+isze];
              cb=img->img[tiy*iw+tix+2*isze];
            }
            c=1;
	  } else {
             for (tix=ilft;tix<irgt;tix++) {
              for (tiy=ibtm;tiy<itop;tiy++) {
                cr+=img->img[tiy*iw+tix];
                ca+=img->msk[tiy*iw+tix];
                if (img->depth !=8) {
                  cg+=img->img[tiy*iw+tix+isze];
                  cb+=img->img[tiy*iw+tix+2*isze];
                }
                c++;
	      }
	    }
	  }
          if (img->depth==8) {
            cg=cr;
            cb=cr;
	  }

          r=cr/c;
          g=cg/c;
          b=cb/c;
          a=ca/c;

          rv=ptr->img[ty*ptr->wdt+tx];
          av=ptr->msk[ty*ptr->wdt+tx];
          if (ptr->depth !=8) {
            gv=ptr->img[ty*ptr->wdt+tx+sze];
            bv=ptr->img[ty*ptr->wdt+tx+2*sze];
	  } else {
            gv=rv;
            bv=rv;
	  }
          alpha=(float) a/255.0;

          r=alpha*r+(1-alpha)*rv;
          g=alpha*g+(1-alpha)*gv;
          b=alpha*b+(1-alpha)*bv;

          s=FrameBufferRawPixel(ptr,tx,ty,sze,r,g,b,av,m,clip);
          if (s !=0) break;
	} else {
          tix=ix[0];
          tiy=iy[0];       
 
          r=img->img[tiy*iw+tix];
          if (img->depth !=8) {
            g=img->img[tiy*iw+tix+isze];
            b=img->img[tiy*iw+tix+2*isze];
          } else {
            g=r;
            b=r;
          }
          a=img->msk[tiy*iw+tix]; 


          rv=ptr->img[ty*ptr->wdt+tx];
          av=ptr->msk[ty*ptr->wdt+tx];
          if (ptr->depth !=8) {
            gv=ptr->img[ty*ptr->wdt+tx+sze];
            bv=ptr->img[ty*ptr->wdt+tx+2*sze];
	  } else {
            gv=rv;
            bv=rv;
	  }
          alpha=a/255.0;
        

          r=alpha*r+(1-alpha)*rv;
          g=alpha*g+(1-alpha)*gv;
          b=alpha*b+(1-alpha)*bv;

          s=FrameBufferRawPixel(ptr,tx,ty,sze,r,g,b,av,m,clip);
          if (s !=0) break;
	}
      }
  
      tx++;
    }
    if (s !=0) break;
    ty++;
  }
 
  return s;
}








