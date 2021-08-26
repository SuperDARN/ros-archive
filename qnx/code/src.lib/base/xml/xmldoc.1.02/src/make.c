/* make.c
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
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"

/*
 $Log: make.c,v $
 Revision 1.1  2003/03/01 17:49:53  barnes
 Initial revision

*/

struct XMLdocdata *XMLdocMake() {
  struct XMLdocdata *ptr=NULL;

  ptr=malloc(sizeof(struct XMLdocdata));
  memset(ptr,0,sizeof(struct XMLdocdata));

  /* set up root element handler */

  ptr->ent=EntityMake();
  ptr->tag=TagDBMake(NULL);
  TagDBSetText(ptr->tag,EntityDecode,ptr->ent);
  return ptr;

}

void XMLdocFreeMod(struct XMLdocmod *ptr) {
  if (ptr==NULL) return;
  if (ptr->name !=NULL) free(ptr->name);
  if (ptr->path !=NULL) free(ptr->path);
  if (ptr->tagdb !=NULL) TagDBFreeTable(ptr->tagdb);
  if (ptr->action !=NULL) XMLDBFreeBuffer(ptr->action);
  free(ptr);
}

void XMLdocFreeElement(struct XMLdocelement *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->name !=NULL) free(ptr->name);
  if (ptr->path !=NULL) free(ptr->path);
  if (ptr->tagdb !=NULL) TagDBFreeTable(ptr->tagdb);
  if (ptr->action !=NULL) XMLDBFreeBuffer(ptr->action);
  if (ptr->defval !=NULL) XMLDBFreeBuffer(ptr->defval);
  if (ptr->mod.ptr !=NULL) {
    for (i=0;i<ptr->mod.num;i++) {
      XMLdocFreeMod(ptr->mod.ptr[i]);
    }
    free(ptr->mod.ptr);
  }
  free(ptr);
}





void XMLdocFree(struct XMLdocdata *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->ent !=NULL) EntityFree(ptr->ent);
  if (ptr->tag !=NULL) TagDBFree(ptr->tag);
  if (ptr->tagdb !=NULL) TagDBFreeTable(ptr->tagdb);
  

  /* free up the rest of this stuff */
  if (ptr->root !=NULL) XMLDBFreeBuffer(ptr->root);
  if (ptr->xml.ptr !=NULL) {
    for (i=0;i<ptr->xml.num;i++) {
      XMLdocFreeElement(ptr->xml.ptr[i]);
    }
    free(ptr->xml.ptr);
  }
  free(ptr);
  return;
}








