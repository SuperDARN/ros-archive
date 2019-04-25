/* line.c
   ====== 
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
#include "rxml.h"
#include "rfbuffer.h"
#include "block.h"
#include "rplotin.h"

/*
 $Log: line.c,v $
 Revision 1.2  2004/04/26 20:11:23  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision
 
*/

int RplotInLineStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInLine *ptr;
  struct RplotInPlot *plt;


  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInLine *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    char tmp[64];
    unsigned int mask;
    int i,s=0;
    int dflg=0;
    ptr->state=0;
    ptr->color=0;
    ptr->mask=0;
  
    for (i=0;i<atnum;i++) {
      if (strcmp(atname[i],"color")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->color);
      } else if (strcmp(atname[i],"mask")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&mask);
        ptr->mask=mask;
      } else if (strcmp(atname[i],"width")==0) ptr->width=atof(atval[i]);
      else if (strcmp(atname[i],"dash")==0) {
        dflg=1;
        if (plt->dash.user !=NULL) s=(plt->dash.user)(atval[i],plt->dash.data);
        if (s !=0) return -1;
      }
    }
    if ((dflg==0) && (plt->dash.user !=NULL)) 
       s=(plt->dash.user)(NULL,plt->dash.data);
    return s;
  }

  if (strcmp(name,"s")==0) {
    struct RplotInPoint *pnt;
    pnt=malloc(sizeof(struct RplotInPoint));
    pnt->inc=&plt->inc;
    pnt->x=&ptr->x1;
    pnt->y=&ptr->y1;
    ptr->state=ptr->state | 0x01;
    blk=RplotInSave(blk->xml,"p",pnt);
    XMLSetStart(blk->xml,RplotInPointStart,blk);
    XMLSetEnd(blk->xml,RplotInPointEnd,blk);
    return 0;
  }

  if (strcmp(name,"e")==0) {
    struct RplotInPoint *pnt;
    pnt=malloc(sizeof(struct RplotInPoint));
    pnt->inc=&plt->inc;
    pnt->x=&ptr->x2;
    pnt->y=&ptr->y2;
    ptr->state=ptr->state | 0x02;
    blk=RplotInSave(blk->xml,"p",pnt);
    XMLSetStart(blk->xml,RplotInPointStart,blk);
    XMLSetEnd(blk->xml,RplotInPointEnd,blk);
    return 0;
  }

  if (strcmp(name,"include")==0) {
    struct RplotInInclude *inc;
    inc=malloc(sizeof(struct RplotInInclude));
    inc->inc=&plt->inc;
    blk=RplotInSave(blk->xml,name,inc);
    XMLSetStart(blk->xml,RplotInIncludeStart,blk);
    XMLSetEnd(blk->xml,RplotInIncludeEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ignore")==0) {
    blk=RplotInSave(blk->xml,name,NULL);
    XMLSetStart(blk->xml,RplotInIgnoreStart,blk);
    XMLSetText(blk->xml,RplotInIgnoreText,blk);
    XMLSetEnd(blk->xml,RplotInIgnoreEnd,blk);
    return 0;
  }

  return 0;
}

int RplotInLineEnd(char *name,char *buf,int sze,void *data) {
 
  struct RplotInBlock *blk;
  struct RplotInLine *ptr;
  struct RplotInPlot *plt;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInLine *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
 
  if (strcmp(name,blk->name)==0) {
    int s=0;  
    if (ptr->state !=0x03) return -1;
    if (plt->matrix !=NULL) {
      RplotInMatrixTransform(plt->matrix,ptr->x1,ptr->y1,&ptr->x1,&ptr->y1);
      RplotInMatrixTransform(plt->matrix,ptr->x2,ptr->y2,&ptr->x2,&ptr->y2);
    }
    ptr->x1+=plt->x;
    ptr->x2+=plt->x;
    ptr->y1+=plt->y;
    ptr->y2+=plt->y;

    if (plt->line.user !=NULL) s=(plt->line.user)(ptr->x1,ptr->y1,
                    ptr->x2,ptr->y2,ptr->color,ptr->mask,
                    ptr->width,plt->line.data);
    free(ptr);
    RplotInRestore(blk);
    return s;
  }
  return 0;
}

