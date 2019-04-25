/* embed.c
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
#include "rxml.h"
#include "rfbuffer.h"
#include "block.h"
#include "rplotin.h"

/*
 $Log: embed.c,v $
 Revision 1.2  2004/06/16 16:28:38  barnes
 Added missing header.

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/

int RplotInEmbedStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInEmbed *ptr;
  struct RplotInPlot *plt;


  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInEmbed *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (strcmp(name,blk->name)==0) {
    int i;
    ptr->matrix=NULL;
    ptr->x=0;
    ptr->y=0;
    for (i=0;i<atnum;i++) {
      if (strcmp(atname[i],"matrix")==0)
        ptr->matrix=RplotInMatrixString(atval[i]);
      else if (strcmp(atname[i],"position")==0)
        RplotInPointString(atval[i],&ptr->x,&ptr->y);
    }
    return 0;
  }

  if (strcmp(name,"rplot")==0) {
    struct RplotInPlot *plt;
    struct RplotInPlot *root;
  
    root=ptr->plt;

    plt=malloc(sizeof(struct RplotInPlot));
    memcpy(plt,root,sizeof(struct RplotInPlot));
    plt->matrix=ptr->matrix;
    plt->x=ptr->x;
    plt->y=ptr->y;
     
    if (root->matrix !=NULL) {
      RplotInMatrixTransform(root->matrix,plt->x,plt->y,&plt->x,&plt->y);
      if (plt->matrix !=NULL) RplotInMatrixMultiply(root->matrix,
                                                    plt->matrix,plt->matrix);
      else plt->matrix=root->matrix;
    }
    plt->x+=root->x;
    plt->y+=root->y;

    plt->clp.num=0;
    plt->clp.px=NULL;
    plt->clp.py=NULL;
    plt->clp.t=NULL;

    blk=RplotInSave(blk->xml,name,plt);
    XMLSetStart(blk->xml,RplotInPlotStart,blk);
    XMLSetEnd(blk->xml,RplotInPlotEnd,blk);
    XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
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

int RplotInEmbedEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  struct RplotInBlock *blk;
  struct RplotInEmbed *ptr;
  struct RplotInPlot *plt;
 
  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInEmbed *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (plt==NULL) return -1;
  if (strcmp(name,blk->name)==0) {
    
    if (plt->clip.user !=NULL) s=(plt->clip.user)(plt->clp.num,plt->clp.px,
                                                     plt->clp.py,plt->clp.t,
                                                     plt->clip.data);
    
    if (ptr->matrix !=NULL) free(ptr->matrix);
    free(ptr);
    RplotInRestore(blk);
    return s;
  }
  return 0;
}

