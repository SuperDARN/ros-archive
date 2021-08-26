/* tree.c
   ====== 
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
 $Log: tree.c,v $
 Revision 1.1  2003/03/01 17:42:50  barnes
 Initial revision

*/

struct XMLDBtree *XMLDBMakeTree() {
  struct XMLDBtree *ptr;

  ptr=malloc(sizeof(struct XMLDBtree));
  if (ptr==NULL) return NULL;
  ptr->depth=0;
  ptr->name=NULL;

  return ptr;
}

void XMLDBFreeTree(struct XMLDBtree *ptr) {
  int i;

  if (ptr==NULL) return;
  if (ptr->name !=NULL) {
    for (i=0;i<ptr->depth;i++) if (ptr->name[i] !=NULL) free(ptr->name[i]);
    free(ptr->name);
  }
  free(ptr);
}

int XMLDBBuildTree(char *path,char delim,struct XMLDBtree *ptr) {

  int c=0;
  int s=0;
  int i;
  char **tmp;

  ptr->depth=0;
  if (ptr->name !=NULL) {
    for (i=0;i<ptr->depth;i++) if (ptr->name[i] !=NULL) free(ptr->name[i]);
    free(ptr->name);
    ptr->name=NULL;
  }

  while (c<strlen(path)) {
    if (path[c]==delim) {
      if ((c-s)>0) {
        if (ptr->name==NULL) ptr->name=malloc(sizeof(char *));
        else {
          tmp=realloc(ptr->name,sizeof(char *)*
                                 (ptr->depth+1));
          if (tmp==NULL) return -1;
          ptr->name=tmp;
	}
        if (ptr->name==NULL) return -1;
        ptr->name[ptr->depth]=malloc(c-s+1);
        if (ptr->name[ptr->depth]==NULL) return -1;
        memcpy(ptr->name[ptr->depth],path+s,c-s);
        ptr->name[ptr->depth][c-s]=0;
        ptr->depth++;
        s=c+1;
      }
    }
    c++;
  }    

  if ((c-s)>0) {
    if (ptr->name==NULL) ptr->name=malloc(sizeof(char *));
      else {
        tmp=realloc(ptr->name,sizeof(char *)*(ptr->depth+1));
        if (tmp==NULL) return -1;
        ptr->name=tmp;
      }
    if (ptr->name==NULL) return -1;

    ptr->name[ptr->depth]=malloc(c-s+1);
    if (ptr->name[ptr->depth]==NULL) return -1;
    memcpy(ptr->name[ptr->depth],path+s,c-s);
    ptr->name[ptr->depth][c-s]=0;
    ptr->depth++;
  }
  return 0;
}
