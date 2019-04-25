/* table.c
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

/*
 $Log: table.c,v $
 Revision 1.2  2004/04/26 20:27:45  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 17:42:50  barnes
 Initial revision

*/

struct XMLDBtable *XMLDBMakeTable() {
 
  struct XMLDBtable *ptr;

  ptr=malloc(sizeof(struct XMLDBtable));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->element=NULL; 
  return ptr;
}

int XMLDBAddTable(struct XMLDBtable *ptr,struct XMLDBelement *element) {
  struct XMLDBelement **tmp;
  if (ptr==NULL) return -1;
  if (ptr->element==NULL) 
      ptr->element=malloc(sizeof(struct XMLDBelement *));
  else {
    tmp=realloc(ptr->element,
                sizeof(struct XMLDBelement *)*(ptr->num+1));
    if (tmp==NULL) return -1; 
    ptr->element=tmp;
  }

  if (ptr->element==NULL) return -1;
  ptr->element[ptr->num]=element;
  ptr->num++;
  return 0;
}


int XMLDBRemoveTable(struct XMLDBtable *ptr) {
  struct XMLDBelement **tmp;

  if (ptr==NULL) return -1;
  if (ptr->element==NULL) return -1;
  
  ptr->num--;
  if (ptr->num<0) return -1;
  
  if (ptr->element[ptr->num] !=NULL) XMLDBFreeElement(ptr->element[ptr->num]);
  

  if (ptr->num >0) {
     tmp=realloc(ptr->element,
                          sizeof(struct XMLDBelement *)*(ptr->num));
     if (tmp==NULL) return -1;
     ptr->element=tmp;
  } else {
    free(ptr->element);
    ptr->element=NULL;
  }
  return 0;
}




void XMLDBFreeTable(struct XMLDBtable *ptr) {
  int i;

  if (ptr==NULL) return;
  
  if (ptr->element !=NULL) {
    for (i=0;i<ptr->num;i++) 
      if (ptr->element[i] !=NULL) XMLDBFreeElement(ptr->element[i]);
    free (ptr->element);
  }
  free(ptr);

}







  
