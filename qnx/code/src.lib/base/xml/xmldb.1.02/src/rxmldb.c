/* rxmldb.c
   ======== 
   Author R.J.Barnes
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
#include "rxmldb.h"

/*
 $Log: rxmldb.c,v $
 Revision 1.2  2004/04/26 20:26:06  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 17:42:50  barnes
 Initial revision

*/

struct XMLDBdata *XMLDBMake(struct XMLdata *xml) {

  struct XMLDBdata *ptr;
  ptr=malloc(sizeof(struct XMLDBdata));
  if (ptr==NULL) return NULL;
 
  ptr->ptr=xml;
  ptr->doc=XMLDBMakeTable();

  if (ptr->doc==NULL) {
    free(ptr);
    return NULL;
  }

  ptr->table=NULL;
  ptr->buf=NULL;
  ptr->tree=NULL;
  ptr->depth=0;
  ptr->text.user=NULL;
  ptr->text.data=NULL;
  return ptr;
}

void XMLDBFree(struct XMLDBdata *ptr) {
  if (ptr==NULL) return;
  if (ptr->table !=NULL) XMLDBFreeTable(ptr->table);
  if (ptr->doc !=NULL) XMLDBFreeTable(ptr->doc);
  free(ptr);
}

int XMLDBSetTree(struct XMLDBdata *ptr,struct XMLDBtree *tree) {
  if (ptr==NULL) return -1;
  ptr->tree=tree;
  return 0;
}

int XMLDBSetText(struct XMLDBdata *ptr,
             int (*text)(struct XMLDBtable *,struct XMLDBtable *,
                         void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->text.user=text;
  ptr->text.data=data;
  return 0;
} 


int XMLDBElementStart(char *name,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {
  int i;
  int status=0;
  struct XMLDBdata *x;
  struct XMLDBbuffer *b;
  x=(struct XMLDBdata *) data;
  x->depth++;
 
  b=x->buf;

  status=XMLDBAddBuffer(b,"<",1);
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,name,strlen(name));
  if (status !=0) return status;

 
  for (i=0;i<atnum;i++) {
    status=XMLDBAddBuffer(b," ",1);
    if (status !=0) return status;
    status=XMLDBAddBuffer(b,atname[i],strlen(atname[i]));
    if (status !=0) return status;

    if (atval[i]!=NULL) {
      status=XMLDBAddBuffer(b,"=\"",2);
      if (status !=0) return status;
      status=XMLDBAddBuffer(b,atval[i],strlen(atval[i]));
      if (status !=0) return status;
      status=XMLDBAddBuffer(b,"\"",1);
      if (status !=0) return status;
    }
  }
  status=XMLDBAddBuffer(b,">",1);

  return status;
}



int XMLDBElementEnd(char *name,char *buf,int sze,void *data) {
  int status=0;
  struct XMLDBdata *x;
  struct XMLDBbuffer *b;
  x=(struct XMLDBdata *) data;
  x->depth--;
  b=x->buf;

  if (x->depth==0) { /* reset the userr */
    x->depth++; 
    XMLSetStart(x->ptr,XMLDBPeerStart,x);
    XMLSetEnd(x->ptr,XMLDBPeerEnd,x);
    XMLSetText(x->ptr,NULL,NULL);
    status=XMLDBPeerEnd(name,buf,sze,data);
    return status;

  }
  
  status=XMLDBAddBuffer(b,"</",2);
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,name,strlen(name));
  if (status !=0) return status;
  status=XMLDBAddBuffer(b,">",1);
  return status;
 
}

int XMLDBElementText(char *buf,int sze,void *data) {
  int status=0;
  struct XMLDBdata *x;
  struct XMLDBbuffer *b;
  x=(struct XMLDBdata *) data;
  b=x->buf;
  status=XMLDBAddBuffer(b,buf,sze);
  return status;
}


int XMLDBPeerStart(char *name,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {

  
  struct XMLDBdata *x;
  struct XMLDBelement *e;


  x=(struct XMLDBdata *) data;
  x->depth++;

  e=XMLDBMakeElement(name,atnum,atname,atval,100);
  if (e==NULL) return -1;
  if (x->table==NULL) x->table=XMLDBMakeTable();
 
  if (x->table==NULL) {
     XMLDBFreeElement(e);
     return -1;
  }
  
  if (XMLDBAddTable(x->table,e) !=0) {
    XMLDBFreeElement(e);
    return -1;
  }
  x->buf=e->data;

  XMLSetStart(x->ptr,XMLDBElementStart,x);
  XMLSetEnd(x->ptr,XMLDBElementEnd,x);
  XMLSetText(x->ptr,XMLDBElementText,x);

  return 0;
}

int XMLDBPeerEnd(char *name,char *buf,int sze,void *data) {

  int status=0;
  struct XMLDBdata *x;
  x=(struct XMLDBdata *) data;
  x->depth--; 
 
  if (x->depth<0) { 

    /* we have read in all the peers at this level */

    
    if ((x->text.user !=NULL)) 
      status=(x->text.user)(x->table,x->doc,x->text.data);
      
    XMLDBFreeTable(x->table);
    x->table=NULL;

    if (status !=0) return status;  
    XMLSetStart(x->ptr,XMLDBStart,x);
    XMLSetEnd(x->ptr,XMLDBEnd,x);
    status=XMLDBEnd(name,buf,sze,data);
  }
  return status;
}





int XMLDBStart(char *name,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {
  int i=0;

  struct XMLDBdata *x;
  struct XMLDBtree *t;
  struct XMLDBtable *d;
  struct XMLDBelement *e;
 
  x=(struct XMLDBdata *) data;
  t=x->tree;
  d=x->doc;

  e=XMLDBMakeElement(name,atnum,atname,atval,10);
  if (e==NULL) return -1;
  
  if (XMLDBAddTable(d,e) !=0) {
    XMLDBFreeElement(e);
    return -1;
  }


  if (d->num==t->depth) {
    for (i=0;i<t->depth;i++) 
      if (strcmp(t->name[i],
                (d->element[i])->name) !=0) break;
  }

  if (i==t->depth) {
     /* we are now in the right depth */
      x->depth=0;
      XMLSetStart(x->ptr,XMLDBPeerStart,data);
      XMLSetEnd(x->ptr,XMLDBPeerEnd,data);
  }
  return 0;
 
}

int XMLDBEnd(char *name,char *buf,int sze,void *data) {

  struct XMLDBdata *x;
  struct XMLDBtable *t;
  
  x=(struct XMLDBdata *) data;
  t=x->doc;

  if (t->num==0) return -1;

  XMLDBRemoveTable(t);
  
  return 0; 

}

















