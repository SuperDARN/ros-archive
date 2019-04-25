/* tagdb.c
   ======= 
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
#include "entity.h"
#include "tagdb.h"

/*
 $Log: tagdb.c,v $
 Revision 1.3  2004/08/25 20:13:25  barnes
 Initialized t to a NULL pointer.

 Revision 1.2  2004/04/26 20:24:09  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 17:30:01  barnes
 Initial revision

*/

struct TagDBtable *TagDBMakeTable() {
  struct TagDBtable *ptr;
  ptr=malloc(sizeof(struct TagDBtable));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->tag=NULL;
  return ptr;
}

void TagDBFreeTable(struct TagDBtable *ptr) {
  int i;
  struct TagDBtag *t=NULL;
  if (ptr==NULL) return;
  
  for (i=0;i<ptr->num;i++) {
    if ((ptr->tag !=NULL) && (ptr->tag[i] !=NULL)) {
      t=ptr->tag[i];
      if (t->name !=NULL) free(t->name);
      if (t->start.encode !=NULL) XMLDBFreeBuffer(t->start.encode);
      if (t->end.encode !=NULL) XMLDBFreeBuffer(t->end.encode);
      if (t->start.decode !=NULL) XMLDBFreeBuffer(t->start.decode);
      if (t->end.decode !=NULL) XMLDBFreeBuffer(t->end.decode);
    }
  }
  if (ptr->tag !=NULL) free(ptr->tag);
  free(ptr);
}

int TagDBAddTable(struct TagDBtable *ptr,
             struct XMLDBbuffer *name,struct XMLDBbuffer *start,
             struct XMLDBbuffer *end) {


  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};

  int n,s=0;
  struct TagDBtag **tmp;
  struct TagDBtag *t;
  struct Entitydata *entptr;

  if (ptr==NULL) return -1;
  
  if (ptr->tag==NULL) ptr->tag=malloc(sizeof(struct TagDBtag *));
  else {
    tmp=realloc(ptr->tag,sizeof(struct TagDBtag *)*(ptr->num+1));
    if (tmp==NULL) return -1;
    ptr->tag=tmp;
  }
  if (ptr->tag==NULL) return -1;
  n=ptr->num;
  ptr->tag[n]=malloc(sizeof(struct TagDBtag));
  if (ptr->tag[n]==NULL) return -1;

  
  t=ptr->tag[n];
  
  t->name=malloc(name->sze+1);
  if (t->name==NULL) return -1;
  
  memcpy(t->name,name->buf,name->sze);
  t->name[name->sze]=0;

  t->start.encode=NULL;
  t->end.encode=NULL;
  t->start.decode=NULL;
  t->end.decode=NULL;

  entptr=EntityMake(); 
  if (entptr==NULL) return -1;
  s=EntityAddEntityStringArray(entptr,entity);
  s=EntityAddSymbolStringArray(entptr,symbol);
  
  if (s !=0) {
    EntityFree(entptr);
    return -1;
  }

  if (start !=NULL) {
   
     t->start.encode=XMLDBCopyBuffer(start);
     if (t->start.encode==NULL) {
       EntityFree(entptr);
       return -1;
     }
     t->start.decode=EntityDecodeBuffer(entptr,start);
     if (t->start.decode==NULL) {
        EntityFree(entptr);
        return -1;
     }      
  }
  if (end !=NULL) {
    
    t->end.encode=XMLDBCopyBuffer(end);
    if (t->end.encode==NULL) {
        EntityFree(entptr);
        return -1;
    }    
    t->end.decode=EntityDecodeBuffer(entptr,end);
    if (t->end.decode==NULL) {
        EntityFree(entptr);
        return -1;
    } 
  
    
  }
  EntityFree(entptr); 
  ptr->num++; 
  return 0;
}

int TagDBBuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int status=0;
  int i;
  struct XMLDBelement *e;
  struct XMLDBbuffer *name=NULL;
  struct XMLDBbuffer *start=NULL; 
  struct XMLDBbuffer *end=NULL;

  struct TagDBtable *t=NULL;
  t=(struct TagDBtable *) data;
 
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"name")==0) name=e->data;
    if (strcmp(e->name,"start")==0) start=e->data;
    if (strcmp(e->name,"end")==0) end=e->data;
  }  
  
  status=TagDBAddTable(t,name,start,end);
  
  return status;
}
  

int TagDBText(char *buf,int sze,void *data) {
  struct TagDBdata *ptr;
  ptr=(struct TagDBdata *) data;
  if (ptr->text.user==NULL) return 0;
  return (ptr->text.user)(buf,sze,ptr->text.data);
 
}

