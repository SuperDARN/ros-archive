/* plot.c
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
#include "fontdb.h"


/*
 $Log: plot.c,v $
 Revision 1.2  2004/06/16 16:28:38  barnes
 Added missing header.

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/


int RplotInPlotStart(char *name,int atnum,char **atname,char **atval,
		  char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInPlot *plt;
  char *wrap="<>";
  int s=0;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  plt=(struct RplotInPlot *) blk->data;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
     return 0;
  }

  if (strcmp(name,"info")==0) {
    blk=RplotInSave(plt->xml,name,plt);
    XMLSetStart(plt->xml,RplotInInfoStart,blk);
    XMLSetText(plt->xml,RplotInInfoText,blk);
    XMLSetEnd(plt->xml,RplotInInfoEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"clip")==0) {
    struct RplotInClip *clp;
    clp=malloc(sizeof(struct RplotInClip));
    clp->plt=plt;
    blk=RplotInSave(plt->xml,name,clp);
    XMLSetStart(plt->xml,RplotInClipStart,blk);
    XMLSetEnd(plt->xml,RplotInClipEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"line")==0) {
    struct RplotInLine *lne;
    lne=malloc(sizeof(struct RplotInLine));
    lne->plt=plt;
    blk=RplotInSave(plt->xml,name,lne);
    XMLSetStart(plt->xml,RplotInLineStart,blk);
    XMLSetEnd(plt->xml,RplotInLineEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"bezier")==0) {
    struct RplotInBezier *bez;
    bez=malloc(sizeof(struct RplotInBezier));
    bez->plt=plt;
    blk=RplotInSave(plt->xml,name,bez);
    XMLSetStart(plt->xml,RplotInBezierStart,blk);
    XMLSetEnd(plt->xml,RplotInBezierEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ellipse")==0) {
    struct RplotInShape *shp;
    shp=malloc(sizeof(struct RplotInShape));
    shp->plt=plt;
    blk=RplotInSave(plt->xml,name,shp);
    XMLSetStart(plt->xml,RplotInShapeStart,blk);
    XMLSetEnd(plt->xml,RplotInShapeEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"rectangle")==0) {
    struct RplotInShape *shp;
    shp=malloc(sizeof(struct RplotInShape));
    shp->plt=plt;
    blk=RplotInSave(plt->xml,name,shp);
    XMLSetStart(plt->xml,RplotInShapeStart,blk);
    XMLSetEnd(plt->xml,RplotInShapeEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"polygon")==0) {
    struct RplotInPolygon *pol;
    pol=malloc(sizeof(struct RplotInPolygon));
    pol->plt=plt;
    blk=RplotInSave(plt->xml,name,pol);
    XMLSetStart(plt->xml,RplotInPolygonStart,blk);
    XMLSetEnd(plt->xml,RplotInPolygonEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"text")==0) {
    struct RplotInText *txt;
    txt=malloc(sizeof(struct RplotInText));
    txt->plt=plt;
    blk=RplotInSave(plt->xml,name,txt);
    XMLSetStart(plt->xml,RplotInTextStart,blk);
    XMLSetEnd(plt->xml,RplotInTextEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"image")==0) {
    struct RplotInImage *img;
    img=malloc(sizeof(struct RplotInImage));
    img->plt=plt;
    blk=RplotInSave(plt->xml,name,img);
    XMLSetStart(plt->xml,RplotInImageStart,blk);
    XMLSetEnd(plt->xml,RplotInImageEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

 if (strcmp(name,"imagen")==0) {
    struct RplotInImageName *img;
    img=malloc(sizeof(struct RplotInImageName));
    img->plt=plt;
    blk=RplotInSave(plt->xml,name,img);
    XMLSetStart(plt->xml,RplotInImageNameStart,blk);
    XMLSetEnd(plt->xml,RplotInImageNameEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }


  if (strcmp(name,"embed")==0) {
    struct RplotInEmbed *emb;
    emb=malloc(sizeof(struct RplotInEmbed));
    emb->plt=plt;
    blk=RplotInSave(plt->xml,name,emb);
    XMLSetStart(plt->xml,RplotInEmbedStart,blk);
    XMLSetEnd(plt->xml,RplotInEmbedEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
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

  if (plt->pass.user !=NULL) {
    s=(plt->pass.user)(wrap,1,plt->pass.data);
    if (s==0) s=(plt->pass.user)(buf,sze,plt->pass.data);
    if (s==0) s=(plt->pass.user)(wrap+1,1,plt->pass.data);
  }

  return 0;
}

int RplotInPlotEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  char *wrap="<>";
  struct RplotInBlock *blk;
  struct RplotInPlot *ptr;
  struct XMLdata *xml;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInPlot *) blk->data;
  if (ptr==NULL) return -1;
  xml=blk->xml;

  if (strcmp(name,blk->name)==0) {
    if (ptr->dash.user !=NULL) s=(ptr->dash.user)(NULL,ptr->dash.data);
    if (ptr->clp.px !=NULL) free(ptr->clp.px);
    if (ptr->clp.py !=NULL) free(ptr->clp.py);
    if (ptr->clp.t !=NULL) free(ptr->clp.t);
    if (s !=0) return -1;
    free(ptr);
    RplotInRestore(blk);
    return XMLCallEnd(xml,name,buf,sze);
  }

  if (ptr->pass.user !=NULL) {
    s=(ptr->pass.user)(wrap,1,ptr->pass.data);
    if (s==0) s=(ptr->pass.user)(buf,sze,ptr->pass.data);
    if (s==0) s=(ptr->pass.user)(wrap+1,1,ptr->pass.data);
  }

  return 0;

}