int TagDBEnd(char *name,char *buf,int sze,void *data) {
  int i,s=0;
  struct TagDBdata *ptr;
  struct TagDBtable *tagdb;
  struct TagDBtag *t=NULL;
    
  ptr=(struct TagDBdata *)data;
 
  
  if (ptr->text.user==NULL) return 0;

  tagdb=ptr->tagdb;
  if (tagdb !=NULL) {
    for (i=0;i<tagdb->num;i++) {
      t=tagdb->tag[i];
      if (strcmp(name,t->name)==0) break;
    }
    if (i<tagdb->num) { 
       s=(ptr->text.user)((t->end.decode)->buf,
                           (t->end.decode)->sze,ptr->text.data);
       return s;
    }
  }

  s=(ptr->text.user)("</",2,ptr->text.data);
  if (s==0) s=(ptr->text.user)(name,strlen(name),ptr->text.data);
  if (s==0) s=(ptr->text.user)(">",1,ptr->text.data);
  return s;
  
}


int TagDBStart(char *name,int atnum,char **atname,char **atval,
                char *buf,int sze,void *data) {
  int i;
 
  struct TagDBdata *ptr;
  struct TagDBtable *tagdb;
  struct TagDBtag *t=NULL;
  
  ptr=(struct TagDBdata *) data;

  if (ptr->text.user==NULL) return 0;

  tagdb=ptr->tagdb;

  if (tagdb !=NULL) {
    for (i=0;i<tagdb->num;i++) {
      t=tagdb->tag[i];
      if (strcmp(name,t->name)==0) break;
    }
    if (i<tagdb->num) {
      /* buffer attributes */
  
      struct XMLDBbuffer *abuf=NULL;
      struct XMLDBbuffer *tbuf=NULL;
      struct Entitydata *entptr;

      entptr=EntityMake();
      if (entptr==NULL) return -1;
      EntityAddEntityString(entptr,"&attr;");
    
      abuf=XMLDBMakeBuffer(100);
    
      for (i=0;i<atnum;i++) {
        if (i !=0) XMLDBAddBuffer(abuf," ",1);
        XMLDBAddBuffer(abuf,atname[i],strlen(atname[i]));
        if (atval[i] !=0) {
          XMLDBAddBuffer(abuf,"=\"",2);
          XMLDBAddBuffer(abuf,atval[i],strlen(atval[i]));
          XMLDBAddBuffer(abuf,"\"",1);
        }
      
      }
      EntityAddSymbol(entptr,abuf);

      tbuf=EntityDecodeBuffer(entptr,t->start.decode); 
     
      if (tbuf==NULL) return -1;

      (ptr->text.user)(tbuf->buf,
                      tbuf->sze,ptr->text.data);
      XMLDBFreeBuffer(tbuf);
      XMLDBFreeBuffer(abuf);
      EntityFree(entptr);
      return 0;
    }
  }
  (ptr->text.user)("<",1,ptr->text.data);
  (ptr->text.user)(name,strlen(name),ptr->text.data);
 
  for (i=0;i<atnum;i++) {
   
    (ptr->text.user)(" ",1,ptr->text.data);
    (ptr->text.user)(atname[i],strlen(atname[i]),ptr->text.data);
    if (atval[i] !=0) {
      (ptr->text.user)("=\"",2,ptr->text.data);
      (ptr->text.user)(atval[i],strlen(atval[i]),ptr->text.data);
      (ptr->text.user)("\"",1,ptr->text.data);
    }
  }
  (ptr->text.user)(">",1,ptr->text.data);
  return 0;
}

struct TagDBdata *TagDBMake(struct TagDBtable *tagdb) {
  struct TagDBdata *ptr=NULL;

  ptr=malloc(sizeof(struct TagDBdata));
  if (ptr==NULL) return NULL;

  ptr->ptr=XMLMake();
  if (ptr->ptr==NULL) {
    free(ptr);
    return NULL;
  }

  
  ptr->tagdb=tagdb;  
  ptr->text.user=NULL;
  ptr->text.data=NULL;
  XMLSetStart(ptr->ptr,TagDBStart,ptr);
  XMLSetEnd(ptr->ptr,TagDBEnd,ptr);
  XMLSetText(ptr->ptr,TagDBText,ptr);
  return ptr;
}

void TagDBFree(struct TagDBdata *ptr) {
  if (ptr==NULL) return;
  free(ptr);
}


int TagDBSetTable(struct TagDBdata *ptr,struct TagDBtable *tagdb) {
  if (ptr==NULL) return -1;
  ptr->tagdb=tagdb;
  return 0;
} 


int TagDBSetText(struct TagDBdata *ptr,
             int (*text)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->text.user=text;
  ptr->text.data=data;
  return 0;
} 

int TagDBDecode(char *buf,int sze,void *data) {
  struct TagDBdata *tag;
  tag=(struct TagDBdata *)data;
  return XMLDecode(tag->ptr,buf,sze);
}
